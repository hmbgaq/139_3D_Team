#include "stdafx.h"
#include "InstanceMonster.h"
#include "Collider.h"
#include "GameInstance.h"
#include "Animation.h"
#include "VIBuffer_AnimModel_Instance.h"
#include "Bone.h"

CInstanceMonster::CInstanceMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CInstanceMonster::CInstanceMonster(const CInstanceMonster& rhs)
	: CGameObject(rhs)
{
}

HRESULT CInstanceMonster::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CInstanceMonster::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Instance()))
		return E_FAIL;

	m_pModelCom->Set_Animation(rand() % 20);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(_float(rand() % 20), 0.f, _float(rand() % 20), 1.f));

	m_iRenderPass = 0;
	m_fTimeDelta = 0;

	return S_OK;
}

void CInstanceMonster::Priority_Tick(_float fTimeDelta)
{
}

void CInstanceMonster::Tick(_float fTimeDelta)
{
}

void CInstanceMonster::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Transform_Frustum_ToLocalSpace(m_pTransformCom->Get_WorldMatrix());

	//if (true == m_pGameInstance->isIn_LocalPlanes(XMVector3TransformCoord(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pTransformCom->Get_WorldMatrixInverse()), 0.f))
	//{
		_float3 vRootAnimPos = {};

		m_pModelCom->Play_Animation(fTimeDelta, &vRootAnimPos);

		m_pTransformCom->Add_RootBone_Position(vRootAnimPos);

		if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
			return;

//#ifdef _DEBUG
//		m_pGameInstance->Add_DebugRender(m_pColliderCom);
//#endif	
	//}
}

HRESULT CInstanceMonster::Render()
{
	
	Render_Instance(0);
		

	return S_OK;
}

HRESULT CInstanceMonster::Render_Instance(_uint iSize)
{
	HRESULT hr;

	hr = FAILED(m_tInstanceDesc.pInstanceShader->Bind_SRV("g_InstanceTransform", m_tInstanceDesc.pInstanceSRV));
	hr = FAILED(m_pDissolveTexCom->Bind_ShaderResource(m_tInstanceDesc.pInstanceShader, "g_DissolveTexture"));
	
	hr = FAILED(m_pTransformCom->Bind_ShaderResource(m_tInstanceDesc.pInstanceShader, "g_WorldMatrix"));
	hr = FAILED(m_tInstanceDesc.pInstanceShader->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)));
	hr = FAILED(m_tInstanceDesc.pInstanceShader->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)));


	_int iMeshSize = m_pModelCom->Get_NumMeshes();
	
	for (_int i = 0; i < iMeshSize; ++i)
	{
		hr = m_pModelCom->Bind_BoneMatrices(m_tInstanceDesc.pInstanceShader, "g_BoneMatrices", (_uint)i);

		hr = m_pModelCom->Bind_ShaderResource(m_tInstanceDesc.pInstanceShader, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE);
		//m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture", (_uint)i, aiTextureType_NORMALS); /* 노말 텍스쳐 추가 */

		if (3 == m_iRenderPass)
		{
			hr = m_tInstanceDesc.pInstanceShader->Bind_RawValue("g_fDissolveWeight", &m_fTimeDelta, sizeof(_float));
			hr = m_pDissolveTexCom->Bind_ShaderResource(m_tInstanceDesc.pInstanceShader, "g_DissolveTexture");
		}

		hr = m_tInstanceDesc.pInstanceShader->Begin(m_iRenderPass);
		hr = FAILED(m_pInstanceModelCom->Render(m_pModelCom->Get_Mesh_For_Index(i), i));

		
//		m_pModelCom->Render(_uint(i));
	}

	if(hr == E_FAIL)
		_int i = 0;
//	ID3D11ShaderResourceView* pInstanceSRV = { nullptr };
	
	return S_OK;
}

