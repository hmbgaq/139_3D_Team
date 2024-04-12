#include "Son.h"

#include "GameInstance.h"
#include "Body_Son.h"

// #include "Son_Idle.h"
#include "Son_Spawn.h"
#include "Son_Hit.h"
#include "Son_Dead.h"
#include "Mother.h"

#include "UI_Manager.h"

#include "Data_Manager.h"
#include "Player.h"

#include "Effect_Manager.h"
#include "Effect.h"



CSon::CSon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CMonster_Character(pDevice, pContext, strPrototypeTag)
{
}

CSon::CSon(const CSon& rhs)
	: CMonster_Character(rhs)
{
}

HRESULT CSon::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSon::Initialize(void* pArg)
{
	if (pArg == nullptr)
	{
		CGameObject::GAMEOBJECT_DESC		GameObjectDesc = {};

		GameObjectDesc.fSpeedPerSec = 10.f;
		GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);
		FAILED_CHECK(__super::Initialize(&GameObjectDesc));
	}
	else
	{
		FAILED_CHECK(__super::Initialize(pArg));
	}

	if (m_pGameInstance->Get_NextLevel() != ECast(LEVEL::LEVEL_TOOL))
	{
		m_pActor = new CActor<CSon>(this);
		m_pActor->Set_State(new CSon_Spawn);
	}
	//HP
	m_fMaxHp = 150;
	m_fHp = m_fMaxHp;

	//m_fMaxHP = 1000.f;
	//m_fCurHP = m_fMaxHP;
	m_pMother = CData_Manager::GetInstance()->Get_Mother();
	// Ready BossHUDBar
	//FAILED_CHECK(m_pUIManager->Ready_BossHUD_Bar(LEVEL_STATIC, this));
	
	m_pTarget = CData_Manager::GetInstance()->Get_Player();
	CData_Manager::GetInstance()->Set_Son(this);

	// !UI : HUD_Create
	Ready_EnemyHUD_Shard(m_pGameInstance->Get_NextLevel(), this);

	return S_OK;
}

void CSon::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CSon::Tick(_float fTimeDelta)
{
	if (GAME_STATE::GAMEPLAY != m_pDataManager->Get_GameState())
		return;

	__super::Tick(fTimeDelta);

	m_fTimeDelta2 += fTimeDelta;

	// !UI : HUD_Position									오프셋 조절
	Check_EnemyHUD_World(m_pTransformCom->Get_WorldMatrix()/*, Offset*/);

	if (m_iCurrnetLevel != (_uint)LEVEL_TOOL)
	{
		Search_Target(200.f);

		if (m_pActor)
		{
			m_pActor->Update_State(fTimeDelta);
		}
		m_fTimeDelta += fTimeDelta;

		//if (m_fTimeDelta >= 1.f)
		//{
		//	cout << "SonHP:" << m_fHp << endl;
		//	m_fTimeDelta = 0.f;
		//}
		_float fAngle = Target_Contained_Angle(Get_Transform()->Get_Look(), CData_Manager::GetInstance()->Get_Player()->Get_Transform()->Get_Pos());

		//cout << "Son : " << fAngle << endl;
	// 	if (m_bLookAt == true)
	// 	{
	// 	
	// 		if (0 <= fAngle && fAngle <= 180)
	// 			Look_At_Target_Lerp(fTimeDelta);
	// 		else if (-180 <= fAngle && fAngle < 0)
	// 			Look_At_Target_Lerp(fTimeDelta);
	// 	
	// 		/*m_bLookAt = false;*/
	// 	
	// 	}
		if (m_bLookAt == true)
		{

			if (0 <= fAngle && fAngle <= 180)
				Look_At_Target();
			else if (-180 <= fAngle && fAngle < 0)
				Look_At_Target();

			/*m_bLookAt = false;*/

		}
		if (m_fHp <= 0.f)
		{
			m_fHp = m_fMaxHp;
			++m_pMother->m_iSonDead;

			// UI : HUD_Dead
			Set_EnemyHUD_Dead();

			//여기서 UI체력도 꺼버렸다가 켜지면 다 같이 켜지게 만들어야 함 ! 
			m_pActor->Set_State(new CSon_Dead);
		}
		if (m_fTimeDelta2 >= 0.3f && m_fHp <= 0.f)
		{
			m_pEffect = EFFECT_MANAGER->Play_Effect("Parasiter/", "Monster_Blood4.json", this, true, "Bone020");
			//cout << "MotherBossHP:" << m_fHp << endl;
			m_fTimeDelta2 = 0.f;
		}
	}
	

}

void CSon::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CSon::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSon::Ready_Components()
{
	return S_OK;
}

HRESULT CSon::Ready_PartObjects()
{
	CBody::BODY_DESC		BodyDesc = {};
	if (FAILED(Add_Body(TEXT("Prototype_GameObject_Body_Son"), BodyDesc)))
		return E_FAIL;

	CWeapon::WEAPON_DESC		WeaponDesc = {};
	FAILED_CHECK(Add_Weapon(TEXT("Prototype_GameObject_Son_Weapon_Head"), "Bone020", WeaponDesc, TEXT("Weapon_head")));
	
	//! 가라 몸전용 콜라이더 
	CWeapon::WEAPON_DESC		ColliderBodyDesc = {};
	FAILED_CHECK(Add_Weapon(TEXT("Prototype_GameObject_Son_ColliderBody"), "Bone020", ColliderBodyDesc, TEXT("Body")));

	
	
	CWeapon* m_pWeapon_Punch_R = Get_Weapon(TEXT("Weapon_head"));
	m_pWeapon_Punch_R->Set_Enable(true);


	return S_OK;
}


void CSon::Hitted_Front(Power ePower)
{
	switch (ePower)
	{
	case Engine::Heavy:
	case Engine::Absolute:
		m_pActor->Set_State(new CSon_Hit);
		break;
	}
}

void CSon::Hitted_Dead(Power ePower)
{

}

void CSon::Hitted_Stun(Power ePower)
{
	//m_pActor->Set_State(new CSon_Stun_Start);
}

void CSon::Hitted_Finish()
{
// 	m_pActor->Set_State(new CSon_CutScene());
// 
// 	CPlayer* pPlayer = Set_Player_Finisher_Pos(_float3(0.f, 0.f, 2.0f));
// 	pPlayer->Get_Actor()->Set_State(new CPlayer_Finisher_Son_VS());
}

void CSon::Hitted_Weakness()
{
	//m_pActor->Set_State(new CSon_BloodRange_Stun_Start());
}

void CSon::Check_Frustum()
{
	m_bIsInFrustum = true;

}

CSon* CSon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CSon* pInstance = new CSon(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CSon::Clone(void* pArg)
{
	CSon* pInstance = new CSon(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CSon::Pool()
{
	return new CSon(*this);
}

void CSon::Free()
{
	__super::Free();

	if (m_pActor)
	{
		Safe_Delete(m_pActor);
	}

	if (nullptr != m_pEffect)
		Safe_Release(m_pEffect);

// 	if (nullptr != m_pMapEffect)
// 	{
// 		m_pMapEffect->Set_Dead(TRUE);
// 	}
}
