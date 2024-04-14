#include "VampireCommander_Stun_Start.h"
#include "VampireCommander_Stun_Loop.h"
#include "GameInstance.h"

void CVampireCommander_Stun_Start::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
	m_pGameInstance->Play_Sound(L"VAMPIRE_HIT", L"commander_lesser_vo_hit_strong003.wav", SOUND_ENEMY_VOICE, 6.f);
	
}

CState<CVampireCommander>* CVampireCommander_Stun_Start::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	if (m_bFlags[0] == false && pActor->Is_Inputable_Front(71))
	{
		m_pGameInstance->Play_Sound(L"VAMPIRE_HIT", L"commander_lesser_vo_hit002.wav", SOUND_ENEMY_VOICE, 6.f);
		m_bFlags[0] = true;
	}

	if (pActor->Is_Animation_End())
	{
		return new CVampireCommander_Stun_Loop();
	}

	return nullptr;
}

void CVampireCommander_Stun_Start::Release(CVampireCommander* pActor)
{
	__super::Release(pActor);
}
