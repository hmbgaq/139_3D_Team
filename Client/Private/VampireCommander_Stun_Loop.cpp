#include "VampireCommander_Stun_Loop.h"
#include "GameInstance.h"

void CVampireCommander_Stun_Loop::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true);
	//1번 7번
}

CState<CVampireCommander>* CVampireCommander_Stun_Loop::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	PlaySound_Breath(fTimeDelta);
	//마지막에 때려서 맞으면 루프 상태에서 강제로 Stun_Stop으로 변경 시킬 거임 플레이어쪽에서 이거 죽이거나 동영상으로 보내거나 하는 거 넣어둬야 함
	return nullptr;
}

void CVampireCommander_Stun_Loop::Release(CVampireCommander* pActor)
{
	__super::Release(pActor);

	m_fSoundTimeAcc = 0.f;
	m_bBreathChange = false; //! 1번 숨결 2번 숨결 번갈아가면서
}

void CVampireCommander_Stun_Loop::PlaySound_Breath(_float fTimeDelta)
{
	
	//!_float	m_fSoundTimeAcc = 0.f;
	//!_float  m_fBreathPlayTime = 1.5f;
	//!_bool   m_bBreathChange = false; //! 1번 숨결 2번 숨결 번갈아가면서
	
	m_fSoundTimeAcc += fTimeDelta;


	if (m_fSoundTimeAcc >= m_fBreathPlayTime)
	{
		if (m_bBreathChange == true)
		{
			m_pGameInstance->Play_Sound(L"VAMPIRE_IDLE", L"commander_lesser_vo_idle001.wav", SOUND_ENEMY_VOICE, 7.f);
			m_bBreathChange = false;
		}
		else
		{
			m_pGameInstance->Play_Sound(L"VAMPIRE_IDLE", L"commander_lesser_vo_idle007.wav", SOUND_ENEMY_VOICE2, 7.f);
			m_bBreathChange = true;
		}

		m_fSoundTimeAcc = 0.f;
	}

}
