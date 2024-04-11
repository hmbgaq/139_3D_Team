#include "..\Public\Projectile.h"
#include "Collider.h"
#include "Transform.h"
#include "Character.h"
#include "GameInstance.h"
#include "AttackObject.h"

CProjectile::CProjectile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CAttackObject(pDevice, pContext, strPrototypeTag)
{
}

CProjectile::CProjectile(const CProjectile& rhs)
	: CAttackObject(rhs)
{
}

HRESULT CProjectile::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CProjectile::Initialize(void* pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	FAILED_CHECK(Ready_Components());

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

	m_fLifeTime -= fTimeDelta;
	if (0 >= m_fLifeTime) 
	{
		if (m_bIsPoolObject)
		{
			Set_Enable(false);
		}
		else 
		{
			Set_Dead(true);
		}
	}
}

void CProjectile::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (true == m_pGameInstance->isIn_WorldPlanes(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f))
	{
		FAILED_CHECK_RETURN(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this), );
		FAILED_CHECK_RETURN(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW, this), ); 
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
	//if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4())))
	//	return E_FAIL;

	//_float4x4		ViewMatrix, ProjMatrix;

	//_float4			vLightPos = Engine::g_vLightPos;

	//XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(XMVectorSet(Engine::g_vLightPos.x, Engine::g_vLightPos.y, Engine::g_vLightPos.z, Engine::g_vLightPos.w), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	//XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), g_iWinsizeX / g_iWinsizeY, 0.1f, Engine::g_fLightFar));

	//if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
	//	return E_FAIL;

	//_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	//for (size_t i = 0; i < iNumMeshes; i++)
	//{
	//	m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

	//	m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE);

	//	m_pShaderCom->Begin((_uint)1);

	//	m_pModelCom->Render((_uint)i);
	//}

	return S_OK;
}

void CProjectile::Set_Enable(_bool _Enable)
{
	m_pCollider->Set_Enable(false);

	__super::Set_Enable(_Enable);

}

//CCharacter* CProjectile::Get_Target_Character(CCollider* other)
//{
//	if (nullptr == other || nullptr == other->Get_Owner() || nullptr == other->Get_Owner()->Get_Object_Owner())
//		return nullptr;
//
//	CCharacter* pTarget_Character = dynamic_cast<CCharacter*>(other->Get_Owner()->Get_Object_Owner());
//	if (nullptr == pTarget_Character)
//		return nullptr;
//
//	return pTarget_Character;
//}
//
//CProjectile* CProjectile::Set_Damage(_float _fDamage)
//{
//	m_fDamage = _fDamage;
//
//	return this;
//}
//
//CProjectile* CProjectile::Set_Direction(Direction _eHitDirection)
//{
//	m_eHitDirection = _eHitDirection;
//
//	return this;
//}
//
//CProjectile* CProjectile::Set_Power(Power _eHitPower)
//{
//	m_eHitPower = _eHitPower;
//
//	return this;
//}
//
//
//CProjectile* CProjectile::Set_Force(_float _fForce)
//{
//	m_fForce = _fForce;
//
//	return this;
//}

void CProjectile::Search_Target(const wstring& strLayerTag, const _float fSearchDistance)
{
	//if (nullptr != m_pTarget)
	//	return;

	m_pTarget = Select_The_Nearest_Enemy(strLayerTag, fSearchDistance);
}

CCharacter* CProjectile::Select_The_Nearest_Enemy(const wstring& strLayerTag, _float fMaxDistance)
{
	CCharacter* pResult = { nullptr };

	_float fMinDistance = fMaxDistance;

	_uint iCurrentLevel = m_pGameInstance->Get_NextLevel();

	list<CGameObject*>* pTargetLayer = m_pGameInstance->Get_GameObjects(iCurrentLevel, strLayerTag);

	if (nullptr == pTargetLayer)
		return nullptr;

	for (CGameObject* pTarget : *pTargetLayer)
	{

		if (nullptr == pTarget || false == pTarget->Get_Enable())
			continue;

		CCharacter* pTargetCharacter = dynamic_cast<CCharacter*>(pTarget);

		if (nullptr == pTargetCharacter)
			continue;

		_float fDistance = Calc_Distance(pTarget);
		if (fMinDistance > fDistance)
		{
			fMinDistance = fDistance;
			pResult = pTargetCharacter;
		}
	}

	return pResult;
}

_float CProjectile::Calc_Distance(CGameObject* pTarget)
{
	if (nullptr == pTarget || false == pTarget->Get_Enable())
		return 1000000.f;

	return Calc_Distance(pTarget->Get_Position());
}

_float CProjectile::Calc_Distance(_float3 vTargetPos)
{
	_float3 vPos = Get_Position();

	_float3 vDiff = vTargetPos - vPos;

	return sqrt(vDiff.x * vDiff.x + vDiff.y * vDiff.y + vDiff.z * vDiff.z);
}

void CProjectile::Look_At_Target()
{
	if (nullptr == m_pTarget || false == m_pTarget->Get_Enable())
		return;


	_fvector vTargetPos = XMLoadFloat3(&m_pTarget->Get_WeaknessPos());
	//_fvector vTargetPos = m_pTarget->Get_Position_Vector();

	m_pTransformCom->Look_At(vTargetPos);
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
	if (m_pCollider)
	{
		m_pCollider->Set_Owner(nullptr);
		m_pCollider->Set_Enable(false);
	}

	Set_Enable(false);

	__super::Free();



	Safe_Release(m_pCollider);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
