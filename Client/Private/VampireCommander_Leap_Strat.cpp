#include "VampireCommander_Leap_Strat.h"
#include "VampireCommander_Leap_Loop.h"

void CVampireCommander_Leap_Strat::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CVampireCommander>* CVampireCommander_Leap_Strat::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return new CVampireCommander_Leap_Loop();
	}

	return nullptr;
}

void CVampireCommander_Leap_Strat::Release(CVampireCommander* pActor)
{
	__super::Release(pActor);
}
