#include "Model.h"
#include "Mesh.h"
#include "Texture.h"
#include "Bone.h"
#include "Animation.h"
#include "Channel.h"
#include "Shader.h"
#include "GameObject.h"
#include "SMath.h"

CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CModel::CModel(const CModel & rhs)
	: CComponent(rhs)
	, m_PivotMatrix(rhs.m_PivotMatrix)
	, m_eModelType(rhs.m_eModelType)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_Meshes(rhs.m_Meshes)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_Materials(rhs.m_Materials)
	, m_iNumAnimations(rhs.m_iNumAnimations)
	, m_fModelWidth(rhs.m_fModelWidth)
	, m_fModelHeight(rhs.m_fModelHeight)

{
	for (auto& pPrototypeAnimation : rhs.m_Animations)
		m_Animations.push_back(pPrototypeAnimation->Clone());

	for (auto& pPrototypeBone : rhs.m_Bones)
		m_Bones.push_back(pPrototypeBone->Clone());

	for (auto& MaterialDesc : m_Materials)
	{
		for (auto& pTexture : MaterialDesc.pMtrlTextures)
			Safe_AddRef(pTexture);
	}

	for (auto& pMesh : m_Meshes)
	{
		Safe_AddRef(pMesh);
	}

	//XMStoreFloat4x4(&m_UpperSpineMatrix, XMMatrixRotationX(XMConvertToRadians(5.0f))); 
	//XMStoreFloat4x4(&m_UpperSpineMatrix, XMMatrixIdentity()); // 	

	//Y - Right
	//X - Up
	//Z - Look


}

_uint CModel::Get_MaterialIndex(_uint iMeshIndex)
{
	return m_Meshes[iMeshIndex]->Get_MaterialIndex();
}

_uint CModel::Get_NumMeshIndice(_int iMeshIndex)
{
	 return m_Meshes[iMeshIndex]->Get_NumIndices();
}

CMesh* CModel::Get_Mesh_For_Index(_int iMeshIndex)
{
	return m_Meshes[iMeshIndex];
}

CAnimation* CModel::Get_Animation_For_Index(_uint iAnimIndex)
{
	return m_Animations[iAnimIndex];
}

_float4x4* CModel::Get_Combined_For_AnimationIndex(_uint iAnimationIndex, _float fTrackPosition)
{

	return nullptr;
	//return m_Animations[iAnimationIndex]->Get_TransformationBoneMatrices(fTrackPosition);
}

_float4x4* CModel::Calc_OffsetMatrice(_uint iAnimationIndex, _float fTrackPosition, _float4x4* pMatrix)
{
	
	_float4x4* pCalcMatrix = pMatrix;
	_uint iNumBones = (_uint)m_Bones.size();


	for (_uint i = 0; i < iNumBones; ++i)
	{
		pCalcMatrix[i] = m_Bones[i]->Get_CombinedTransformationFloat4x4();
	}


	pMatrix = m_Animations[iAnimationIndex]->Get_TransformationBoneMatrices(fTrackPosition, pCalcMatrix);


//_float4x4* pOffsetMatrices = Get_OffsetMatrices();
//
//for (_int i = 0; i < m_iNumMeshes; ++i)
//{
//	vector<_float4x4> m_OffsetMatrices = m_Meshes[i]->Get_OffsetMatrices();
//	vector<_uint> vecIndices = m_Meshes[i]->Get_BoneIndices();
//	_int iNumBones = m_Meshes[i]->Get_NumBones();
//
//	for (_int j = 0; j < iNumBones; ++j)
//	{
//		XMStoreFloat4x4(&pMatrix[vecIndices[j]], XMLoadFloat4x4(&pMatrix[vecIndices[j]]) * XMLoadFloat4x4(&m_OffsetMatrices[j]));
//	}
//}


	return pMatrix;
}

_float4x4* CModel::Get_OffsetMatrices()
{
	_float4x4 BoneMatrices[800];
	
	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		vector<_float4x4> m_OffsetMatrices = m_Meshes[i]->Get_OffsetMatrices();
		vector<_uint> vecIndices = m_Meshes[i]->Get_BoneIndices();
		_int iNumBones = m_Meshes[i]->Get_NumBones();

		for (_int j = 0; j < iNumBones; ++j)
		{
			BoneMatrices[vecIndices[j]] = m_OffsetMatrices[j];
		}
	}

	return BoneMatrices;
}

_float3& CModel::Calculate_AABB_Extents_From_Model()
{
	_float3 vMin = XMFLOAT3(FLT_MAX, FLT_MAX, FLT_MAX);
	_float3 vMax = XMFLOAT3(FLT_MIN, FLT_MIN, FLT_MIN);

	// 모델의 모든 메쉬에 대해 AABB 계산

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
			m_Meshes[i]->Calculate_AABB_Extents(&vMin, &vMax);
	}

	_float3 vExtents = (vMin - vMax) * 0.5f;

	return vExtents;
}

