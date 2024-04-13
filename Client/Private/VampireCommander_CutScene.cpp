#include "VampireCommander_CutScene.h"
#include "GameInstance.h"

void CVampireCommander_CutScene::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

	pActor->Get_Body()->Collider_Off();
	m_pGameInstance->Play_Sound(L"VAMPIRE_IDLE", L"commander_lesser_vo_idle001.wav", SOUND_ENEMY_VOICE, 7.f);
}

CState<CVampireCommander>* CVampireCommander_CutScene::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	if (false == m_bFlags[0])
	{
		m_bFlags[0] = pActor->Is_Inputable_Front(32);
		if (true == m_bFlags[0])
		{
			m_pGameInstance->Play_Sound(L"VAMPIRE_HIT", L"commander_lesser_vo_hit001.wav", SOUND_ENEMY_VOICE2, 7.f);
		}
	}

	if (false == m_bFlags[1])
	{
		m_bFlags[1] = pActor->Is_Inputable_Front(104);
		if (true == m_bFlags[1])
		{
			m_pGameInstance->Play_Sound(L"VAMPIRE_HIT", L"commander_lesser_vo_hit001.wav", SOUND_ENEMY_VOICE, 7.f);
		}
	}

	if (false == m_bFlags[2])
	{
		m_bFlags[2] = pActor->Is_Inputable_Front(180);
		if (true == m_bFlags[2])
		{
			m_pGameInstance->Play_Sound(L"VAMPIRE_HIT", L"commander_lesser_vo_hit002.wav", SOUND_ENEMY_VOICE2, 7.f);
		}
	}

	if (false == m_bFlags[3])
	{
		m_bFlags[3] = pActor->Is_Inputable_Front(253);
		if (true == m_bFlags[3])
		{
			m_pGameInstance->Play_Sound(L"VAMPIRE_HIT", L"commander_lesser_vo_hit001.wav", SOUND_ENEMY_VOICE, 7.f);
		}
	}

	if (false == m_bFlags[4])
	{
		m_bFlags[4] = pActor->Is_Inputable_Front(322);
		if (true == m_bFlags[4])
		{
			m_pGameInstance->Play_Sound(L"VAMPIRE_HIT", L"commander_lesser_vo_hit_strong005.wav", SOUND_ENEMY_VOICE2, 8.f);
		}
	}

	if (m_bFlags[4] == true && m_bFlags[5] == false)
	{
		PlaySound_Breath(fTimeDelta);
	}

	if (false == m_bFlags[5])
	{
		m_bFlags[5] = pActor->Is_Inputable_Front(564);
		if (true == m_bFlags[5])
		{
			m_pGameInstance->Play_Sound(L"VAMPIRE_HIT", L"commander_lesser_vo_hit001.wav", SOUND_ENEMY_VOICE, 7.f);
		}
	}

	if (m_bFlags[5] == true && m_bFlags[6] == false)
	{
		PlaySound_Breath(fTimeDelta);
	}

	if (false == m_bFlags[6])
	{
		m_bFlags[6] = pActor->Is_Inputable_Front(707);
		if (true == m_bFlags[6])
		{
			m_pGameInstance->Play_Sound(L"VAMPIRE_HIT", L"commander_lesser_vo_hit001.wav", SOUND_ENEMY_VOICE2, 7.f);
		}
	}

	if (m_bFlags[6] == true && m_bFlags[7] == false)
	{
		PlaySound_Breath(fTimeDelta);
	}
	if (false == m_bFlags[7])
	{
		m_bFlags[7] = pActor->Is_Inputable_Front(860);
		if (true == m_bFlags[7])
		{
			m_pGameInstance->Play_Sound(L"VAMPIRE_DEATH", L"commander_lesser_vo_death004.wav", SOUND_ENEMY_VOICE, 9.f);
		}
	}

	if (pActor->Is_Animation_End())
	{
		pActor->m_bCntDead_Active = true;

	}

	return nullptr;
}

void CVampireCommander_CutScene::Release(CVampireCommander* pActor)
{
	__super::Release(pActor);
}

void CVampireCommander_CutScene::PlaySound_Breath(_float fTimeDelta)
{
	//!_float	m_fSoundTimeAcc = 0.f;
		//!_float  m_fBreathPlayTime = 1.5f;
		//!_bool   m_bBreathChange = false; //! 1번 숨결 2번 숨결 번갈아가면서

	m_fSoundTimeAcc += fTimeDelta;


	if (m_fSoundTimeAcc >= m_fBreathPlayTime)
	{
		if (m_bBreathChange == true)
		{
			m_pGameInstance->Play_Sound(L"VAMPIRE_IDLE", L"commander_lesser_vo_idle001.wav", SOUND_ENEMY_HIT, 7.f);
			m_bBreathChange = false;
		}
		else
		{
			m_pGameInstance->Play_Sound(L"VAMPIRE_IDLE", L"commander_lesser_vo_idle007.wav", SOUND_ENEMY_HIT2, 7.f);
			m_bBreathChange = true;
		}

		m_fSoundTimeAcc = 0.f;
	}
}
