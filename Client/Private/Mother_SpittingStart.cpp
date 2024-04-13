#include "Mother_SpittingStart.h"
#include "Mother_SpittingLoop.h"
#include "GameInstance.h"

void CMother_SpittingStart::Initialize(CMother* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CMother>* CMother_SpittingStart::Update(CMother* pActor, _float fTimeDelta)
{
	if (false == m_bFlags[0])
	{
		m_bFlags[0] = pActor->Is_Inputable_Front(17);

		if (true == m_bFlags[0])
		{
			m_pGameInstance->Play_Sound(L"MOTHER_TAUNT", L"grand_parasiter_vo_spit001.wav", SOUND_ENEMY_SKILL1, 10.f);
			
		}
	}

	if (pActor->Is_Animation_End())
	{
		return new CMother_SpittingLoop;
	}
	
	return nullptr;
}

void CMother_SpittingStart::Release(CMother* pActor)
{
	__super::Release(pActor);

}