void CModel::Calculate_Sphere_Radius(_float3* vOutCenter, _float* fOutRadius)
{
	_float fMaxRadius = 0.0f;
	_float3 Min, Max;
	
	if (m_eModelType == CModel::TYPE_ANIM)
	{
		for (size_t i = 0; i < m_iNumMeshes; ++i)
		{
			m_Meshes[i]->Calculate_AABB_Extents(&Min, &Max);

			// 중심점 계산
			_float3 vCenter = (Min + Max) * 0.5f;


			_uint iNumVertices = m_Meshes[i]->Get_NumVertices();
			VTXANIMMESH* pAnimVertices = m_Meshes[i]->Get_AnimVertices();
			// 중심점으로부터 각 정점까지의 거리 계산
			for (_uint j = 0; j < iNumVertices; ++j)
			{
				_float3 VertexPos = pAnimVertices[j].vPosition;
				fMaxRadius = max(fMaxRadius, XMVector3Length(XMLoadFloat3(&VertexPos) - XMLoadFloat3(&vCenter)).m128_f32[0]);
			}
		}

	}
	else if (m_eModelType == CModel::TYPE_NONANIM)
	{
		for (size_t i = 0; i < m_iNumMeshes; ++i)
		{
			m_Meshes[i]->Calculate_AABB_Extents(&Min, &Max);

			// 중심점 계산
			_float3 vCenter = (Min + Max) * 0.5f;


			_uint iNumVertices = m_Meshes[i]->Get_NumVertices();
			VTXMESH* pVertices = m_Meshes[i]->Get_Vertices();
			// 중심점으로부터 각 정점까지의 거리 계산
			for (_uint j = 0; j < iNumVertices; ++j)
			{
				_float3 VertexPos = pVertices[j].vPosition;
				fMaxRadius = max(fMaxRadius, XMVector3Length(XMLoadFloat3(&VertexPos) - XMLoadFloat3(&vCenter)).m128_f32[0]);
			}
		}
	}
	
	if (vOutCenter != nullptr)
		*vOutCenter = (Min + Max) * 0.5f;

	if(fOutRadius != nullptr)
		*fOutRadius = fMaxRadius;
}

void CModel::Calculate_ModelSize(_float* fOutWidth, _float* fOutHeight)
{
	float3 vMin = XMFLOAT3(FLT_MAX, FLT_MAX, FLT_MAX);
	_float3 vMax = XMFLOAT3(FLT_MIN, FLT_MIN, FLT_MIN);

	// 모델의 모든 메쉬에 대해 AABB 계산

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		m_Meshes[i]->Calculate_AABB_Extents(&vMin, &vMax);
	}

	// 가로 및 세로 크기 계산
	*fOutWidth = vMax.x - vMin.x;
	*fOutHeight = vMax.y - vMin.y;
}

void CModel::Set_MouseMove(_float2 vMouseMove)
{
	m_vMouseMove = vMouseMove;
}

CBone * CModel::Get_BonePtr(const _char * pBoneName) const
{
	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)
	{
		if (!strcmp(pBone->Get_Name(), pBoneName))
			return true;
		return false;
	});

	if (iter == m_Bones.end())
		return nullptr;

	return *iter;
}

void CModel::Set_StiffnessRate(_float fStiffnessRate)
{
	m_Animations[m_iCurrentAnimIndex]->Set_StiffnessRate(fStiffnessRate);
}

void CModel::Set_StiffnessRate_Upper(_float fStiffnessRate)
{
	m_Animations[m_iUpperAnimIndex]->Set_StiffnessRate(fStiffnessRate);
}

_float CModel::Get_StiffnessRate()
{
	return	m_Animations[m_iCurrentAnimIndex]->Get_StiffnessRate();	
}

_matrix CModel::Get_CombinedMatrix(_uint iBoneIndex)
{
	return m_Bones[iBoneIndex]->Get_CombinedTransformationMatrix();
}

HRESULT CModel::Initialize_Prototype(TYPE eType, const string & strModelFilePath, _fmatrix PivotMatrix)
{
	m_eModelType = eType;

	_uint	iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

	if (TYPE_NONANIM == eType)
		iFlag |= aiProcess_PreTransformVertices;


	/*TODOm_pAIScene = m_MyAssimp.ReadFile(strModelFilePath, iFlag, false); */ //! 무조건 FBX 로드, TRUE 일 시 BINARY 로드

	m_pAIScene = m_MyAssimp.ReadFile(strModelFilePath, iFlag);

	//if (nullptr == m_pAIScene.Get_AIScene())
	//	return E_FAIL;

	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	CMyAINode root = m_pAIScene.Get_RootNode();

	if (FAILED(Ready_Bones(m_pAIScene.Get_RootNode(), -1)))
		return E_FAIL;

	if (FAILED(Ready_Meshes(PivotMatrix)))
		return E_FAIL;

	if (FAILED(Ready_Materials(strModelFilePath)))
		return E_FAIL;

	if (FAILED(Ready_Animations()))
		return E_FAIL;

	Write_Names(strModelFilePath);

	Calculate_ModelSize(&m_fModelWidth, &m_fModelHeight);

	return S_OK;
}

