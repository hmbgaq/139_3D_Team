#include "..\Public\Mesh.h"
#include "Shader.h"
#include "Bone.h"
#include "GameInstance.h"

CMesh::CMesh(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CMesh::CMesh(const CMesh & rhs)
	: CVIBuffer(rhs)
{
}


void CMesh::Calculate_AABB_Extents(_float3* pOutMin, _float3* pOutMax)
{
	_float3 Min, Max;

	if (m_pVertices != nullptr)
	{
		for (_uint i = 0; i < m_iNumVertices; ++i)
		{
			_float3 VertexPos = m_pVertices[i].vPosition;

			// 최소, 최대 좌표 업데이트

			for (_uint j = 0; j < 3; ++j)
			{
				Min.x = min(Min.x, VertexPos.x);
				Min.y = min(Min.y, VertexPos.y);
				Min.z = min(Min.z, VertexPos.z);

				Max.x = max(Max.x, VertexPos.x);
				Max.y = max(Max.y, VertexPos.y);
				Max.z = max(Max.z, VertexPos.z);
			}
		}
		
	}

	else if (m_pAnimVertices != nullptr)
	{
		for (_uint i = 0; i < m_iNumVertices; ++i)
		{
			_float3 VertexPos = m_pAnimVertices[i].vPosition;

			// 최소, 최대 좌표 업데이트

			for (_uint j = 0; j < 3; ++j)
			{
				Min.x = min(Min.x, VertexPos.x);
				Min.y = min(Min.y, VertexPos.y);
				Min.z = min(Min.z, VertexPos.z);

				Max.x = max(Max.x, VertexPos.x);
				Max.y = max(Max.y, VertexPos.y);
				Max.z = max(Max.z, VertexPos.z);
			}
		}
	}
	// 메쉬의 모든 정점에 대해 AABB 계산

	// 출력 변수에 결과 저장

	*pOutMin = Min;
	*pOutMax = Max;
}

HRESULT CMesh::Initialize_Prototype(CModel::TYPE eModelType, CMyAIMesh pAIMesh, _fmatrix PivotMatrix, const vector<class CBone*>& Bones)
{
	m_iMaterialIndex = pAIMesh.Get_MaterialIndex();
	strcpy_s(m_szName, pAIMesh.Get_Name().c_str());
	m_iNumVertexBuffers = 1;
	m_iNumVertices = pAIMesh.Get_NumVertices();
	m_iNumIndices = pAIMesh.Get_NumFaces() * 3;//삼각형의 정점 인덱스기 떄문에 *3 한거임
	m_iIndexStride = 4;
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER
	HRESULT		hr = CModel::TYPE_NONANIM == eModelType ? Ready_Vertices_NonAnim(pAIMesh, PivotMatrix) : Ready_Vertices_Anim(pAIMesh, Bones);

	if (FAILED(hr))
		return E_FAIL;

#pragma endregion


#pragma region INDEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT /*D3D11_USAGE_DYNAMIC*/;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	m_pIndices = new _uint[m_iNumIndices];

	_uint		iNumIndices = { 0 };

	for (size_t i = 0; i < pAIMesh.Get_NumFaces(); i++)
	{
		m_pIndices[iNumIndices++] = pAIMesh.Get_Face((_uint)i).Get_Indices()[0];
		m_pIndices[iNumIndices++] = pAIMesh.Get_Face((_uint)i).Get_Indices()[1];
		m_pIndices[iNumIndices++] = pAIMesh.Get_Face((_uint)i).Get_Indices()[2];

		//m_MeshIndex.push_back({ pAIMesh.Get_Face((_uint)i).Get_Indices()[0],
		//						pAIMesh.Get_Face((_uint)i).Get_Indices()[1],
		//						pAIMesh.Get_Face((_uint)i).Get_Indices()[2] });
	}
	
	m_SubResourceData.pSysMem = m_pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;


#pragma endregion

#ifndef _DEBUG
	Safe_Delete_Array(m_pIndices);
#endif // !_DEBUG

	

	return S_OK;
}

HRESULT CMesh::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CMesh::Bind_BoneMatrices(CShader * pShader, const _char * pConstantName, const vector<CBone*>& Bones, _float4x4* BoneMatrix)
{
	_float4x4   _BoneMatrices[550];

	for (_uint i = 0; i < m_iNumBones; i++)
	{
		XMStoreFloat4x4(&_BoneMatrices[i], XMLoadFloat4x4(&m_OffsetMatrices[i]) * Bones[m_BoneIndices[i]]->Get_CombinedTransformationMatrix());

	}

	if (nullptr != BoneMatrix)
	{
		memcpy(BoneMatrix, _BoneMatrices, sizeof(_float4x4) * 500);
		//memcpy(BoneMatrix, _BoneMatrices, sizeof(_float4x4) * 800);
		//BoneMatrix = _BoneMatrices;
	}

	return pShader->Bind_Matrices(pConstantName, _BoneMatrices, 500);

}

_bool CMesh::Compute_Ray(RAY _Ray, _matrix _WorldMatrix, _float4* pOut)
{
	_vector	vRayPos = XMLoadFloat4(&_Ray.vPosition);
	_vector vRayDir = XMLoadFloat4(&(_float4(_Ray.vDirection.x, _Ray.vDirection.y, _Ray.vDirection.z, 0.f)));

	// 마우스 광선 위치(시작점), 방향
	_float		fDist;	// 만약 충돌했다면 거리를 반환 받아야 해서 만들어뒀다.

	size_t		iMeshIndex = m_MeshIndex.size(); // 삼각형 개수 (삼각형의 개수만큼 순회해서 피킹된 녀석을 찾을거야)

	for (_uint i = 0; i < iMeshIndex; i++)
	{
		//_uint		iIndices = m_BoneIndices[i];
		_vector		vPickedPos;

		// 삼각형 인덱스의 정점 위치 3개를 받는다.
		//		m_MeshVertex(정점) 이녀석의 어떤 정점을 가져와야해? -> [[i번째 삼각형의].x번째정점]
		_vector	vVec0 = XMLoadFloat3(&m_MeshVertex[m_MeshIndex[i].ix]); // 0
		_vector	vVec1 = XMLoadFloat3(&m_MeshVertex[m_MeshIndex[i].iy]); // 1
		_vector	vVec2 = XMLoadFloat3(&m_MeshVertex[m_MeshIndex[i].iz]); // 2

		// 담아온 정점 위치 3개와 마우스를 비교하러 보내줘
		if (true == DirectX::TriangleTests::Intersects(vRayPos, vRayDir, vVec0, vVec1, vVec2, fDist))
		{
			// 비교했더니 이 메쉬의 특정 정점과 마우스의 레이저가 충돌
			vPickedPos = vRayPos + XMVector3Normalize(vRayDir) * fDist; // 충돌된 위치를 계산해서 담아

			// 충돌된 위치를 월드 좌표계로 변환
			_vector vWorldPickedPos = XMVector3TransformCoord(vPickedPos, _WorldMatrix);

			// 월드 좌표계로 변환된 위치를 pOut에 저장
			XMStoreFloat4(pOut, vWorldPickedPos);

			return true; // 값도 보내고, 충돌도 성공했다고 알려
		}
	}

	return false; // 값 없이 충돌 실패했다고 알려
}

#ifdef _DEBUG
_bool CMesh::Picking(RAY ray, _float3* out)
{
	_uint triNum = m_iNumIndices / 3;

	

	return m_pGameInstance->Picking_Vertex(ray, out, triNum, m_pVertices, m_pIndices);
}
#endif // _DEBUG



HRESULT CMesh::Ready_Vertices_NonAnim(CMyAIMesh pAIMesh, _fmatrix PivotMatrix)
{
	m_iStride = sizeof(VTXMESH);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT /*D3D11_USAGE_DYNAMIC*/;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	m_pVertices = new VTXMESH[m_iNumVertices];

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&m_pVertices[(_uint)i].vPosition, &pAIMesh.Get_Vertice((_uint)i), sizeof(_float3));
		XMStoreFloat3(&m_pVertices[(_uint)i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&m_pVertices[(_uint)i].vPosition), PivotMatrix));
		//m_MeshVertex.push_back(m_pVertices[i].vPosition); // 피킹을 GO 메쉬의 정점 위치를 저장.

		memcpy(&m_pVertices[(_uint)i].vNormal, &pAIMesh.Get_Normal((_uint)i), sizeof(_float3));
		XMStoreFloat3(&m_pVertices[(_uint)i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&m_pVertices[(_uint)i].vNormal), PivotMatrix));

		memcpy(&m_pVertices[(_uint)i].vTexcoord, &pAIMesh.Get_TextureCoord((_uint)i), sizeof(_float3));
		memcpy(&m_pVertices[(_uint)i].vTangent, &pAIMesh.Get_Tangent((_uint)i), sizeof(_float3));
		XMStoreFloat3(&m_pVertices[(_uint)i].vTangent, XMVector3TransformNormal(XMLoadFloat3(&m_pVertices[(_uint)i].vTangent), PivotMatrix));
	}

	m_SubResourceData.pSysMem = m_pVertices;

	/* pVertices에 할당하여 채워놨던 정점들의 정보를 ID3D11Buffer로 할당한 공간에 복사하여 채워넣는다. */
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

