#include "Mother_SpittingEnd.h"
#include "Mother_Idle.h"
#include "GameInstance.h"

void CMother_SpittingEnd::Initialize(CMother* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
	m_pGameInstance->Play_Sound(L"MOTHER_ATTACK", L"grand parasiter_vo_attack_push001.wav", SOUND_ENEMY_VOICE2, 9.f);
}

CState<CMother>* CMother_SpittingEnd::Update(CMother* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return new CMother_Idle;
	}
	return nullptr;
}

void CMother_SpittingEnd::Release(CMother* pActor)
{
	
	__super::Release(pActor);
}