HRESULT CInstanceMonster::Ready_Components()
{
	_int iCurrentLevel = m_pGameInstance->Get_NextLevel();

	/* For.Com_Shader */
	{
		if (FAILED(__super::Add_Component(iCurrentLevel, TEXT("Prototype_Component_Shader_AnimModel"),
			TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
			return E_FAIL;
	}


	{
		if (FAILED(__super::Add_Component(iCurrentLevel, TEXT("Prototype_Component_Model_BeastBoss_Phase3"),
			TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			return E_FAIL;
	}

	/* For. Texture */
	{
		if (FAILED(__super::Add_Component(iCurrentLevel, TEXT("Prototype_Component_Texture_Dissolve"),
			TEXT("Com_DissolveTex"), reinterpret_cast<CComponent**>(&m_pDissolveTexCom))))
			return E_FAIL;
	}

	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC		BoundingDesc = {};
	{
		BoundingDesc.vExtents = _float3(0.5f, 0.7f, 0.5f);
		BoundingDesc.vCenter = _float3(0.f, BoundingDesc.vExtents.y, 0.f);

		if (FAILED(__super::Add_Component(iCurrentLevel, TEXT("Prototype_Component_Collider_AABB"),
			TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CInstanceMonster::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

HRESULT CInstanceMonster::Ready_Instance()
{
	m_tInstanceDesc.iMaxInstanceCount = 100;

	_int iCurrentLevel = m_pGameInstance->Get_NextLevel();

	if (FAILED(__super::Add_Component(iCurrentLevel, TEXT("Prototype_Component_Shader_AnimModel_Instance"),
		TEXT("Com_InstanceShader"), reinterpret_cast<CComponent**>(&m_tInstanceDesc.pInstanceShader))))
		return E_FAIL;

	//! 버퍼

	//Instance
	{
		size_t iSizePerInstacne = sizeof(_uint) + sizeof(_float4x4);

		
		BYTE* pData = new BYTE[iSizePerInstacne * m_tInstanceDesc.iMaxInstanceCount];
		ZeroMemory(pData, iSizePerInstacne * m_tInstanceDesc.iMaxInstanceCount);

		m_tInstanceDesc.pByte = pData;
		

		CVIBuffer_AnimModel_Instance::ANIMINSTANCE_BUFFER_DESC InstanceBufferDesc = {};

		InstanceBufferDesc.iMaxInstance = m_tInstanceDesc.iMaxInstanceCount;
		InstanceBufferDesc.iSizePerInstance = iSizePerInstacne;
		InstanceBufferDesc.pValue = m_tInstanceDesc.pByte;

		if (FAILED(__super::Add_Component(iCurrentLevel, TEXT("Prototype_Component_VIBuffer_AnimModel_Instance"),
			TEXT("Com_InstanceBuffer"), reinterpret_cast<CComponent**>(&m_pInstanceModelCom), &InstanceBufferDesc)))
			return E_FAIL;
	}









	//! 텍스처
		_uint iBoneSize = m_pModelCom->Get_Bones()->size();


		_float4x4* pMatrix = new _float4x4[m_tInstanceDesc.iMaxInstanceCount * iBoneSize];
		ZeroMemory(pMatrix, m_tInstanceDesc.iMaxInstanceCount * iBoneSize);
		
		m_tInstanceDesc.pMatrix = pMatrix;

	{
		D3D11_TEXTURE2D_DESC	TextureDesc;
		ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

		TextureDesc.Width = iBoneSize * 4;
		TextureDesc.Height = m_tInstanceDesc.iMaxInstanceCount;
		TextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		TextureDesc.Usage = D3D11_USAGE_DYNAMIC;
		TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		TextureDesc.MipLevels = 1;
		TextureDesc.SampleDesc.Count = 1;
		TextureDesc.ArraySize = 1;
		TextureDesc.SampleDesc.Quality = 0;

		D3D11_SUBRESOURCE_DATA tSubResources;

		tSubResources.pSysMem = m_tInstanceDesc.pMatrix;
		tSubResources.SysMemPitch = iBoneSize * sizeof(_float4x4);

		
		FAILED_CHECK(m_pDevice->CreateTexture2D(&TextureDesc, &tSubResources, &m_tInstanceDesc.pInstanceTexture));
	}

	{
		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
		ZeroMemory(&SRVDesc, sizeof(SRVDesc));
		SRVDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = 1;

		if (FAILED(m_pDevice->CreateShaderResourceView(m_tInstanceDesc.pInstanceTexture, &SRVDesc, &m_tInstanceDesc.pInstanceSRV)))
			return E_FAIL;
	}


	return S_OK;
}

void CInstanceMonster::Add_InstanceData(_uint iSize, _uint& iIndex)
{
	{
		_float4x4* pAnimInstanceValue = m_tInstanceDesc.pMatrix;

		_uint iBoneSize = m_pModelCom->Get_Bones()->size();

		size_t iSizePerInstance = iBoneSize * sizeof(_float4x4);
		_uint iDataIndex = iIndex * iBoneSize;

		vector<CBone*> Bones = *m_pModelCom->Get_Bones();
		

		vector<_float4x4> BoneMatrices;
		BoneMatrices.reserve(Bones.size());

		for (_int i = 0; i < Bones.size(); ++i)
		{
			BoneMatrices.push_back(Bones[i]->Get_CombinedTransformationMatrix());
		}

		memcpy(pAnimInstanceValue + iDataIndex, BoneMatrices.data(), iSizePerInstance);
	}

}

CInstanceMonster* CInstanceMonster::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CInstanceMonster* pInstance = new CInstanceMonster(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CInstanceMonster");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CInstanceMonster::Clone(void* pArg)
{
	CInstanceMonster* pInstance = new CInstanceMonster(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CInstanceMonster");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CInstanceMonster::Free()
{
	__super::Free();

	Safe_Release(m_pDissolveTexCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