#ifndef _DEBUG
	Safe_Delete_Array(m_pVertices);
#endif // !_DEBUG
	return S_OK;
	
}

HRESULT CMesh::Ready_Vertices_Anim(CMyAIMesh pAIMesh, const vector<class CBone*>& Bones)
{
	m_iStride = sizeof(VTXANIMMESH);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT /*D3D11_USAGE_DYNAMIC*/;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	m_pAnimVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(m_pAnimVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&m_pAnimVertices[(_uint)i].vPosition, &pAIMesh.Get_Vertice((_uint)i), sizeof(_float3));
		//m_MeshVertex.push_back(m_pAnimVertices[(_uint)i].vPosition); // For. 피킹 
		memcpy(&m_pAnimVertices[(_uint)i].vNormal, &pAIMesh.Get_Normal((_uint)i), sizeof(_float3));
		memcpy(&m_pAnimVertices[(_uint)i].vTexcoord, &pAIMesh.Get_TextureCoord((_uint)i), sizeof(_float3));
		memcpy(&m_pAnimVertices[(_uint)i].vTangent, &pAIMesh.Get_Tangent((_uint)i), sizeof(_float3));
	}

	m_iNumBones = pAIMesh.Get_NumBones();

	/* 이 메시에게 영향을 주는 뼈을 순회하면서 각각의 뼈가 어떤 정점들에게 영향을 주는지 파악한다.*/
	for (size_t i = 0; i < pAIMesh.Get_NumBones(); i++)
	{
		CMyAIBone		pAIBone = pAIMesh.Get_Bone((_uint)i);

		_float4x4		OffsetMatrix;
		memcpy(&OffsetMatrix, &pAIBone.Get_OffsetMatrix(), sizeof(_float4x4));
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

		m_OffsetMatrices.push_back(OffsetMatrix);

		_uint		iBoneIndex = { 0 };

		auto	iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)
			{
				if (false == strcmp(pAIBone.Get_Name().c_str(), pBone->Get_Name()))
				{
					return true;
				}

				++iBoneIndex;

				return false;
			});

		if (iter == Bones.end())
			return E_FAIL;

		m_BoneIndices.push_back(iBoneIndex);

		/* 이 뼈는 몇개의 정점에게 영향을 주는가?! */
		for (size_t j = 0; j < pAIBone.Get_NumWeights(); j++)
		{
			/* pAIBone->mWeights[j].mVertexId : 이 뼈가 영향을 주는 j번째 정점의 인덱스 */

			if (0.0f == m_pAnimVertices[pAIBone.Get_Weights((_uint)j).Get_VertexId()].vBlendWeights.x)
			{
				m_pAnimVertices[pAIBone.Get_Weights((_uint)j).Get_VertexId()].vBlendIndices.x = (_uint)i;
				m_pAnimVertices[pAIBone.Get_Weights((_uint)j).Get_VertexId()].vBlendWeights.x = pAIBone.Get_Weights((_uint)j).Get_Weight();
			}

			else if (0.0f == m_pAnimVertices[pAIBone.Get_Weights((_uint)j).Get_VertexId()].vBlendWeights.y)
			{
				m_pAnimVertices[pAIBone.Get_Weights((_uint)j).Get_VertexId()].vBlendIndices.y = (_uint)i;
				m_pAnimVertices[pAIBone.Get_Weights((_uint)j).Get_VertexId()].vBlendWeights.y = pAIBone.Get_Weights((_uint)j).Get_Weight();
			}

			else if (0.0f == m_pAnimVertices[pAIBone.Get_Weights((_uint)j).Get_VertexId()].vBlendWeights.z)
			{
				m_pAnimVertices[pAIBone.Get_Weights((_uint)j).Get_VertexId()].vBlendIndices.z = (_uint)i;
				m_pAnimVertices[pAIBone.Get_Weights((_uint)j).Get_VertexId()].vBlendWeights.z = pAIBone.Get_Weights((_uint)j).Get_Weight();
			}

			else if (0.0f == m_pAnimVertices[pAIBone.Get_Weights((_uint)j).Get_VertexId()].vBlendWeights.w)
			{
				m_pAnimVertices[pAIBone.Get_Weights((_uint)j).Get_VertexId()].vBlendIndices.w = (_uint)i;
				m_pAnimVertices[pAIBone.Get_Weights((_uint)j).Get_VertexId()].vBlendWeights.w = pAIBone.Get_Weights((_uint)j).Get_Weight();
			}
		}
	};

	m_SubResourceData.pSysMem = m_pAnimVertices;

	/* pVertices에 할당하여 채워놨던 정점들의 정보를 ID3D11Buffer로 할당한 공간에 복사하여 채워넣는다. */

	FAILED_CHECK(__super::Create_Buffer(&m_pVB));

