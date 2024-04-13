#include "Mother_VomitStart.h"
#include "Mother_VomitLoop.h"

#include "Effect_Manager.h"
#include "Effect.h"

void CMother_VomitStart::Initialize(CMother* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);


	EFFECT_MANAGER->Play_Effect("Parasiter/Mother_Breath/", "Mother_Breath_Ready_01.json", pActor, true, "Jaws_Center");
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
