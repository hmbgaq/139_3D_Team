#include "Mother.h"
#include "GameInstance.h"
#include "Body_Mother.h"

#include "Mother_Idle.h"
#include "Mother_Spawn.h"
#include "Mother_Hit.h"
// #include "Mother_CutScene.h"
// #include "Mother_TurnL90.h"
// #include "Mother_TurnL180.h"
// #include "Mother_TurnR90.h"
// #include "Mother_TurnR180.h"
// #include "Mother_Stun_Start.h"
// #include "Mother_CutScene.h"
// #include "Mother_BloodRange_Stun_Start.h"
// #include "Player_Finisher_Mother_VS.h"

#include "UI_Manager.h"

#include "Data_Manager.h"
#include "Player.h"

#include "Effect_Manager.h"
#include "Effect.h"

#include "Bone.h"

CMother::CMother(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CMonster_Character(pDevice, pContext, strPrototypeTag)
{
}

CMother::CMother(const CMother& rhs)
	: CMonster_Character(rhs)
{
}

HRESULT CMother::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMother::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECT_DESC		GameObjectDesc = {};

	GameObjectDesc.fSpeedPerSec = 10.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (m_pGameInstance->Get_NextLevel() != ECast(LEVEL::LEVEL_TOOL))
	{
		m_pActor = new CActor<CMother>(this);
		m_pActor->Set_State(new CMother_Spawn);
	}
	//HP
	m_fMaxHp = 1500;	
	m_fHp = m_fMaxHp;

	//m_fMaxHP = 1000.f;
	//m_fCurHP = m_fMaxHP;

	// Ready BossHUDBar
	FAILED_CHECK(CUI_Manager::GetInstance()->Ready_BossHUD_Bar(LEVEL_STATIC, this));

	CData_Manager::GetInstance()->Set_Mother(this);
	m_pTarget = CData_Manager::GetInstance()->Get_Player();
	//m_pMapEffect = EFFECT_MANAGER->Create_Effect("Test_Blood_map_04.json");
	//m_pMapEffect->Set_Position(m_pTransformCom->Get_Position());
	Search_Target(200.f);

	return S_OK;
}

void CMother::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CMother::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Search_Target(200.f);


	if (m_pActor)
	{
		m_pActor->Update_State(fTimeDelta);
	}
	
	//cout << "introBossHP:" << m_iHp << endl;
	_float fAngle = Target_Contained_Angle(Get_Transform()->Get_Look(), CData_Manager::GetInstance()->Get_Player()->Get_Transform()->Get_Pos());
	
	//cout << "Mother : " << fAngle << endl;
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
	/*if (m_bLookAt == true)*/
// 	{
// 
// 		if (0 <= fAngle && fAngle <= 180)
// 			Look_At_Target();
// 		else if (-180 <= fAngle && fAngle < 0)
// 			Look_At_Target();
// 
// 		/*m_bLookAt = false;*/
// 
// 	}
	Look_At_TargetBoss();
	//m_pTransformCom->m_fRadian += 90.f;
}

void CMother::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CMother::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}


HRESULT CMother::Ready_Components()
{
	return S_OK;
}

HRESULT CMother::Ready_PartObjects()
{
	CBody::BODY_DESC		BodyDesc = {};
	if (FAILED(Add_Body(TEXT("Prototype_GameObject_Body_Mother"), BodyDesc)))
		return E_FAIL;




	return S_OK;
}



void CMother::Hitted_Front(Power ePower)
{
	switch (ePower)
	{
	case Engine::Heavy:
	case Engine::Absolute:
		m_pActor->Set_State(new CMother_Hit);
		break;
	}
}

void CMother::Hitted_Dead(Power ePower)
{
	//stun이 걸리고 그다음에 처형이 있기 때문에 그냥 때려서는 죽일수 없다.
	//m_pActor->Set_State(new CMother_Stun_Start);
	//CPlayer* pPlayer = CData_Manager::GetInstance()->Get_Player();

}

void CMother::Hitted_Stun(Power ePower)
{
	//m_pActor->Set_State(new CMother_Stun_Start);
}

void CMother::Hitted_Finish()
{
	//m_pActor->Set_State(new CMother_CutScene());
	//
	//CPlayer* pPlayer = Set_Player_Finisher_Pos(_float3(0.f, 0.f, 2.0f));
	//pPlayer->Get_Actor()->Set_State(new CPlayer_Finisher_Mother_VS());
}

void CMother::Hitted_Weakness()
{
	//m_pActor->Set_State(new CMother_BloodRange_Stun_Start());
}

CMother* CMother::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CMother* pInstance = new CMother(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMother");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CMother::Clone(void* pArg)
{
	CMother* pInstance = new CMother(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMother");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CMother::Pool()
{
	return new CMother(*this);
}

void CMother::Free()
{
	__super::Free();

	if (m_pActor)
	{
		Safe_Delete(m_pActor);
	}

	if (nullptr != m_pMapEffect)
	{
		m_pMapEffect->Set_Dead(TRUE);
	}
}
