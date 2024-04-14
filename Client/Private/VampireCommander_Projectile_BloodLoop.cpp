#include "VampireCommander_Projectile_BloodLoop.h"
#include "GameInstance.h"
#include "Character.h"
#include "Data_Manager.h"
#include "Player.h"
#include "Effect.h"
#include "Effect_Manager.h"
#include "VampireCommander.h"

CVampireCommander_Projectile_BloodLoop::CVampireCommander_Projectile_BloodLoop(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CProjectile(pDevice, pContext, strPrototypeTag)
{
}

CVampireCommander_Projectile_BloodLoop::CVampireCommander_Projectile_BloodLoop(const CVampireCommander_Projectile_BloodLoop& rhs)
	: CProjectile(rhs)
{
}

HRESULT CVampireCommander_Projectile_BloodLoop::Initialize_Prototype()
{

	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CVampireCommander_Projectile_BloodLoop::Initialize(void* pArg)
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

		m_pMonster = dynamic_cast<CCharacter*>(pGameObject);
		m_vMonsterPos = m_pMonster->Get_Transform()->Get_State(CTransform::STATE_POSITION) + 8 * m_pMonster->Get_Transform()->Get_State(CTransform::STATE_LOOK);
		m_pTransformCom->Set_WorldMatrix(m_pMonster->Get_Transform()->Get_WorldMatrix());
	}

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vMonsterPos);

	//m_vPlayerPos = CData_Manager::GetInstance()->Get_Player()->Get_Transform()->Get_State(CTransform::STATE_POSITION) /*+ CData_Manager::GetInstance()->Get_Player()->Get_Transform()->Get_State(CTransform::STATE_UP)*/;

	m_pTransformCom->Look_At(m_pMonster->Get_Position_Vector());

	m_fDamage = 10.f;


	// 이펙트 생성
	//m_pEffect = EFFECT_MANAGER->Play_Effect("VampireCommander/Projectile_Range3/", "Projectile_Range3_02.json", this);

	return S_OK;
}

void CVampireCommander_Projectile_BloodLoop::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CVampireCommander_Projectile_BloodLoop::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	//생성되는 위치에서 그냥 앞방향으로 ㄱㄱ 
	m_pTransformCom->Look_At(m_pMonster->Get_Position_Vector());

	m_pTransformCom->Go_Straight(fTimeDelta);

	//EFFECT_MANAGER->Play_Effect(&m_fEffectTimeAcc, 0.32f, fTimeDelta, "VampireCommander/Projectile_Range3/", "Projectile_Pillar_Tick_10.json", Get_Position(), true, m_vPlayerPos);

	//! 유정 : 두두두두 이펙트 생성 테스트
	//EFFECT_MANAGER->Generate_Effect(&m_fEffectTimeAcc, 0.18f, fTimeDelta, "VampireCommander/Projectile_Range3/", "Projectile_Range3_Tick_03.json", Get_Position(), true, m_vPlayerPos);
	EFFECT_MANAGER->Generate_Effect(&m_fEffectTimeAcc, 1.f, fTimeDelta, "VampireCommander/Projectile_Range3/", "Projectile_Pillar_Tick_10.json", Get_Position(), true, m_pMonster->Get_Position());
}

void CVampireCommander_Projectile_BloodLoop::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CVampireCommander_Projectile_BloodLoop::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CVampireCommander_Projectile_BloodLoop::Render_Shadow()
{
	//이거 필요한거? 딱히 필요는 없어 보임 
	return S_OK;
}

void CVampireCommander_Projectile_BloodLoop::OnCollisionEnter(CCollider* other)
{

	CCharacter* pTarget_Character = Get_Target_Character(other);

	if (nullptr != pTarget_Character)// 일반 타격 
	{
		pTarget_Character->Set_Hitted(m_fDamage, pTarget_Character->Calc_Look_Dir_XZ(m_pTransformCom->Get_Position()), m_fForce, 1.f, m_eHitDirection, m_eHitPower);


		// 타격 이펙트
		//CEffect* pEffect = EFFECT_MANAGER->Create_Effect("Hit/", "Hit_Normal.json", m_pTransformCom->Get_Position(), TRUE, m_pGameInstance->Get_Player()->Get_Position());
		EFFECT_MANAGER->Play_Effect("Hit/", "Hit_Normal.json", nullptr, m_pGameInstance->Get_Player()->Get_Position());
	}
	//m_pCollider->Set_Enable(false);
	//this->Set_Dead(true);

}

void CVampireCommander_Projectile_BloodLoop::OnCollisionStay(CCollider* other)
{
}

void CVampireCommander_Projectile_BloodLoop::OnCollisionExit(CCollider* other)
{
}

HRESULT CVampireCommander_Projectile_BloodLoop::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Shader_Model"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Model_VampireCommander_Projectile_BloodLoop"),
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

CVampireCommander_Projectile_BloodLoop* CVampireCommander_Projectile_BloodLoop::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CVampireCommander_Projectile_BloodLoop* pInstance = new CVampireCommander_Projectile_BloodLoop(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVampireCommander_Projectile_BloodLoop");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CVampireCommander_Projectile_BloodLoop::Clone(void* pArg)
{
	CVampireCommander_Projectile_BloodLoop* pInstance = new CVampireCommander_Projectile_BloodLoop(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVampireCommander_Projectile_BloodLoop");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CVampireCommander_Projectile_BloodLoop::Pool()
{
	return new CVampireCommander_Projectile_BloodLoop(*this);
}

void CVampireCommander_Projectile_BloodLoop::Free()
{
	__super::Free();

	Safe_Release(m_pEffect);
}
