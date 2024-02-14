#include "stdafx.h"
#include "..\Public\Weapon_Player.h"

#include "GameInstance.h"
#include "Bone.h"


CWeapon_Player::CWeapon_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CWeapon(pDevice, pContext)
{

}

CWeapon_Player::CWeapon_Player(const CWeapon_Player & rhs)
	: CWeapon(rhs)
{
}

HRESULT CWeapon_Player::Initialize_Prototype()
{	

	return S_OK;
}

HRESULT CWeapon_Player::Initialize(void* pArg)
{	

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;	

	m_pTransformCom->Set_Scaling(0.1f, 0.1f, 0.1f);
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.0f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.7f, 0.f, 0.f, 1.f));


	return S_OK;
}

void CWeapon_Player::Priority_Tick(_float fTimeDelta)
{

}

void CWeapon_Player::Tick(_float fTimeDelta)
{
	_matrix		SocketMatrix = m_pSocketBone->Get_CombinedTransformationMatrix();

	for (size_t i = 0; i < 3; i++)
	{
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
	}

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix  * m_pParentTransform->Get_WorldMatrix());

	//m_pColliderCom->Update(XMLoadFloat4x4(&m_WorldMatrix));

}

void CWeapon_Player::Late_Tick(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return ;


#ifdef _DEBUG
	//m_pGameInstance->Add_DebugRender(m_pColliderCom);
#endif	
}

HRESULT CWeapon_Player::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE);

		m_pShaderCom->Begin(0);

		m_pModelCom->Render((_uint)i);
	}


	
	

	return S_OK;
}



HRESULT CWeapon_Player::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Shader_Model"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_ForkLift"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;


	/* For.Com_Collider */
// 	CBounding_OBB::BOUNDING_OBB_DESC			BoundingDesc = {};
// 
// 	BoundingDesc.vExtents = _float3(1.f, 1.f, 3.f);
// 	BoundingDesc.vCenter = _float3(0.f, BoundingDesc.vExtents.y, 0.f);
// 	BoundingDesc.vRotation = _float3(0.f, 0.f, 0.f);
// 
// 
// 	if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Collider_OBB"),
// 		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc)))
// 		return E_FAIL;

	return S_OK;
}

HRESULT CWeapon_Player::Bind_ShaderResources()
{
	/*if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;*/

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	
	
	return S_OK;
}

CWeapon_Player * CWeapon_Player::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CWeapon_Player*		pInstance = new CWeapon_Player(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CWeapon_Player");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CWeapon_Player::Clone(void* pArg)
{
	CWeapon_Player*		pInstance = new CWeapon_Player(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CWeapon_Player");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWeapon_Player::Free()
{
	__super::Free();

	Safe_Release(m_pParentTransform);
	//Safe_Release(m_pColliderCom);
	Safe_Release(m_pSocketBone);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);		
}

