#include "..\Public\Model.h"
#include "Mesh.h"
#include "Texture.h"
#include "Bone.h"
#include "Animation.h"
#include "Channel.h"

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

HRESULT CModel::Initialize_Prototype(TYPE eType, const string & strModelFilePath, _fmatrix PivotMatrix)
{
	m_eModelType = eType;

	_uint	iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

	if (TYPE_NONANIM == eType)
		iFlag |= aiProcess_PreTransformVertices;


	m_pAIScene = m_MyAssimp.ReadFile(strModelFilePath, iFlag, true);

	

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

	return S_OK;
}

HRESULT CModel::Initialize(void * pArg)
{

	return S_OK;
}

HRESULT CModel::Render(_uint iMeshIndex)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	m_Meshes[iMeshIndex]->Bind_VIBuffers();
	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

void CModel::Play_Animation(_float fTimeDelta, _bool isLoop)
{
	if (m_iCurrentAnimIndex >= m_iNumAnimations)
		return;

	/* ���� �ִϸ��̼��� ����ϰ� �ִ� ������ TransformationMatrix�� �����Ѵ�. */
	m_Animations[m_iCurrentAnimIndex]->Invalidate_TransformationMatrix(isLoop, fTimeDelta, m_Bones);


	/* ȭ�鿡 �������� ���·� �׷��������ؼ��� �ݵ�� ������ CombinedTransformationMatrix�� ���ŵǾ���Ѵ�. */
	/* ��� ������ �� �����ϸ� �θ�κ��� �ڽı��� �޿������Ƽ� CombinedTransformationMatrix�� �����Ѵ�. */
	for (auto& pBone : m_Bones)
	{
		pBone->Invalidate_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PivotMatrix));
	}
}

HRESULT CModel::Bind_BoneMatrices(CShader * pShader, const _char * pConstantName, _uint iMeshIndex)
{
	return m_Meshes[iMeshIndex]->Bind_BoneMatrices(pShader, pConstantName, m_Bones);	
}

HRESULT CModel::Bind_ShaderResource(CShader * pShader, const _char * pConstantName, _uint iMeshIndex, aiTextureType eTextureType)
{

	_uint		iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();
	if (iMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	return m_Materials[iMaterialIndex].pMtrlTextures[eTextureType]->Bind_ShaderResource(pShader, pConstantName);	
}

void CModel::Set_Animation(_uint _iAnimationIndex, CModel::ANIM_STATE _eAnimState, _bool _bIsTransition, _float _fTransitionDuration, _uint iTargetKeyFrameIndex)
{
	m_eAnimState = _eAnimState;

	//if (_iAnimationIndex != m_iCurrentAnimIndex)
	{
		Reset_Animation(_iAnimationIndex);

		if (_bIsTransition)
		{
			Set_Animation_Transition(_iAnimationIndex, _fTransitionDuration, iTargetKeyFrameIndex);
		}
		else
		{
			m_iCurrentAnimIndex = _iAnimationIndex;
		}
	}
}

void CModel::Set_Animation_Transition(_uint _iAnimationIndex, _float _fTransitionDuration, _uint iTargetKeyFrameIndex)
{
	if (_iAnimationIndex == m_iCurrentAnimIndex)
	{
		m_Animations[m_iCurrentAnimIndex]->Set_TrackPosition(iTargetKeyFrameIndex);
	}

	CAnimation* currentAnimation = m_Animations[m_iCurrentAnimIndex];
	CAnimation* targetAnimation = m_Animations[_iAnimationIndex];

	targetAnimation->Reset_Animation(m_Bones);		// �ӽ�

	targetAnimation->Set_Transition(currentAnimation, _fTransitionDuration, iTargetKeyFrameIndex);

	m_iCurrentAnimIndex = _iAnimationIndex;
}

void CModel::Reset_Animation(_int iAnimIndex)
{
	if (iAnimIndex == -1)
		m_Animations[m_iCurrentAnimIndex]->Reset_Animation(m_Bones);
	else
		m_Animations[iAnimIndex]->Reset_Animation(m_Bones);
}

_float CModel::Get_TickPerSecond()
{
	return m_Animations[m_iCurrentAnimIndex]->Get_TickPerSecond();
}

_bool CModel::Is_Transition()
{
	return m_Animations[m_iCurrentAnimIndex]->Is_Transition();
}

_bool CModel::Is_Inputable_Front(_uint _iIndexFront)
{
	return m_Animations[m_iCurrentAnimIndex]->Is_Inputable_Front(_iIndexFront);
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

HRESULT CModel::Ready_Meshes(_fmatrix PivotMatrix)
{
	m_iNumMeshes = m_pAIScene.Get_NumMeshes();

	m_Meshes.reserve(m_iNumMeshes);

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, m_pAIScene.Get_Mesh(i), PivotMatrix, m_Bones);

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
		CMyAIMaterial pAIMaterial = m_pAIScene.Get_Material(i);

		MATERIAL_DESC			MaterialDesc = {  };

		for (size_t j = 1; j < AI_TEXTURE_TYPE_MAX; j++)
		{
			_char		szDrive[MAX_PATH] = "";
			_char		szDirectory[MAX_PATH] = "";

			_splitpath_s(strModelFilePath.c_str(), szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);

			//aiString			strPath;
			//if (FAILED(pAIMaterial.GetTexture(aiTextureType(j), 0, &strPath)))
			//	continue;

			string strPath = pAIMaterial.Get_Textures(j);
			if (strPath == "")
				continue;

			_char		szFileName[MAX_PATH] = "";
			_char		szEXT[MAX_PATH] = "";

			//_splitpath_s(strPath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szEXT, MAX_PATH);
			_splitpath_s(strPath.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szEXT, MAX_PATH);

			_char		szTmp[MAX_PATH] = "";
			strcpy_s(szTmp, szDrive);
			strcat_s(szTmp, szDirectory);
			strcat_s(szTmp, szFileName);
			strcat_s(szTmp, szEXT);

			//_char szTest[MAX_PATH] = ".dds";
			//strcat_s(szTmp, szTest);

			_tchar		szFullPath[MAX_PATH] = TEXT("");

			MultiByteToWideChar(CP_ACP, 0, szTmp, strlen(szTmp), szFullPath, MAX_PATH);


			MaterialDesc.pMtrlTextures[j] = CTexture::Create(m_pDevice, m_pContext, szFullPath, 1);
			if (nullptr == MaterialDesc.pMtrlTextures[j])
				return E_FAIL;
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

	_int		iParentIdx = m_Bones.size() - 1;

	for (size_t i = 0; i < pAINode.Get_NumChildren(); i++)
	{
		Ready_Bones(CMyAINode(pAINode.Get_Children(i)), iParentIdx);
	}

	return S_OK;
}

HRESULT CModel::Ready_Animations()
{
	m_iNumAnimations = m_pAIScene.Get_NumAnimations();

	for (size_t i = 0; i < m_iNumAnimations; i++)
	{
		CAnimation* pAnimation = CAnimation::Create(m_pAIScene.Get_Animation(i), m_Bones);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation);
	}

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

	/* ������ü�� �ʱ�ȭ�Ѵ�.  */
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

	if (false == m_isCloned)
		m_MyAssimp.FreeScene();
}