HRESULT CModel::Initialize(void * pArg)
{

	
	return S_OK;
}

void CModel::Play_Animation(_float fTimeDelta, _bool bIsLoop)
{
	if (m_iCurrentAnimIndex >= m_iNumAnimations)
		return;

	if (true == bIsLoop)
		m_eAnimState = ANIM_STATE::ANIM_STATE_LOOP;
	else
		m_eAnimState = ANIM_STATE::ANIM_STATE_NORMAL;

	m_bIsAnimEnd = m_Animations[m_iCurrentAnimIndex]->Invalidate_TransformationMatrix(m_eAnimState, fTimeDelta, m_Bones, m_bIsSplitted);


	_float3 NowPos;
	for (auto& pBone : m_Bones)
	{
		pBone->Invalidate_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PivotMatrix), NowPos);
	}

	if (true == m_bUseAnimationPos && false == m_bIsAnimEnd && false == Is_Transition())
	{
		if (false == m_Animations[m_iCurrentAnimIndex]->Is_TransitionEnd_Now())
		{
			_float3 ChangedPos = NowPos - m_Animations[m_iCurrentAnimIndex]->Get_PrevPos();
			//_Pos = ChangedPos;
		}

		m_Animations[m_iCurrentAnimIndex]->Set_PrevPos(NowPos);
	}
}

void CModel::Play_Animation(_float fTimeDelta, _float3& _Pos)
{
	if (m_iCurrentAnimIndex >= m_iNumAnimations)
		return;

	m_bIsAnimEnd = m_Animations[m_iCurrentAnimIndex]->Invalidate_TransformationMatrix(m_eAnimState, fTimeDelta, m_Bones, m_bIsSplitted);
	if (true == m_bIsSplitted)
	{
		//HERE
		m_bIsUpperAnimEnd = m_Animations[m_iUpperAnimIndex]->Invalidate_TransformationMatrix_Upper(m_eUpperAnimState, fTimeDelta, m_Bones, m_vMouseMove);
	}


	_float3 NowPos;
	for (auto& pBone : m_Bones)
	{
		pBone->Invalidate_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PivotMatrix), NowPos);
	}

	if (true == m_bUseAnimationPos && false == m_bIsAnimEnd && false == Is_Transition())
	{
		if (false == m_Animations[m_iCurrentAnimIndex]->Is_TransitionEnd_Now())
		{
			_float3 ChangedPos = NowPos - m_Animations[m_iCurrentAnimIndex]->Get_PrevPos();
			_Pos = ChangedPos;
		}

		m_Animations[m_iCurrentAnimIndex]->Set_PrevPos(NowPos);
	}

}

void CModel::Play_Animation(_float fTimeDelta, _float3& _Pos,_float3 pPlayerPos)
{
	if (m_iCurrentAnimIndex >= m_iNumAnimations)
		return;

	//m_bIsAnimEnd = m_Animations[m_iCurrentAnimIndex]->Invalidate_TransformationMatrix(m_eAnimState, fTimeDelta, m_Bones, m_bIsSplitted);
	
		//HERE
	m_bIsAnimEnd = m_Animations[m_iCurrentAnimIndex]->Invalidate_TransformationMatrix_Parasiter(m_eAnimState, fTimeDelta, m_Bones, pPlayerPos);
	
	

	_float3 NowPos;
	for (auto& pBone : m_Bones)
	{
		pBone->Invalidate_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PivotMatrix), NowPos);
	}

	if (true == m_bUseAnimationPos && false == m_bIsAnimEnd && false == Is_Transition())
	{
		if (false == m_Animations[m_iCurrentAnimIndex]->Is_TransitionEnd_Now())
		{
			_float3 ChangedPos = NowPos - m_Animations[m_iCurrentAnimIndex]->Get_PrevPos();
			_Pos = ChangedPos;
		}

		m_Animations[m_iCurrentAnimIndex]->Set_PrevPos(NowPos);
	}

}

HRESULT CModel::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, _uint iMeshIndex, _float4x4* BoneMatrices)
{
	if (BoneMatrices != nullptr)
		return m_Meshes[iMeshIndex]->Bind_BoneMatrices(pShader, pConstantName, m_Bones, BoneMatrices);
	else
	{
		return m_Meshes[iMeshIndex]->Bind_BoneMatrices(pShader, pConstantName, m_Bones, nullptr);
	}
}

