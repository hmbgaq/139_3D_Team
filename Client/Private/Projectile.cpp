#include "..\Public\Projectile.h"
#include "Collider.h"
#include "Transform.h"
#include "Character.h"
#include "GameInstance.h"

CProjectile::CProjectile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CGameObject(pDevice, pContext, strPrototypeTag)
{
}

CProjectile::CProjectile(const CProjectile& rhs)
	: CGameObject(rhs)
{
}

HRESULT CProjectile::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CProjectile::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	return S_OK;
}

void CProjectile::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CProjectile::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_pCollider->Update(m_pTransformCom->Get_WorldMatrix());
}

void CProjectile::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (true == m_pGameInstance->isIn_WorldPlanes(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f))
	{
		if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
			return;

		if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW, this)))
			return;
	}

#ifdef _DEBUG
	m_pGameInstance->Add_DebugRender(m_pCollider);
#endif

}

HRESULT CProjectile::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (nullptr == m_pModelCom)
		return S_OK;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE);

		m_pShaderCom->Begin(0);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

HRESULT CProjectile::Render_Shadow()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	_float4x4		ViewMatrix, ProjMatrix;

	_float4			vLightPos = Engine::g_vLightPos;

	XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(XMVectorSet(Engine::g_vLightPos.x, Engine::g_vLightPos.y, Engine::g_vLightPos.z, Engine::g_vLightPos.w), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), g_iWinsizeX / g_iWinsizeY, 0.1f, Engine::g_fLightFar));

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE);

		m_pShaderCom->Begin((_uint)1);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

CCharacter* CProjectile::Get_Target_Character(CCollider* other)
{
	if (nullptr == other || nullptr == other->Get_Owner() || nullptr == other->Get_Owner()->Get_Object_Owner())
		return nullptr;

	CCharacter* pTarget_Character = dynamic_cast<CCharacter*>(other->Get_Owner()->Get_Object_Owner());
	if (nullptr == pTarget_Character)
		return nullptr;

	return pTarget_Character;
}

CProjectile* CProjectile::Set_Damage(_int _iDamage)
{
	m_iDamage = _iDamage;

	return this;
}

CProjectile* CProjectile::Set_Direction(Direction _eHitDirection)
{
	m_eHitDirection = _eHitDirection;

	return this;
}

CProjectile* CProjectile::Set_Power(Power _eHitPower)
{
	m_eHitPower = _eHitPower;

	return this;
}


CProjectile* CProjectile::Set_Force(_float _fForce)
{
	m_fForce = _fForce;

	return this;
}

HRESULT CProjectile::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

void CProjectile::Free()
{
	__super::Free();

	Safe_Release(m_pCollider);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
