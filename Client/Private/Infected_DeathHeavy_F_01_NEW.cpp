#include "stdafx.h"
#include "Infected_DeathHeavy_F_01_NEW.h"
#include "GameInstance.h"

void CInfected_DeathHeavy_F_01_NEW::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
	m_pGameInstance->Play_Sound(L"INFECTED_DEATH", L"death_3_turned01.wav", SOUND_ENEMY_DEAD, 7.f);
}

CState<CInfected>* CInfected_DeathHeavy_F_01_NEW::Update(CInfected* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
	}

	return nullptr;
}

void CInfected_DeathHeavy_F_01_NEW::Release(CInfected* pActor)
{	
	__super::Release(pActor);
}