HRESULT CModel::Bind_MaterialResource(CShader* pShader, _uint iMeshIndex)
{
	// Bone도 연계하고싶은데 Anim만 연계되는거고 NonAnim은 들어가면 안되서 터지니까 안넣음. 하 

	/* 해당 메시가 가진 MaterialIndex*/
	_uint		iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();
	if (iMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	MATERIAL_DESC& material = m_Materials[iMaterialIndex];

	for (_int i = 0; i < (_int)AI_TEXTURE_TYPE_MAX; ++i)
	{
		if (nullptr == material.pMtrlTextures[i])
			continue;

		switch (i)
		{
		case (_int)aiTextureType_DIFFUSE:
			Bind_ShaderResource(pShader, "g_DiffuseTexture", iMeshIndex, aiTextureType_DIFFUSE);
			break;
		case (_int)aiTextureType_SPECULAR:
			Bind_ShaderResource(pShader, "g_SpecularTexture", iMeshIndex, aiTextureType_SPECULAR);
			break;
		case (_int)aiTextureType_EMISSIVE:
			Bind_ShaderResource(pShader, "g_EmissiveTexture", iMeshIndex, aiTextureType_EMISSIVE);
			break;
		case (_int)aiTextureType_NORMALS:
			Bind_ShaderResource(pShader, "g_NormalTexture", iMeshIndex, aiTextureType_NORMALS);
			break;
		case (_int)aiTextureType_OPACITY:
			Bind_ShaderResource(pShader, "g_OpacityTexture", iMeshIndex, aiTextureType_OPACITY);
			break;
		case (_int)aiTextureType_METALNESS:
			Bind_ShaderResource(pShader, "g_MetalicTexture", iMeshIndex, aiTextureType_METALNESS);
			break;
		case (_int)aiTextureType_DIFFUSE_ROUGHNESS:
			Bind_ShaderResource(pShader, "g_RoughnessTexture", iMeshIndex, aiTextureType_DIFFUSE_ROUGHNESS);
			break;
		//case (_int)aiTextureType_AMBIENT_OCCLUSION:
		//	Bind_ShaderResource(pShader, "g_AmbientOcclusionTexture", iMeshIndex, aiTextureType_AMBIENT_OCCLUSION);
		//	break;
		}
		/* AO 컬러는 모델의 디테일과 형태를 강조하는데 사용 - EX. SNOW맵 표지판. PBR에서 쓰는 O는 Opacity를 말한것임. */
	}

	return S_OK;
}

HRESULT CModel::Bind_ShaderResource(CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eTextureType)
{

	_uint		iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();
	if (iMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	return m_Materials[iMaterialIndex].pMtrlTextures[eTextureType]->Bind_ShaderResource(pShader, pConstantName);
}

HRESULT CModel::Bind_ShaderCascade(CShader* pShader)
{
	//if (FAILED(pShader->Bind_Matrices("g_BoneMatrices", m_matCurrTransforms.data(), (size_t)m_matCurrTransforms.size())))
	return S_OK;	
}

void CModel::Set_Animation(_uint _iAnimationIndex, CModel::ANIM_STATE _eAnimState, _bool _bIsTransition, _float _fTransitionDuration, _uint iTargetKeyFrameIndex)
{
	if (m_Animations.size() <= 0)
		return;

	m_eAnimState = _eAnimState;

	if (_iAnimationIndex != m_iCurrentAnimIndex)
	{
		Reset_Animation(_iAnimationIndex);

		if (_bIsTransition)
		{
			Set_Animation_Transition(_iAnimationIndex, _fTransitionDuration, iTargetKeyFrameIndex);
		}
		else
		{
			m_iCurrentAnimIndex = _iAnimationIndex;
			_float fTargetTrackPosition = (*m_Animations[m_iCurrentAnimIndex]->Get_Channels())[0]->Get_KeyFrame(iTargetKeyFrameIndex).fTrackPosition;
			m_Animations[m_iCurrentAnimIndex]->Set_TrackPosition(fTargetTrackPosition);
		}
	}
	else 
	{
		_float fTargetTrackPosition = (*m_Animations[m_iCurrentAnimIndex]->Get_Channels())[0]->Get_KeyFrame(iTargetKeyFrameIndex).fTrackPosition;
		m_Animations[m_iCurrentAnimIndex]->Set_TrackPosition(fTargetTrackPosition);
	}
	//else 
	//{
	//	m_iCurrentAnimIndex = _iAnimationIndex;
	//	_float fTargetTrackPosition = (*m_Animations[m_iCurrentAnimIndex]->Get_Channels())[0]->Get_KeyFrame(iTargetKeyFrameIndex).fTrackPosition;
	//	m_Animations[m_iCurrentAnimIndex]->Set_TrackPosition(fTargetTrackPosition);
	//}
}

void CModel::Set_Animation_Transition(_uint _iAnimationIndex, _float _fTransitionDuration, _uint iTargetKeyFrameIndex)
{
	if (_iAnimationIndex == m_iCurrentAnimIndex)
	{
		_float fTargetTrackPosition = (*m_Animations[m_iCurrentAnimIndex]->Get_Channels())[0]->Get_KeyFrame(iTargetKeyFrameIndex).fTrackPosition;
		m_Animations[m_iCurrentAnimIndex]->Set_TrackPosition(fTargetTrackPosition);
	}

	CAnimation* currentAnimation = m_Animations[m_iCurrentAnimIndex];
	CAnimation* targetAnimation = m_Animations[_iAnimationIndex];

	targetAnimation->Reset_Animation(m_Bones, m_bIsSplitted);		// 임시

	targetAnimation->Set_Transition(currentAnimation, _fTransitionDuration, iTargetKeyFrameIndex, m_bIsSplitted);

	m_iCurrentAnimIndex = _iAnimationIndex;
}

void CModel::Reset_Animation(_int iAnimIndex)
{
	if (iAnimIndex == -1)
		m_Animations[m_iCurrentAnimIndex]->Reset_Animation(m_Bones, m_bIsSplitted);
	else
		m_Animations[iAnimIndex]->Reset_Animation(m_Bones, m_bIsSplitted);
}

void CModel::Set_AnimState(CModel::ANIM_STATE _eAnimState)
{
	m_eAnimState = _eAnimState;
}

void CModel::Set_Animation_Upper(_uint _iAnimationIndex, CModel::ANIM_STATE _eAnimState, _float _fTransitionDuration, _uint iTargetKeyFrameIndex)
{
	m_iUpperAnimIndex = _iAnimationIndex;
	m_eUpperAnimState = _eAnimState;
	Reset_UpperAnimation(_iAnimationIndex);

	

	if (false == m_bIsSplitted)
	{
		m_bIsSplitted = true;
		CAnimation* currentAnimation = m_Animations[m_iCurrentAnimIndex];
		CAnimation* targetAnimation = m_Animations[_iAnimationIndex];

		targetAnimation->Set_Transition_Upper(currentAnimation, _fTransitionDuration, iTargetKeyFrameIndex);
	}
	else 
	{
		CAnimation* pTargetAnimation = m_Animations[m_iUpperAnimIndex];
		CChannel* pChannel = (*pTargetAnimation->Get_Channels())[0];
		_float fTargetTrackPosition = pChannel->Get_KeyFrame(iTargetKeyFrameIndex).fTrackPosition;
		pTargetAnimation->Set_TrackPosition(fTargetTrackPosition);
	}

	

}

void CModel::Reset_UpperAnimation(_int iAnimIndex)
{
	if (iAnimIndex == -1)
		m_Animations[m_iCurrentAnimIndex]->Reset_UpperAnimation(m_Bones);
	else
		m_Animations[iAnimIndex]->Reset_UpperAnimation(m_Bones);
}

_float CModel::Get_TickPerSecond()
{
	return m_Animations[m_iCurrentAnimIndex]->Get_TickPerSecond();
}

void CModel::Set_TickPerSecond(_float _TickPerSecond)
{
	m_Animations[m_iCurrentAnimIndex]->Set_TickPerSecond(_TickPerSecond);
}

_bool CModel::Is_Transition()
{
	return m_Animations[m_iCurrentAnimIndex]->Is_Transition();
}

_bool CModel::Is_Inputable_Front(_uint _iIndexFront)
{
	return m_Animations[m_iCurrentAnimIndex]->Is_Inputable_Front(_iIndexFront);
}

_bool CModel::Compare_TrackPosition_Is_Over(_float fTrackPosition)
{
	return Get_TrackPosition() > fTrackPosition;
}

_float CModel::Get_TrackPosition()
{
	return m_Animations[m_iCurrentAnimIndex]->Get_TrackPosition();
}

void CModel::Set_TrackPosition(_int iNewTrackPosition)
{
	m_Animations[m_iCurrentAnimIndex]->Set_TrackPosition((_float)iNewTrackPosition);
}

void CModel::Write_Names(const string& strModelFilePath)
{
	ofstream osTxt(strModelFilePath + ".txt");

	osTxt << "Meshes: " << endl;
	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		osTxt << i << ". " << m_Meshes[i]->Get_Name() << endl;
	}
	osTxt << endl;

	osTxt << "Materials: " << endl;

	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		CMyAIMaterial pAIMaterial = m_pAIScene.Get_Material(i);

		for (size_t j = 1; j < (size_t)AI_TEXTURE_TYPE_MAX; j++)
		{
			_char		szDrive[MAX_PATH] = "";
			_char		szDirectory[MAX_PATH] = "";

			_splitpath_s(strModelFilePath.c_str(), szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);

			string strPath = pAIMaterial.Get_Textures((_uint)j);
			if (strPath == "")
				continue;

			_char		szFileName[MAX_PATH] = "";
			_char		szEXT[MAX_PATH] = "";

			_splitpath_s(strPath.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szEXT, MAX_PATH);

			osTxt << i << "-" << j << ". " << szFileName << endl;
		}
	}
	osTxt << endl;

	osTxt << "Animations: " << endl;
	for (_uint i = 0; i < m_iNumAnimations; ++i)
	{
		osTxt << i << ". " << m_Animations[i]->Get_Name() << endl;
	}
	osTxt << endl;

	osTxt << "Bones: " << endl;
	for (CBone* pBone : m_Bones)
	{
		osTxt << pBone->Get_Name() << endl;
	}
	osTxt << endl;

	osTxt.close();
}

