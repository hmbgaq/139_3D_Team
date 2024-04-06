#include "Player_EnergyWhip_CloseRange_01.h"
#include "Weapon.h"
#include "GameInstance.h"
#include "Effect_Manager.h"
#include "Effect.h"
#include "Bone.h"

#include "Player_EnergyWhip_Leap.h"
#include "Player_EnergyWhip_LongRange.h"
#include "Player_EnergyWhip_Pull.h"


void CPlayer_EnergyWhip_CloseRange_01::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);
	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

	CWeapon* pWeapon = pActor->Get_Weapon(PLAYER_WEAPON_ZAPPER);
	pWeapon
		->Set_Damage(0)
		->Set_Direction(Direction::Front)
		->Set_Power(Power::Medium)
		->Set_Force(0.2f);

	pWeapon->Set_Enable(true);
	pWeapon->Set_Enable_Collisions(false);

	

}

CState<CPlayer>* CPlayer_EnergyWhip_CloseRange_01::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);


	if (false == m_bFlags[0])
	{
		m_bFlags[0] = pActor->Is_Inputable_Front(4);
		if (true == m_bFlags[0])
		{
			pActor->Set_Weapon_Collisions_Enable(PLAYER_WEAPON_ZAPPER, true);

			// 이펙트 생성 테스트
			//EFFECT_MANAGER->Create_Effect("Player/Zapper_Shield/", "Zapper_Shield_20_distortionTest.json", pActor);
			EFFECT_MANAGER->Play_Effect("Player/Zapper_Shield/", "Zapper_Shield_21_distortionTest.json", pActor);

			//pActor->Get_Body()->Collider_Off();
		}
	}
	else if (false == m_bFlags[1])
	{
		m_bFlags[1] = pActor->Is_Inputable_Front(18);
		if (true == m_bFlags[1])
		{
			pActor->Set_Weapon_Collisions_Enable(PLAYER_WEAPON_ZAPPER, false);
			//pActor->Get_Body()->Get_Collider()->Set_Enable(true);
		}
	}
	else if (false == m_bFlags[2])
	{
		m_bFlags[2] = true;
	}

	if (m_pGameInstance->Key_Down(DIK_W)) 
	{
		pActor->Search_Target(20.f);
		if (pActor->Get_Target()) 
		{
			return new CPlayer_EnergyWhip_Leap();
			//return new CPlayer_EnergyWhip_LongRange();
		}
	}

	if (m_pGameInstance->Key_Down(DIK_S))
	{
		pActor->Search_Target(20.f);
		if (pActor->Get_Target())
		{
			return new CPlayer_EnergyWhip_Pull();
		}
	}

	if (pActor->Is_Animation_End())
	{
		return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
	}

	return nullptr;
}

void CPlayer_EnergyWhip_CloseRange_01::Release(CPlayer* pActor)
{
	__super::Release(pActor);
	CWeapon* pWeapon = pActor->Set_Weapon_Enable(PLAYER_WEAPON_ZAPPER, false);

	
}
