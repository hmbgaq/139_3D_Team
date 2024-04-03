#include "Bullet_ELShotgun.h"
#include "GameInstance.h"
#include "Character.h"
#include "Data_Manager.h"
#include "Effect.h"
#include "Effect_Manager.h"
#include "Player.h"

#include "Effect_Manager.h"
#include "Effect_Trail.h"


CBullet_ELShotgun::CBullet_ELShotgun(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CProjectile(pDevice, pContext, strPrototypeTag)
{
}

CBullet_ELShotgun::CBullet_ELShotgun(const CBullet_ELShotgun& rhs)
	: CProjectile(rhs)
{
}

HRESULT CBullet_ELShotgun::Initialize_Prototype()
{

	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBullet_ELShotgun::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECT_DESC		GameObjectDesc = {};

	GameObjectDesc.fSpeedPerSec = 30.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	//m_pTransformCom->Look_At(m_vPlayerPos);

	m_fDamage = 20.f;

	m_fLifeTime = 0.5f;

	m_bIsMelee = true;

	// 이펙트 생성
	//m_pEffect = EFFECT_MANAGER->Create_Effect(m_iCurrnetLevel, LAYER_EFFECT, "Test_Skull.json", this);

	return S_OK;
}

void CBullet_ELShotgun::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CBullet_ELShotgun::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_pTransformCom->Go_Straight(fTimeDelta);
}

void CBullet_ELShotgun::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CBullet_ELShotgun::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBullet_ELShotgun::Render_Shadow()
{
	//이거 필요한거? 딱히 필요는 없어 보임 
	return S_OK;
}

void CBullet_ELShotgun::OnCollisionEnter(CCollider* other)
{

	CCharacter* pTarget_Character = Get_Target_Character(other);

	//if (nullptr != pTarget_Character)// 일반 타격 
	{
		m_eHitDirection = Direction::Front;
		m_eHitPower = Power::Medium;
		m_fForce = 0.f;
		if (nullptr != pTarget_Character)
		{
			_vector vPlayerPos = CData_Manager::GetInstance()->Get_Player()->Get_Position_Vector();
			_vector vDir = pTarget_Character->Calc_Look_Dir_XZ(vPlayerPos);
			//_vector vDir = pTarget_Character->Calc_Look_Dir(m_pTransformCom->Get_Position());
			pTarget_Character->Set_Hitted(m_fDamage, vDir, m_fForce, 1.f, m_eHitDirection, m_eHitPower, m_bIsMelee);
		}


		CEffect* pEffect = EFFECT_MANAGER->Create_Effect("Hit/", "Hit_Distortion.json", m_pTransformCom->Get_Position());

	}

	//Set_Dead(true);


	//m_pEffect->Set_Dead(true);
}

void CBullet_ELShotgun::OnCollisionStay(CCollider* other)
{
}

void CBullet_ELShotgun::OnCollisionExit(CCollider* other)
{
}

HRESULT CBullet_ELShotgun::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Shader_Model"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	///* For.Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC BoundingDesc = {};
	BoundingDesc.iLayer = ECast(COLLISION_LAYER::PLAYER_ATTACK);
	BoundingDesc.fRadius = { 1.5f };
	BoundingDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pCollider), &BoundingDesc)))
		return E_FAIL;


	//! 유정: 트레일 추가 테스트
	//m_pTrail = EFFECT_MANAGER->Ready_Trail("Monster_Bullet_Trail.json", this);


	return S_OK;
}

CBullet_ELShotgun* CBullet_ELShotgun::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CBullet_ELShotgun* pInstance = new CBullet_ELShotgun(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBullet_ELShotgun");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBullet_ELShotgun::Clone(void* pArg)
{
	CBullet_ELShotgun* pInstance = new CBullet_ELShotgun(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBullet_ELShotgun");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBullet_ELShotgun::Pool()
{
	return new CBullet_ELShotgun(*this);
}

void CBullet_ELShotgun::Free()
{
	__super::Free();

	if (nullptr != m_pEffect)
		m_pEffect->Set_Dead(true);
}