void CModel::Set_Speed(_int iSpeed)
{
	//return m_Animations
}

vector<CAnimation*>* CModel::Get_Animations()
{
	return &m_Animations;
}

CMyAIScene* CModel::Get_AIScene()
{
	return &m_pAIScene;
}

vector<CBone*>* CModel::Get_Bones()
{
	return &m_Bones;
}

_uint CModel::Get_BoneNum(const _char* _szName)
{
	return _uint();
}

_uint CModel::Get_CurrentKeyFrames(_uint iIndex)
{
	return m_Animations[m_iCurrentAnimIndex]->Get_CurrentKeyFrames(iIndex);
}

HRESULT CModel::Ready_Meshes(_fmatrix PivotMatrix)
{
	m_iNumMeshes = m_pAIScene.Get_NumMeshes();

	m_Meshes.reserve(m_iNumMeshes);

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, m_pAIScene.Get_Mesh((_uint)i), PivotMatrix, m_Bones);

		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials(const string& strModelFilePath)
{
	m_iNumMaterials = m_pAIScene.Get_NumMaterials();

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		//if() 내일 와서 이거 작업 할것 
		CMyAIMaterial pAIMaterial = m_pAIScene.Get_Material((_uint)i);

		MATERIAL_DESC			MaterialDesc = {  };

		for (size_t j = 1; j < (size_t)AI_TEXTURE_TYPE_MAX; j++)
		{
			_char		szDrive[MAX_PATH] = "";
			_char		szDirectory[MAX_PATH] = "";

			_splitpath_s(strModelFilePath.c_str(), szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);

			string strPath = pAIMaterial.Get_Textures((_uint)j);
			if (strPath == "")
				continue;

			_char		szFileName[MAX_PATH] = "";
			_char		szEXT[MAX_PATH] = "";

			//_splitpath_s(strPath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szEXT, MAX_PATH);
			_splitpath_s(strPath.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szEXT, MAX_PATH);

			if (szFileName == "ICarusGround2")
				_int iCheck = 0;

			_char		szTmp[MAX_PATH] = "";
			strcpy_s(szTmp, szDrive);
			strcat_s(szTmp, szDirectory);
			strcat_s(szTmp, szFileName);
			strcat_s(szTmp, szEXT);

			//_char szTest[MAX_PATH] = ".dds";
			//strcat_s(szTmp, szTest);

			_tchar		szFullPath[MAX_PATH] = TEXT("");

			MultiByteToWideChar((_uint)CP_ACP, 0, szTmp, (_int)strlen(szTmp), szFullPath, (_int)MAX_PATH);

			if (szFileName == "M_Invisible") /* 현재 게임에서 이 텍스쳐를 가지면 투명한 텍스쳐라 가상으로 만들어줘도 터짐 */
				continue;

			MaterialDesc.pMtrlTextures[j] = CTexture::Create(m_pDevice, m_pContext, szFullPath, 1);

			if (nullptr == MaterialDesc.pMtrlTextures[j])	
				return E_FAIL;
			
			string TestfileName(szFileName);

			if (TestfileName == "T_GiantTreeBark_01_BC")
				int a = 0;

			// Diffuse 일때 한번 검사 + Normal일때 Diffuse에서 못만들었다면 추가 검사 
			if ((j == (size_t)aiTextureType_DIFFUSE) || (j == (size_t)aiTextureType_NORMALS && false == m_bSpecularExist)) // Diffuse 있을때 ORM넣기 
			{
				MaterialDesc.pMtrlTextures[(size_t)aiTextureType_SPECULAR] = Add_NotIncludedTexture(ADD_TEXTURE_TYPE::TYPE_ORM, szFileName, szDrive, szDirectory, szEXT);

				if (nullptr == MaterialDesc.pMtrlTextures[(size_t)aiTextureType_SPECULAR]) /* 1글자 뺴서 하는 ORM 실패 */
				{
					m_bSpecularExist = false;
					MaterialDesc.pMtrlTextures[(size_t)aiTextureType_SPECULAR] = Add_NotIncludedTexture(ADD_TEXTURE_TYPE::TYPE_ORM, szFileName, szDrive, szDirectory, szEXT, 2);

					if (nullptr != MaterialDesc.pMtrlTextures[(size_t)aiTextureType_SPECULAR])/* 2글자 뺴서 하는 ORM 성공  */
						m_bSpecularExist = true;
					else
					{
						/* 뭘해도 Specular가 없다 : 다른텍스쳐로 대체 */
						MaterialDesc.pMtrlTextures[(size_t)aiTextureType_METALNESS] = Add_NotIncludedTexture(ADD_TEXTURE_TYPE::TYPE_METALIC, szFileName, szDrive, szDirectory, szEXT);
						if (nullptr == MaterialDesc.pMtrlTextures[(size_t)aiTextureType_METALNESS])
							MaterialDesc.pMtrlTextures[(size_t)aiTextureType_METALNESS] = Add_NotIncludedTexture(ADD_TEXTURE_TYPE::TYPE_METALIC, szFileName, szDrive, szDirectory, szEXT, 2);
						else
							cout << "Metalic : " << szFileName << endl;
						
						MaterialDesc.pMtrlTextures[(size_t)aiTextureType_OPACITY] = Add_NotIncludedTexture(ADD_TEXTURE_TYPE::TYPE_OPACITY, szFileName, szDrive, szDirectory, szEXT);
						if (nullptr == MaterialDesc.pMtrlTextures[(size_t)aiTextureType_OPACITY])
							MaterialDesc.pMtrlTextures[(size_t)aiTextureType_OPACITY] = Add_NotIncludedTexture(ADD_TEXTURE_TYPE::TYPE_OPACITY, szFileName, szDrive, szDirectory, szEXT, 2);
						else
							cout << "Opacity : " << szFileName << endl;
						
						MaterialDesc.pMtrlTextures[(size_t)aiTextureType_DIFFUSE_ROUGHNESS] = Add_NotIncludedTexture(ADD_TEXTURE_TYPE::TYPE_ROUGHNESS, szFileName, szDrive, szDirectory, szEXT);
						if (nullptr == MaterialDesc.pMtrlTextures[(size_t)aiTextureType_DIFFUSE_ROUGHNESS])
							MaterialDesc.pMtrlTextures[(size_t)aiTextureType_DIFFUSE_ROUGHNESS] = Add_NotIncludedTexture(ADD_TEXTURE_TYPE::TYPE_ROUGHNESS, szFileName, szDrive, szDirectory, szEXT, 2);
						else
							cout << " Roughness: " << szFileName << endl;

						//cout << "Model : " << strModelFilePath << endl;
						//cout << "Texture : " << szFileName << endl;
						//cout << endl;
					}

					MaterialDesc.pMtrlTextures[(size_t)aiTextureType_EMISSIVE] = Add_NotIncludedTexture(ADD_TEXTURE_TYPE::TYPE_EMISSIVE, szFileName, szDrive, szDirectory, szEXT);
					if (nullptr == MaterialDesc.pMtrlTextures[(size_t)aiTextureType_EMISSIVE])
						MaterialDesc.pMtrlTextures[(size_t)aiTextureType_EMISSIVE] = Add_NotIncludedTexture(ADD_TEXTURE_TYPE::TYPE_EMISSIVE, szFileName, szDrive, szDirectory, szEXT, 2);
					else
						cout << " Emissive: " << szFileName << endl;
				}
				else
					m_bSpecularExist = true; /* 1글자 뺴서 하는 ORM 성공  */
			}

			if (j == (size_t)aiTextureType_DIFFUSE)
			{
				
			}
		}

		m_Materials.push_back(MaterialDesc);
	}

	return S_OK;
}

