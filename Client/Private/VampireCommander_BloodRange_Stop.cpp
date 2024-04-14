#include "VampireCommander_BloodRange_Stop.h"
#include "VampireCommander_Idle.h"
#include "GameInstance.h"

void CVampireCommander_BloodRange_Stop::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CVampireCommander>* CVampireCommander_BloodRange_Stop::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	if (m_bFlags[0] == false && pActor->Is_Inputable_Front(55))
	{
		m_pGameInstance->Play_Sound(L"VAMPIRE_TAUNT", L"commander_lesser_vo_taunt005.wav", SOUND_ENEMY_VOICE, 7.f);
		m_bFlags[0] = true;
	}

	
	if (pActor->Is_Animation_End())
	{
		return new CVampireCommander_Idle();
	}

	return nullptr;
}

void CVampireCommander_BloodRange_Stop::Release(CVampireCommander* pActor)
{
	pActor->Set_Invincible(false);
	pActor->Set_Reveal_Weakness(false);
	__super::Release(pActor);
}
