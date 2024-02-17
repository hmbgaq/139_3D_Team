#include "stdafx.h"
#include "Body_Player.h"

#include "GameInstance.h"


CBody_Player::CBody_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBody(pDevice, pContext)
{

}

CBody_Player::CBody_Player(const CBody_Player & rhs)
	: CBody(rhs)
{
}

HRESULT CBody_Player::Initialize_Prototype()
{	
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Player::Initialize(void* pArg)
{	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;	

	m_pModelCom->Set_Animation(3, CModel::ANIM_STATE::ANIM_STATE_LOOP, true);

	return S_OK;
}

void CBody_Player::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CBody_Player::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CBody_Player::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CBody_Player::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Player::Render_Shadow()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	_float4x4		ViewMatrix, ProjMatrix;

	XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(XMVectorSet(-20.f, 20.f, -20.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), g_iWinSizeX / (float)g_iWinSizeY, 0.1f, m_pGameInstance->Get_CamFar()));

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE);

		m_pShaderCom->Begin(2);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

void CBody_Player::SetUp_Animation(_uint iAnimIndex)
{
	m_pModelCom->Set_Animation(iAnimIndex);
}

HRESULT CBody_Player::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Shader_AnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Model_Fiona"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC		BoundingDesc = {};
	BoundingDesc.iLayer = (_uint)COLLISION_LAYER::PLAYER;
	BoundingDesc.fRadius = 0.5f;
	BoundingDesc.vCenter = _float3(0.f, BoundingDesc.fRadius, 0.f);


	if (FAILED(__super::Add_Component(m_pGameInstance->Get_NextLevel(), TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc)))
		return E_FAIL;

	

	return S_OK;
}

HRESULT CBody_Player::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	_float fCamFar = m_pGameInstance->Get_CamFar();
	if(FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", &fCamFar, sizeof(_float))))

		return E_FAIL;

	return S_OK;
}

CBody_Player * CBody_Player::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBody_Player*		pInstance = new CBody_Player(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBody_Player");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBody_Player::Clone(void* pArg)
{
	CBody_Player*		pInstance = new CBody_Player(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBody_Player");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBody_Player::Free()
{
	__super::Free();

	//Safe_Release(m_pParentTransform);
	//Safe_Release(m_pColliderCom);
	//Safe_Release(m_pModelCom);	
	//Safe_Release(m_pShaderCom);
	
}

