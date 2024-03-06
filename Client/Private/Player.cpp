#include "Player.h"
#include "GameInstance.h"
#include "Body_Player.h"
#include "Weapon_Player.h"
#include "Player_IdleLoop.h"
#include "Data_Manager.h"
#include "PhysXController.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CCharacter(pDevice, pContext, strPrototypeTag)
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CCharacter(rhs)
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECT_DESC		GameObjectDesc = {};

	GameObjectDesc.fSpeedPerSec = 10.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	FAILED_CHECK(__super::Initialize(&GameObjectDesc));

// 	if (m_pGameInstance->Get_NextLevel() != ECast(LEVEL::LEVEL_TOOL))
// 	{
		m_pActor = new CActor<CPlayer>(this);
		m_pActor->Set_State(new CPlayer_IdleLoop());
// 	}

	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	/* For.Com_PhysXController */
	FAILED_CHECK(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_PhysXController"), TEXT("Com_PhysXController"), reinterpret_cast<CComponent**>(&m_pPhysXControllerCom)));
	m_pPhysXControllerCom->Init_Controller(Preset::PhysXControllerDesc::PlayerSetting(m_pTransformCom), (_uint)PHYSX_COLLISION_LAYER::PLAYER);

	CData_Manager::GetInstance()->Set_Player(this);

	/* Temp - 맵에 맞게 위치 조정한값*/
	//m_pTransformCom->Set_State(CTransform::STATE::STATE_POSITION, XMVectorSet(-26.f, 0.f, -6.f, 1.f));

	return S_OK;
}

void CPlayer::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CPlayer::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_pActor)
	{
		m_pActor->Update_State(fTimeDelta);
	}
	
}

void CPlayer::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}
	
HRESULT CPlayer::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CPlayer::Aim_Walk(_float fTimeDelta)
{
	_uint AnimIndex;

	if (m_pGameInstance->Key_Pressing(DIK_W))
	{
		if (m_pGameInstance->Key_Pressing(DIK_A))
		{
			AnimIndex = ECast(CPlayer::Player_State::Player_Walk_FL45);
			if (Get_CurrentAnimIndex() != AnimIndex)
			{
				Set_Animation(AnimIndex, CModel::ANIM_STATE_LOOP, true, false);
			}
			Go_Straight_L45(fTimeDelta * 0.5f);
		}
		else if (m_pGameInstance->Key_Pressing(DIK_D))
		{
			AnimIndex = ECast(CPlayer::Player_State::Player_Walk_FR45);
			if (Get_CurrentAnimIndex() != AnimIndex)
			{
				Set_Animation(AnimIndex, CModel::ANIM_STATE_LOOP, true, false);
			}
			Go_Straight_R45(fTimeDelta * 0.5f);
		}
		else
		{
			AnimIndex = ECast(CPlayer::Player_State::Player_Walk_F);
			if (Get_CurrentAnimIndex() != AnimIndex)
			{
				Set_Animation(AnimIndex, CModel::ANIM_STATE_LOOP, true, false);
			}
			Go_Straight(fTimeDelta * 0.5f);
		}
	}
	else if (m_pGameInstance->Key_Pressing(DIK_S))
	{
		if (m_pGameInstance->Key_Pressing(DIK_A))
		{
			AnimIndex = ECast(CPlayer::Player_State::Player_Walk_BL135);
			if (Get_CurrentAnimIndex() != AnimIndex)
			{
				Set_Animation(AnimIndex, CModel::ANIM_STATE_LOOP, true, false);
			}
			Go_Backward_L45(fTimeDelta * 0.5f);
		}
		else if (m_pGameInstance->Key_Pressing(DIK_D))
		{
			AnimIndex = ECast(CPlayer::Player_State::Player_Walk_BR135);
			if (Get_CurrentAnimIndex() != AnimIndex)
			{
				Set_Animation(AnimIndex, CModel::ANIM_STATE_LOOP, true, false);
			}
			Go_Backward_R45(fTimeDelta * 0.5f);
		}
		else
		{
			AnimIndex = ECast(CPlayer::Player_State::Player_Walk_B);
			if (Get_CurrentAnimIndex() != AnimIndex)
			{
				Set_Animation(AnimIndex, CModel::ANIM_STATE_LOOP, true, false);
			}
			Go_Backward(fTimeDelta * 0.5f);
		}
	}
	else if (m_pGameInstance->Key_Pressing(DIK_A))
	{
		AnimIndex = ECast(CPlayer::Player_State::Player_Walk_FL);
		if (Get_CurrentAnimIndex() != AnimIndex)
		{
			Set_Animation(AnimIndex, CModel::ANIM_STATE_LOOP, true, false);
		}
		Go_Left(fTimeDelta * 0.5f);
	}
	else if (m_pGameInstance->Key_Pressing(DIK_D))
	{
		AnimIndex = ECast(CPlayer::Player_State::Player_Walk_FR);
		if (Get_CurrentAnimIndex() != AnimIndex)
		{
			Set_Animation(AnimIndex, CModel::ANIM_STATE_LOOP, true, false);
		}
		Go_Right(fTimeDelta * 0.5f);
	}
	else
	{
		AnimIndex = ECast(CPlayer::Player_State::Player_IdleLoop);
		if (Get_CurrentAnimIndex() != AnimIndex)
		{
			Set_Animation(AnimIndex, CModel::ANIM_STATE_LOOP, true, false);
		}
	}
}

HRESULT CPlayer::Ready_Components()
{
	return S_OK;
}

HRESULT CPlayer::Ready_PartObjects()
{
	CBody::BODY_DESC		BodyDesc = {};
	FAILED_CHECK(Add_Body(TEXT("Prototype_GameObject_Body_Player"), BodyDesc));

	//if (m_pGameInstance->Get_NextLevel() != ECast(LEVEL_TOOL))
	//{
		
		CWeapon::WEAPON_DESC		WeaponDesc = {};
		FAILED_CHECK(Add_Weapon(TEXT("Prototype_GameObject_Player_Weapon_Punch"), "LeftHandIK", WeaponDesc, TEXT("Weapon_Punch_L")));
		FAILED_CHECK(Add_Weapon(TEXT("Prototype_GameObject_Player_Weapon_Punch"), "RightHandIK", WeaponDesc, TEXT("Weapon_Punch_R")));

	//}

	CWeapon* m_pWeapon_Punch_L = Get_Weapon(TEXT("Weapon_Punch_L"));
	m_pWeapon_Punch_L->Set_Enable(false);
	
	CWeapon* m_pWeapon_Punch_R = Get_Weapon(TEXT("Weapon_Punch_R"));
	m_pWeapon_Punch_R->Set_Enable(false);
	
	return S_OK;
}

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPlayer::Pool()
{
	return new CPlayer(*this);
}

void CPlayer::Free()
{
	__super::Free();

	if (m_pActor)
	{
		Safe_Delete(m_pActor);
	}
	
}
