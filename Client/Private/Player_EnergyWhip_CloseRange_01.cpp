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
}

CState<CPlayer>* CPlayer_EnergyWhip_CloseRange_01::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	if (m_pGameInstance->Key_Down(DIK_W)) 
	{
		return new CPlayer_EnergyWhip_Leap();
		//return new CPlayer_EnergyWhip_LongRange();
	}

	if (m_pGameInstance->Key_Down(DIK_S))
	{
		return new CPlayer_EnergyWhip_Pull();
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
}
