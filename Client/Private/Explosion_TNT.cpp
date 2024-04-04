#include "Explosion_TNT.h"
#include "GameInstance.h"
#include "Character.h"
#include "Data_Manager.h"
#include "Effect.h"
#include "Effect_Manager.h"
#include "Player.h"

#include "Effect_Manager.h"
#include "Effect_Trail.h"


CExplosion_TNT::CExplosion_TNT(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CProjectile(pDevice, pContext, strPrototypeTag)
{
}

CExplosion_TNT::CExplosion_TNT(const CExplosion_TNT& rhs)
	: CProjectile(rhs)
{
}

HRESULT CExplosion_TNT::Initialize_Prototype()
{

	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CExplosion_TNT::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECT_DESC		GameObjectDesc = {};

	GameObjectDesc.fSpeedPerSec = 0.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	//m_pTransformCom->Look_At(m_vPlayerPos);

	m_fDamage = 50.f;

	m_fLifeTime = 2.f;

	// 이펙트 생성
	//m_pEffect = EFFECT_MANAGER->Create_Effect(m_iCurrnetLevel, LAYER_EFFECT, "Test_Skull.json", this);

	return S_OK;
}

void CExplosion_TNT::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CExplosion_TNT::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	//m_pTransformCom->Go_Straight(fTimeDelta);

}

void CExplosion_TNT::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

}

HRESULT CExplosion_TNT::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CExplosion_TNT::Render_Shadow()
{
	
	return S_OK;
}

void CExplosion_TNT::OnCollisionEnter(CCollider* other)
{


	CCharacter* pTarget_Character = Get_Target_Character(other);
	{

		m_eHitDirection = Direction::Front;
		m_eHitPower = Power::Heavy;
		m_fForce = 0.f;
		if (nullptr != pTarget_Character)
		{
			_vector vDir = pTarget_Character->Calc_Look_Dir_XZ(Get_Position_Vector());
			pTarget_Character->Set_Hitted(m_fDamage, vDir, m_fForce, 1.f, m_eHitDirection, m_eHitPower);

		}

		CEffect* pEffect = EFFECT_MANAGER->Create_Effect("Hit/", "Hit_Distortion.json", m_pTransformCom->Get_Position(), TRUE, m_pGameInstance->Get_Player()->Get_Position());

	}

}

void CExplosion_TNT::OnCollisionStay(CCollider* other)
{
}

void CExplosion_TNT::OnCollisionExit(CCollider* other)
{
}

HRESULT CExplosion_TNT::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Shader_Model"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	///* For.Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC BoundingDesc = {};
	BoundingDesc.iLayer = ECast(COLLISION_LAYER::EXPLOSION_ATTACK);
	BoundingDesc.fRadius = { 5.0f };
	BoundingDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pCollider), &BoundingDesc)))
		return E_FAIL;


	//! 유정: 트레일 추가 테스트
	//m_pTrail = EFFECT_MANAGER->Ready_Trail("Monster_Bullet_Trail.json", this);


	return S_OK;
}

CExplosion_TNT* CExplosion_TNT::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CExplosion_TNT* pInstance = new CExplosion_TNT(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CExplosion_TNT");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CExplosion_TNT::Clone(void* pArg)
{
	CExplosion_TNT* pInstance = new CExplosion_TNT(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CExplosion_TNT");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CExplosion_TNT::Pool()
{
	return new CExplosion_TNT(*this);
}

void CExplosion_TNT::Free()
{
	__super::Free();

	if (nullptr != m_pEffect)
		m_pEffect->Set_Dead(true);
}
