#include "Mother_VomitEnd.h"
#include "Mother_Idle.h"
#include "GameInstance.h"

void CMother_VomitEnd::Initialize(CMother* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
	m_pGameInstance->Play_Sound(L"MOTHER_ATTACK", L"grand_parasiter_attack_vomit_start.wav", SOUND_ENEMY_SKILL4, 10.f);
}

CState<CMother>* CMother_VomitEnd::Update(CMother* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return new CMother_Idle;
	}

	return nullptr;
}

void CMother_VomitEnd::Release(CMother* pActor)
{
	__super::Release(pActor);
	//pActor->m_bfirstCheck = true;
}
