#include "VampireCommander_TurnR180.h"
#include "VampireCommander_Idle.h"

void CVampireCommander_TurnR180::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
	pActor->m_bTurn = false;
}

CState<CVampireCommander>* CVampireCommander_TurnR180::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	if (pActor->Is_Inputable_Front(35))
	{
		pActor->Look_At_Target();

		return new CVampireCommander_Idle;
		//return nullptr;
	}
	return nullptr;
}

void CVampireCommander_TurnR180::Release(CVampireCommander* pActor)
{
	__super::Release(pActor);
}
