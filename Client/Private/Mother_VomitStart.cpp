#include "Mother_VomitStart.h"
#include "Mother_VomitLoop.h"

#include "Effect_Manager.h"
#include "Effect.h"
#include "GameInstance.h"

void CMother_VomitStart::Initialize(CMother* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

	m_pGameInstance->Play_Sound(L"MOTHER_ATTACK", L"grand_parasiter_attack_vomit_start.wav", SOUND_ENEMY_SKILL1, 10.f);
	m_pGameInstance->Play_Sound(L"MOTHER_ATTACK", L"stalker_vo_taunt005.wav", SOUND_ENEMY_VOICE, 10.f);

	EFFECT_MANAGER->Play_Effect("Parasiter/Mother_Breath/", "Mother_Breath_Ready_02.json", pActor, true, "Jaws_Center");
	EFFECT_MANAGER->Play_Effect("Parasiter/Mother_Breath/", "Mother_Breath_Static_02.json", pActor, true, "Jaws_Center");
}

CState<CMother>* CMother_VomitStart::Update(CMother* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return new CMother_VomitLoop;
	}

	return nullptr;
}

void CMother_VomitStart::Release(CMother* pActor)
{
	__super::Release(pActor);
}
