#include "VampireCommander.h"

#include "GameInstance.h"
#include "Body_VampireCommander.h"

#include "VampireCommander_Idle.h"
#include "VampireCommander_Spawn1.h"
#include "VampireCommander_HitCenter.h"
#include "VampireCommander_HitLeft.h"
#include "VampireCommander_HitRight.h"
#include "VampireCommander_CutScene.h"
#include "VampireCommander_TurnL90.h"
#include "VampireCommander_TurnL180.h"
#include "VampireCommander_TurnR90.h"
#include "VampireCommander_TurnR180.h"
#include "VampireCommander_Stun_Start.h"
#include "VampireCommander_CutScene.h"
#include "VampireCommander_BloodRange_Stun_Start.h"
#include "Player_Finisher_VampireCommander_VS.h"

//#include "UI_Manager.h"

#include "Data_Manager.h"
#include "Player.h"

#include "Effect_Manager.h"
#include "Effect.h"

CVampireCommander::CVampireCommander(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CMonster_Character(pDevice, pContext, strPrototypeTag)
{
}

CVampireCommander::CVampireCommander(const CVampireCommander& rhs)
	: CMonster_Character(rhs)
{
}

HRESULT CVampireCommander::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CVampireCommander::Initialize(void* pArg)
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
		m_pActor = new CActor<CVampireCommander>(this);
		m_pActor->Set_State(new CVampireCommander_Spawn1);
	}
	//HP
	m_fMaxHp = 1000;
	m_fHp = m_fMaxHp;

	//m_fMaxHP = 1000.f;
	//m_fCurHP = m_fMaxHP;

	// Ready BossHUDBar
	FAILED_CHECK(CUI_Manager::GetInstance()->Ready_BossHUD_Bar(LEVEL_STATIC, this));

	m_vWeaknessPos_Local = _float3(0.f, 2.f, 0.f);


	m_pMapEffect = EFFECT_MANAGER->Create_Effect("VampireCommander/Map_Blood/", "Map_Blood_04.json", m_pTransformCom->Get_Position());


	if (nullptr == m_pTarget)
	{
		m_pTarget = CData_Manager::GetInstance()->Get_Player();
	}

	return S_OK;
}

void CVampireCommander::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CVampireCommander::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	
	if (m_iCurrnetLevel != (_uint)LEVEL_TOOL)
	{
		if (m_pActor)
		{
			m_pActor->Update_State(fTimeDelta);
		}
		//cout << "introBossHP:" << m_iHp << endl;
		_float fAngle = Target_Contained_Angle(Get_Transform()->Get_Look(), Get_Target()->Get_Transform()->Get_Pos());

		//cout << "VampireCommander : " << fAngle << endl;
		if (m_bLookAt == true)
		{

			if (0 <= fAngle && fAngle <= 45)
				Look_At_Target_Lerp(fTimeDelta);
			else if (-45 <= fAngle && fAngle < 0)
				Look_At_Target_Lerp(fTimeDelta);

			/*m_bLookAt = false;*/

		}
	}
	//Search_Target(200.f);

	
	
}

void CVampireCommander::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CVampireCommander::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

_float CVampireCommander::Ratation_Target_Test()
{
	_vector vPlayerPos = XMVector3Normalize(CData_Manager::GetInstance()->Get_Player()->Get_Transform()->Get_State(CTransform::STATE_POSITION));
	_vector vMonsterPos = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	//시선벡터 계산 
	_float dotProduct = XMVectorGetX(XMVector3Dot(vPlayerPos, vMonsterPos));
	_float Radian = acosf(dotProduct);
	_float Degree = XMConvertToRadians(Radian);

	return Degree;

}

HRESULT CVampireCommander::Ready_Components()
{
	return S_OK;
}

HRESULT CVampireCommander::Ready_PartObjects()
{
	CBody::BODY_DESC		BodyDesc = {};
	if (FAILED(Add_Body(TEXT("Prototype_GameObject_Body_VampireCommander"), BodyDesc)))
		return E_FAIL;

	CWeapon::WEAPON_DESC		WeaponDesc = {};
	FAILED_CHECK(Add_Weapon(TEXT("Prototype_GameObject_VampireCommander_Weapon_Hand"), "LeftHandIK", WeaponDesc, TEXT("Weapon_hand_L")));
	FAILED_CHECK(Add_Weapon(TEXT("Prototype_GameObject_VampireCommander_Weapon_Hand"), "RightHandIK", WeaponDesc, TEXT("Weapon_hand_R")));

	CWeapon* m_pWeapon_Punch_L = Get_Weapon(TEXT("Weapon_hand_L"));
	m_pWeapon_Punch_L->Set_Enable(false);

	CWeapon* m_pWeapon_Punch_R = Get_Weapon(TEXT("Weapon_hand_R"));
	m_pWeapon_Punch_R->Set_Enable(false);


	return S_OK;
}

void CVampireCommander::Hitted_Left(Power ePower)
{
	switch (ePower)
	{
	case Engine::Heavy:
	case Engine::Absolute:
		m_pActor->Set_State(new CVampireCommander_HitRight);
		break;
	}
	
}

void CVampireCommander::Hitted_Right(Power ePower)
{
	switch (ePower)
	{
	case Engine::Heavy:
	case Engine::Absolute:
		m_pActor->Set_State(new CVampireCommander_HitLeft);
	break;
	}
}


void CVampireCommander::Hitted_Front(Power ePower)
{
	switch (ePower)
	{
	case Engine::Heavy:
	case Engine::Absolute:
		m_pActor->Set_State(new CVampireCommander_HitCenter);
	break;
	}
}

void CVampireCommander::Hitted_Dead(Power ePower)
{
	//stun이 걸리고 그다음에 처형이 있기 때문에 그냥 때려서는 죽일수 없다.
	m_pActor->Set_State(new CVampireCommander_Stun_Start);
	//CPlayer* pPlayer = CData_Manager::GetInstance()->Get_Player();
	
}

void CVampireCommander::Hitted_Stun(Power ePower)
{
	m_pActor->Set_State(new CVampireCommander_Stun_Start);
}

void CVampireCommander::Hitted_Finish()
{
	m_pActor->Set_State(new CVampireCommander_CutScene());

	CPlayer* pPlayer = Set_Player_Finisher_Pos(_float3(0.f, 0.f, 2.0f));
	pPlayer->Get_Actor()->Set_State(new CPlayer_Finisher_VampireCommander_VS());
}

void CVampireCommander::Hitted_Weakness()
{
	m_pActor->Set_State(new CVampireCommander_BloodRange_Stun_Start());
}

CVampireCommander* CVampireCommander::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CVampireCommander* pInstance = new CVampireCommander(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVampireCommander");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CVampireCommander::Clone(void* pArg)
{
	CVampireCommander* pInstance = new CVampireCommander(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVampireCommander");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CVampireCommander::Pool()
{
	return new CVampireCommander(*this);
}

void CVampireCommander::Free()
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
