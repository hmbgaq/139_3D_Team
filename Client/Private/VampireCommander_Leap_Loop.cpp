#include "VampireCommander_Leap_Loop.h"
#include "Data_Manager.h"
#include "VampireCommander_Leap_Stop.h"

void CVampireCommander_Leap_Loop::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true);
	CData_Manager::GetInstance()->Get_Player();
}

CState<CVampireCommander>* CVampireCommander_Leap_Loop::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	//if (0.5f < pActor->Calc_Distance())
	//{
	//	pActor->Go_Straight(fTimeDelta);
	//}
	//else
	//{
	//	return new CVampireCommander_Leap_Stop();
	//}
	//
	return nullptr;
}

void CVampireCommander_Leap_Loop::Release(CVampireCommander* pActor)
{
	__super::Release(pActor);
}
