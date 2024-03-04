#include "VampireCommander_Leap_Loop.h"
#include "Data_Manager.h"

void CVampireCommander_Leap_Loop::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true);
	CData_Manager::GetInstance()->Get_Player();
}

CState<CVampireCommander>* CVampireCommander_Leap_Loop::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return new CVampireCommander_Leap_Loop();
	}

	return nullptr;
}

void CVampireCommander_Leap_Loop::Release(CVampireCommander* pActor)
{
	__super::Release(pActor);
}
