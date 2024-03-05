#include "VampireCommander_State_CutScene.h"

void CVampireCommander_State_CutScene::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);
}

CState<CVampireCommander>* CVampireCommander_State_CutScene::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CVampireCommander_State_CutScene::Release(CVampireCommander* pActor)
{
	__super::Release(pActor);
}

CState<CVampireCommander>* CVampireCommander_State_CutScene::Update_State(CVampireCommander* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Dead_State(pActor, fTimeDelta, _iAnimIndex);
}
