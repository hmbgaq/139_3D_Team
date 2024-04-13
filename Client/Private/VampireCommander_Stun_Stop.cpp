#include "VampireCommander_Stun_Stop.h"
#include "VampireCommander_Idle.h"
#include "GameInstance.h"

void CVampireCommander_Stun_Stop::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
	m_pGameInstance->Play_Sound(L"VAMPIRE_HIT", L"commander_lesser_vo_hit_strong005.wav", SOUND_ENEMY_VOICE, 6.f);
}

CState<CVampireCommander>* CVampireCommander_Stun_Stop::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return new CVampireCommander_Idle;
	}
	return nullptr;
}

void CVampireCommander_Stun_Stop::Release(CVampireCommander* pActor)
{
	__super::Release(pActor);
}
