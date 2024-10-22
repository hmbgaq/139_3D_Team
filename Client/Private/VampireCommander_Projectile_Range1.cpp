#include "VampireCommander_Projectile_Range1.h"
#include "GameInstance.h"
#include "Character.h"
#include "Data_Manager.h"
#include "Player.h"
#include "Effect.h"
#include "Effect_Manager.h"
#include "VampireCommander.h"

CVampireCommander_Projectile_Range1::CVampireCommander_Projectile_Range1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CProjectile(pDevice,pContext,strPrototypeTag)
{
}

CVampireCommander_Projectile_Range1::CVampireCommander_Projectile_Range1(const CVampireCommander_Projectile_Range1& rhs)
	:CProjectile(rhs)
{
}

HRESULT CVampireCommander_Projectile_Range1::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CVampireCommander_Projectile_Range1::Initialize(void* pArg)
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
		m_vMonsterPos = m_pMonster->Get_Transform()->Get_State(CTransform::STATE_POSITION) + 1.5f * m_pMonster->Get_Transform()->Get_State(CTransform::STATE_UP);
		m_pTransformCom->Set_WorldMatrix(m_pMonster->Get_Transform()->Get_WorldMatrix());
	}

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vMonsterPos);

	m_vPlayerPos = CData_Manager::GetInstance()->Get_Player()->Get_Transform()->Get_State(CTransform::STATE_POSITION) + 1.5f * CData_Manager::GetInstance()->Get_Player()->Get_Transform()->Get_State(CTransform::STATE_UP);
	
	m_pTransformCom->Look_At(m_vPlayerPos );

	m_fDamage = 30.f;

	// 이펙트 생성
	//m_pEffect = EFFECT_MANAGER->Play_Effect("VampireCommander/Projectile_Range1/", "Projectile_Range1_04.json", this);
	//m_pEffect = EFFECT_MANAGER->Create_Effect("VampireCommander/Projectile_Range1/", "Projectile_Range1_04.json", this);

	m_pEffect = EFFECT_MANAGER->Play_Effect("VampireCommander/Projectile_Range1/", "Projectile_Range1_Re_02.json", this);

	return S_OK;
}

void CVampireCommander_Projectile_Range1::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CVampireCommander_Projectile_Range1::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	//생성되는 위치에서 그냥 앞방향으로 ㄱㄱ 
	m_pTransformCom->Go_Straight(fTimeDelta);


	if (0 >= m_fLifeTime)
		m_vEffectScaleAcc = { 0.01f, 0.01f, 0.01f };

	//! 유정 : 두두두두 이펙트 생성
	//EFFECT_MANAGER->Generate_Effect(&m_fEffectTimeAcc, 0.1f, fTimeDelta, "VampireCommander/Projectile_Range1/", "Projectile_Range1_Re_02.json", Get_Position(), true, m_vPlayerPos
	//, true, &m_vEffectScaleAcc);
	
}

void CVampireCommander_Projectile_Range1::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CVampireCommander_Projectile_Range1::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CVampireCommander_Projectile_Range1::Render_Shadow()
{
	//이거 필요한거? 딱히 필요는 없어 보임 
	return S_OK;
}

void CVampireCommander_Projectile_Range1::OnCollisionEnter(CCollider* other)
{
	
	CCharacter* pTarget_Character = Get_Target_Character(other);

	if (nullptr != pTarget_Character)// 일반 타격 
	{
		pTarget_Character->Set_Hitted(m_fDamage, pTarget_Character->Calc_Look_Dir_XZ(m_pTransformCom->Get_Position()), m_fForce, 1.f, m_eHitDirection, m_eHitPower);
		
		// 타격 이펙트
		EFFECT_MANAGER->Play_Effect("Hit/", "Hit_Normal.json", nullptr, m_pGameInstance->Get_Player()->Get_Position());
		//CEffect* pEffect = EFFECT_MANAGER->Create_Effect("Hit/", "Hit_Normal.json", m_pTransformCom->Get_Position(), TRUE, m_pGameInstance->Get_Player()->Get_Position());
		//CEffect* pEffect = EFFECT_MANAGER->Create_Effect("Hit/", "Hit_Distortion.json", m_pTransformCom->Get_Position());
	}
	//m_pCollider->Set_Enable(false);
	m_vEffectScaleAcc = {0.01f, 0.01f, 0.01f };
	this->Set_Dead(true);

}

void CVampireCommander_Projectile_Range1::OnCollisionStay(CCollider* other)
{
}

void CVampireCommander_Projectile_Range1::OnCollisionExit(CCollider* other)
{
}

HRESULT CVampireCommander_Projectile_Range1::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Shader_Model"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Model_VampireCommander_Projectile_Range1"),
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

CVampireCommander_Projectile_Range1* CVampireCommander_Projectile_Range1::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CVampireCommander_Projectile_Range1* pInstance = new CVampireCommander_Projectile_Range1(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVampireCommander_Projectile_Range1");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CVampireCommander_Projectile_Range1::Clone(void* pArg)
{
	CVampireCommander_Projectile_Range1* pInstance = new CVampireCommander_Projectile_Range1(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVampireCommander_Projectile_Range1");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CVampireCommander_Projectile_Range1::Pool()
{
	return new CVampireCommander_Projectile_Range1(*this);
}

void CVampireCommander_Projectile_Range1::Free()
{
	__super::Free();


	Safe_Release(m_pEffect);

}
