#include "Projectile_GroundWave.h"
#include "GameInstance.h"
#include "Character.h"
#include "Data_Manager.h"
#include "Effect.h"
#include "Effect_Manager.h"
#include "Player.h"

#include "Effect_Manager.h"
#include "Effect_Trail.h"


CProjectile_GroundWave::CProjectile_GroundWave(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CProjectile(pDevice, pContext, strPrototypeTag)
{
}

CProjectile_GroundWave::CProjectile_GroundWave(const CProjectile_GroundWave& rhs)
	: CProjectile(rhs)
{
}

HRESULT CProjectile_GroundWave::Initialize_Prototype()
{

	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CProjectile_GroundWave::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECT_DESC		GameObjectDesc = {};

	GameObjectDesc.fSpeedPerSec = 10.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	//m_pTransformCom->Look_At(m_vPlayerPos);

	m_fDamage = 10.f;

	// 이펙트 생성
	m_pEffect = EFFECT_MANAGER->Play_Effect("Parasiter/", "Tank_Projectile2.json",this);


	return S_OK;
}

void CProjectile_GroundWave::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CProjectile_GroundWave::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_pTransformCom->Go_Straight(fTimeDelta);
}

void CProjectile_GroundWave::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);


}

HRESULT CProjectile_GroundWave::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CProjectile_GroundWave::Render_Shadow()
{
	return S_OK;
}

void CProjectile_GroundWave::OnCollisionEnter(CCollider* other)
{

	if (other->m_iLayer == (_uint)COLLISION_LAYER::WEAKNESS)
	{
		other->Get_Owner()->Set_Enable(false);
	} 

	CCharacter* pTarget_Character = Get_Target_Character(other);

	//if (nullptr != pTarget_Character)// 일반 타격 
	{
		m_eHitDirection = Direction::Front;
		m_eHitPower = Power::Medium;
		m_fForce = 0.f;
		if (nullptr != pTarget_Character)
		{
			_vector vDir = pTarget_Character->Calc_Look_Dir_XZ(Get_Position_Vector());
			//_vector vDir = pTarget_Character->Calc_Look_Dir(m_pTransformCom->Get_Position());
			pTarget_Character->Set_Hitted(m_fDamage, vDir, m_fForce, 1.f, m_eHitDirection, m_eHitPower);
		}


		EFFECT_MANAGER->Play_Effect("Hit/", "Hit_Normal.json", nullptr, m_pTransformCom->Get_Position());
	}

	//EFFECT_MANAGER->Return_ToPool(m_pEffect);
	Set_Dead(true);

}

void CProjectile_GroundWave::OnCollisionStay(CCollider* other)
{
}

void CProjectile_GroundWave::OnCollisionExit(CCollider* other)
{
}

HRESULT CProjectile_GroundWave::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Shader_Model"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	///* For.Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC BoundingDesc = {};
	BoundingDesc.iLayer = ECast(COLLISION_LAYER::MONSTER_ATTACK);
	BoundingDesc.fRadius = { 1.4f };
	BoundingDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pCollider), &BoundingDesc)))
		return E_FAIL;


	//! 유정: 트레일 추가 테스트
	//m_pTrail = EFFECT_MANAGER->Ready_Trail("Monster_Bullet_Trail.json", this);


	return S_OK;
}

CProjectile_GroundWave* CProjectile_GroundWave::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CProjectile_GroundWave* pInstance = new CProjectile_GroundWave(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CProjectile_GroundWave");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CProjectile_GroundWave::Clone(void* pArg)
{
	CProjectile_GroundWave* pInstance = new CProjectile_GroundWave(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CProjectile_GroundWave");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CProjectile_GroundWave::Pool()
{
	return new CProjectile_GroundWave(*this);
}

void CProjectile_GroundWave::Free()
{
	__super::Free();

	if(nullptr != m_pEffect)
		Safe_Release(m_pEffect);

}
