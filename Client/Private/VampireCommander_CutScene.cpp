#include "VampireCommander_CutScene.h"

void CVampireCommander_CutScene::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CVampireCommander>* CVampireCommander_CutScene::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		pActor->Set_Dead(true);
	}

	return nullptr;
}

void CVampireCommander_CutScene::Release(CVampireCommander* pActor)
{
	__super::Release(pActor);
}
