#include "VampireCommander_Projectile_Range3.h"
#include "GameInstance.h"
#include "Character.h"
#include "Data_Manager.h"
#include "Player.h"
#include "Effect.h"
#include "Effect_Manager.h"
#include "VampireCommander.h"
#include "GameInstance.h"

CVampireCommander_Projectile_Range3::CVampireCommander_Projectile_Range3(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CProjectile(pDevice, pContext, strPrototypeTag)
{
}

CVampireCommander_Projectile_Range3::CVampireCommander_Projectile_Range3(const CVampireCommander_Projectile_Range3& rhs)
	: CProjectile(rhs)
{
}

HRESULT CVampireCommander_Projectile_Range3::Initialize_Prototype()
{

	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CVampireCommander_Projectile_Range3::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECT_DESC		GameObjectDesc = {};

	GameObjectDesc.fSpeedPerSec = 15.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	list<CGameObject*>* _pMTargets = m_pGameInstance->Get_GameObjects(LEVEL_INTRO_BOSS, TEXT("Layer_Boss"));

	if (nullptr == _pMTargets)
		return E_FAIL;

	for (CGameObject* pGameObject : *_pMTargets)
	{

		CCharacter* m_pMonster = dynamic_cast<CCharacter*>(pGameObject);
		m_vMonsterPos = m_pMonster->Get_Transform()->Get_State(CTransform::STATE_POSITION) /*+ 3 * m_pMonster->Get_Transform()->Get_State(CTransform::STATE_UP)*/;
		m_pTransformCom->Set_WorldMatrix(m_pMonster->Get_Transform()->Get_WorldMatrix());
	}

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vMonsterPos);

	m_vPlayerPos = CData_Manager::GetInstance()->Get_Player()->Get_Transform()->Get_State(CTransform::STATE_POSITION) /*+ CData_Manager::GetInstance()->Get_Player()->Get_Transform()->Get_State(CTransform::STATE_UP)*/;

	m_pTransformCom->Look_At(m_vPlayerPos);

	m_fDamage = 30.f;


	// 이펙트 생성
	//m_pEffect = EFFECT_MANAGER->Play_Effect("VampireCommander/Projectile_Range3/", "Projectile_Range3_02.json", this);

	return S_OK;
}

void CVampireCommander_Projectile_Range3::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CVampireCommander_Projectile_Range3::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	//생성되는 위치에서 그냥 앞방향으로 ㄱㄱ 
	m_pTransformCom->Go_Straight(fTimeDelta);
	m_fSoundTimeAcc += fTimeDelta;

	//! 유정 : 두두두두 이펙트 생성 테스트
	//EFFECT_MANAGER->Generate_Effect(&m_fEffectTimeAcc, 0.18f, fTimeDelta, "VampireCommander/Projectile_Range3/", "Projectile_Range3_Tick_03.json", Get_Position(), true, m_vPlayerPos);
	if (m_fSoundTimeAcc >= 0.32f && m_iPlaySoundCount < m_iMaxSoundCount)
	{
		m_pGameInstance->Play_Sound(L"EFFECT", L"dynamite_explosion_player_04.wav", SOUND_EFFECT6, 10.f);
		m_pGameInstance->Play_Sound(L"VAMPIRE_GROUNDSLAM", L"commander_lesser_attack_ground_slam_impact002.wav", SOUND_EFFECT7, 8.f);
		m_iPlaySoundCount++;
		m_fSoundTimeAcc = 0.f;
	}
	
	EFFECT_MANAGER->Generate_Effect(&m_fEffectTimeAcc, 0.32f, fTimeDelta, "VampireCommander/Projectile_Range3/", "Projectile_Pillar_Tick_10.json", Get_Position(), true, m_vPlayerPos);


	

}

void CVampireCommander_Projectile_Range3::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CVampireCommander_Projectile_Range3::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CVampireCommander_Projectile_Range3::Render_Shadow()
{
	//이거 필요한거? 딱히 필요는 없어 보임 
	return S_OK;
}

void CVampireCommander_Projectile_Range3::OnCollisionEnter(CCollider* other)
{

	CCharacter* pTarget_Character = Get_Target_Character(other);

	if (nullptr != pTarget_Character)// 일반 타격 
	{
		pTarget_Character->Set_Hitted(m_fDamage, pTarget_Character->Calc_Look_Dir_XZ(m_pTransformCom->Get_Position()), m_fForce, 1.f, m_eHitDirection, m_eHitPower);


		// 타격 이펙트
		//CEffect* pEffect = EFFECT_MANAGER->Create_Effect("Hit/", "Hit_Normal.json", m_pTransformCom->Get_Position(), TRUE, m_pGameInstance->Get_Player()->Get_Position());
		EFFECT_MANAGER->Play_Effect("Hit/", "Hit_Normal.json", nullptr, m_pGameInstance->Get_Player()->Get_Position());
	}
	m_pCollider->Set_Enable(false);
	this->Set_Dead(true);

}

void CVampireCommander_Projectile_Range3::OnCollisionStay(CCollider* other)
{
}

void CVampireCommander_Projectile_Range3::OnCollisionExit(CCollider* other)
{
}

HRESULT CVampireCommander_Projectile_Range3::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Shader_Model"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Model_VampireCommander_Projectile_Range3"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	///* For.Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC BoundingDesc = {};
	BoundingDesc.iLayer = ECast(COLLISION_LAYER::MONSTER_ATTACK);
	BoundingDesc.fRadius = { 1.f };
	BoundingDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pCollider), &BoundingDesc)))
		return E_FAIL;

	return S_OK;
}

CVampireCommander_Projectile_Range3* CVampireCommander_Projectile_Range3::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CVampireCommander_Projectile_Range3* pInstance = new CVampireCommander_Projectile_Range3(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVampireCommander_Projectile_Range3");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CVampireCommander_Projectile_Range3::Clone(void* pArg)
{
	CVampireCommander_Projectile_Range3* pInstance = new CVampireCommander_Projectile_Range3(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVampireCommander_Projectile_Range3");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CVampireCommander_Projectile_Range3::Pool()
{
	return new CVampireCommander_Projectile_Range3(*this);
}

void CVampireCommander_Projectile_Range3::Free()
{
	__super::Free();

	Safe_Release(m_pEffect);
}