HRESULT CModel::Ready_Bones(CMyAINode pAINode, _int iParentIndex)
{
	CBone* pBone = CBone::Create(pAINode, iParentIndex);
	if (nullptr == pBone)
		return E_FAIL;

	m_Bones.push_back(pBone);

	_int		iParentIdx = (_int)m_Bones.size() - 1;

	for (size_t i = 0; i < pAINode.Get_NumChildren(); i++)
	{
		Ready_Bones(CMyAINode(pAINode.Get_Children((_uint)i)), iParentIdx);
	}

	return S_OK;
}

HRESULT CModel::Ready_Animations()
{
	m_iNumAnimations = m_pAIScene.Get_NumAnimations();

	for (size_t i = 0; i < m_iNumAnimations; i++)
	{
		CAnimation* pAnimation = CAnimation::Create(m_pAIScene.Get_Animation((_uint)i), m_Bones);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation);
	}

	return S_OK;
}

_bool CModel::IsModelPicking(RAY _Ray, _matrix _WorldMatrix, _float4* pOut)
{
	if (m_Meshes.empty()) // 메쉬가 비었는지 체크
		return false;

	for (auto& iter : m_Meshes)
	{
		if (iter->Compute_Ray(_Ray, _WorldMatrix, pOut))
			return true;
	}

	return false;
}