#ifndef _DEBUG
	Safe_Delete_Array(m_pAnimVertices);
#endif // !_DEBUG
	

	if (0 == m_iNumBones)
	{
		m_iNumBones = 1;

		_uint		iBoneIndex = { 0 };

		auto	iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)
			{
				if (false == strcmp(m_szName, pBone->Get_Name()))
				{
					return true;
				}

				++iBoneIndex;

				return false;
			});

		if (iter == Bones.end())
			return E_FAIL;

		m_BoneIndices.push_back(iBoneIndex);

		_float4x4		OffsetMatrix;
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());
		m_OffsetMatrices.push_back(OffsetMatrix);
	}

	return S_OK;
}

CMesh * CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, CMyAIMesh pAIMesh, _fmatrix PivotMatrix, const vector<class CBone*>& Bones)
{
	CMesh*		pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, pAIMesh, PivotMatrix, Bones)))
	{
		MSG_BOX("Failed to Created : CMesh");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CMesh::Clone(void * pArg)
{
	return nullptr;
}

void CMesh::Free()
{			
	#ifdef _DEBUG
	if	(false == m_isCloned)
	{
		Safe_Delete_Array(m_pIndices);
		Safe_Delete_Array(m_pVertices);
		Safe_Delete_Array(m_pAnimVertices);
	}
	#endif // _DEBUG

	__super::Free();
}
