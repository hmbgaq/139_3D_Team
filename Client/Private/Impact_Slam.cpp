#include "Impact_Slam.h"
#include "GameInstance.h"
#include "Character.h"
#include "Data_Manager.h"
#include "Effect.h"
#include "Effect_Manager.h"
#include "Player.h"


CImpact_Slam::CImpact_Slam(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CProjectile(pDevice, pContext, strPrototypeTag)
{
}

CImpact_Slam::CImpact_Slam(const CImpact_Slam& rhs)
	: CProjectile(rhs)
{
}

HRESULT CImpact_Slam::Initialize_Prototype()
{

	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CImpact_Slam::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECT_DESC		GameObjectDesc = {};

	GameObjectDesc.fSpeedPerSec = 0.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	m_fDamage = 10.f;

	m_fLifeTime = 1.f;



	return S_OK;
}

void CImpact_Slam::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CImpact_Slam::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CImpact_Slam::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CImpact_Slam::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CImpact_Slam::OnCollisionEnter(CCollider* other)
{

	CCharacter* pTarget_Character = Get_Target_Character(other);

	if (nullptr != pTarget_Character)// 일반 타격 
	{
		m_eHitDirection = Direction::Front;
		m_eHitPower = Power::Medium;
		m_fForce = 0.5f;

		_float fDamage = m_fDamage + CData_Manager::GetInstance()->Get_Additional_MeleeDamage();

		//_vector vPlayerPos = CData_Manager::GetInstance()->Get_Player()->Get_Position_Vector();
		//_vector vDir = pTarget_Character->Calc_Look_Dir_XZ(vPlayerPos);
		_vector vDir = CData_Manager::GetInstance()->Get_Player()->Get_Transform()->Get_Look();
		pTarget_Character->Set_Hitted(fDamage, vDir, m_fForce, 1.f, m_eHitDirection, m_eHitPower);


		_float3 vPos = m_pTransformCom->Get_Position();
		_float3 vTargetPos = pTarget_Character->Get_WeaknessPos();
		EFFECT_MANAGER->Play_Effect("Hit/", "Hit_Distortion.json", nullptr, vPos, true, vTargetPos);

		// 타격 이펙트
		//EFFECT_MANAGER->Play_Effect("Hit/", "Hit_Distortion.json", m_pTransformCom->Get_Position(), true, pTarget_Character->Get_Position());

		//EFFECT_MANAGER->Play_Effect("Hit/", "Hit_Distortion.json", m_pTransformCom->Get_Position());


		//CEffect* pEffect = EFFECT_MANAGER->Create_Effect("Hit/", "Hit_Distortion.json", m_pTransformCom->Get_Position());
	}

}

void CImpact_Slam::OnCollisionStay(CCollider* other)
{
}

void CImpact_Slam::OnCollisionExit(CCollider* other)
{
}

HRESULT CImpact_Slam::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Shader_Model"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	///* For.Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC BoundingDesc = {};
	BoundingDesc.iLayer = ECast(COLLISION_LAYER::PLAYER_ATTACK);
	BoundingDesc.fRadius = { 5.0f };
	BoundingDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pCollider), &BoundingDesc)))
		return E_FAIL;


	return S_OK;
}

CImpact_Slam* CImpact_Slam::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CImpact_Slam* pInstance = new CImpact_Slam(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CImpact_Slam");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CImpact_Slam::Clone(void* pArg)
{
	CImpact_Slam* pInstance = new CImpact_Slam(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CImpact_Slam");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CImpact_Slam::Pool()
{
	return new CImpact_Slam(*this);
}

void CImpact_Slam::Free()
{
	__super::Free();

	if (nullptr != m_pEffect)
		Safe_Release(m_pEffect);
}
