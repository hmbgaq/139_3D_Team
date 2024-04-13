#include "Son_HideStart.h"
#include "Son_HideLoop.h"
#include "GameInstance.h"
void CSon_HideStart::Initialize(CSon* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
	m_pGameInstance->Play_Sound(L"SON_DEAD", L"VampireSpitter_VO_Spit_03.wav", SOUND_ENEMY_SPAWN, 7.f);
}

CState<CSon>* CSon_HideStart::Update(CSon* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return new CSon_HideLoop;
	}

	return nullptr;
}

void CSon_HideStart::Release(CSon* pActor)
{
	__super::Release(pActor);
}