HRESULT CModel::SetUp_OnShader(CShader* pShader, _uint iMaterialIndex, aiTextureType eTextureType, const char* strConstantName)
{
	if (iMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	NULL_CHECK_RETURN(m_Materials[iMaterialIndex].pMtrlTextures[eTextureType], E_FAIL);

	return m_Materials[iMaterialIndex].pMtrlTextures[eTextureType]->Set_SRV(pShader, strConstantName);
}

HRESULT CModel::Render(CShader*& pShader, const _uint& iMeshIndex, const _uint& strPassName)
{
	/* m_pShaderCom->Begin(ECast(ANIM_SHADER::ANIM_OUTLINE));

		m_pModelCom->Render(0);
	*/

	FAILED_CHECK(pShader->Begin(strPassName));

	FAILED_CHECK(m_Meshes[iMeshIndex]->Bind_VIBuffers());

	FAILED_CHECK(m_Meshes[iMeshIndex]->Render());

	return S_OK;
}

CTexture* CModel::Add_NotIncludedTexture(ADD_TEXTURE_TYPE eType, const char* strOriginFileName, const char* strOriginDrive, const char* strOriginDirectory, const char* strOriginExt, _int iCnt)
{
	/* Diffuse 기준, ORM텍스쳐는 아니지만 Roughness, Opacity, Metalic 으로도 읽지않아서 만들어줘야하는 텍스쳐 */
	string PBRfileName(strOriginFileName);

	for (_int i = 0; i < iCnt; ++i)
	{
		PBRfileName.pop_back();
	}

	switch (eType)
	{
	case Engine::CModel::ADD_TEXTURE_TYPE::TYPE_METALIC:
		PBRfileName += "Metalic";
		break;
	case Engine::CModel::ADD_TEXTURE_TYPE::TYPE_ROUGHNESS:
		PBRfileName += "Roughness";
		break;
	case Engine::CModel::ADD_TEXTURE_TYPE::TYPE_OPACITY:
		PBRfileName += "Opacity";
		break;
	case Engine::CModel::ADD_TEXTURE_TYPE::TYPE_ORM:
		PBRfileName += "ORM";
		break;
	case Engine::CModel::ADD_TEXTURE_TYPE::TYPE_EMISSIVE:
		PBRfileName += "Emissive";
		break;

	}

	_char		szPBRTmp[MAX_PATH] = "";
	strcpy_s(szPBRTmp, strOriginDrive);
	strcat_s(szPBRTmp, strOriginDirectory);
	strcat_s(szPBRTmp, PBRfileName.c_str());
	strcat_s(szPBRTmp, strOriginExt);

	_tchar		szPBRFullPath[MAX_PATH] = TEXT("");
	MultiByteToWideChar((_uint)CP_ACP, 0, szPBRTmp, (_int)strlen(szPBRTmp), szPBRFullPath, (_int)MAX_PATH);
	CTexture* pTexture = CTexture::Create(m_pDevice, m_pContext, szPBRFullPath, 1, true);

	if (eType == CModel::ADD_TEXTURE_TYPE::TYPE_ORM && nullptr == pTexture)
	{
		string ORMfileName_2(strOriginFileName);

		for (_int i = 0; i < iCnt; ++i)
		{
			ORMfileName_2.pop_back();
		}

		ORMfileName_2 += "M";

		_char		szORMTmp[MAX_PATH] = "";
		strcpy_s(szORMTmp, strOriginDrive);
		strcat_s(szORMTmp, strOriginDirectory);
		strcat_s(szORMTmp, ORMfileName_2.c_str());
		strcat_s(szORMTmp, strOriginExt);

		_tchar		szORMFullPath[MAX_PATH] = TEXT("");

		MultiByteToWideChar((_uint)CP_ACP, 0, szORMTmp, (_int)strlen(szORMTmp), szORMFullPath, (_int)MAX_PATH);

		CTexture* pTexture_New = CTexture::Create(m_pDevice, m_pContext, szORMFullPath, 1, true);

		return pTexture_New;
	}

	return pTexture;
}

HRESULT CModel::Render(_uint iMeshIndex)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	m_Meshes[iMeshIndex]->Bind_VIBuffers();
	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

CModel * CModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eType, const string & strModelFilePath, _fmatrix PivotMatrix)
{
	CModel*		pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, strModelFilePath, PivotMatrix)))
	{
		MSG_BOX("Failed to Created : CModel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CModel::Clone(void * pArg)
{
	CModel*		pInstance = new CModel(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CComponent");
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CModel::Free()
{
	__super::Free();

	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);

	m_Bones.clear();

	for (auto& MaterialDesc : m_Materials)
	{
		for (auto& pTexture : MaterialDesc.pMtrlTextures)
			Safe_Release(pTexture);
	}
	m_Materials.clear();

	for (auto& pMesh : m_Meshes)
	{
		Safe_Release(pMesh);
	}
	m_Meshes.clear();

	//if (false == m_isCloned)
	m_MyAssimp.FreeScene();
}
