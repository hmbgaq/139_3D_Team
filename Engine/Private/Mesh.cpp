#include "..\Public\Mesh.h"
#include "Shader.h"
#include "Bone.h"

CMesh::CMesh(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CMesh::CMesh(const CMesh & rhs)
	: CVIBuffer(rhs)
{
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
	}

	m_SubResourceData.pSysMem = m_pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;


#pragma endregion

	//Safe_Delete_Array(m_pIndices);

	return S_OK;
}

HRESULT CMesh::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CMesh::Bind_BoneMatrices(CShader * pShader, const _char * pConstantName, const vector<CBone*>& Bones)
{
	_float4x4		BoneMatrices[256];

	for (size_t i = 0; i < m_iNumBones; i++)
	{
		XMStoreFloat4x4(&BoneMatrices[i], XMLoadFloat4x4(&m_OffsetMatrices[i]) * Bones[m_BoneIndices[i]]->Get_CombinedTransformationMatrix());
	}

	return pShader->Bind_Matrices(pConstantName, BoneMatrices, 256);
}

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

	//Safe_Delete_Array(m_pVertices);

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

	//Safe_Delete_Array(m_pAnimVertices);

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
	if (false == m_isCloned)
	{
		Safe_Delete_Array(m_pIndices);
		Safe_Delete_Array(m_pVertices);
		Safe_Delete_Array(m_pAnimVertices);
	}

	__super::Free();
}
