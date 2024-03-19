#include "VampireCommander_TurnL180.h"
#include "VampireCommander_Idle.h"
#include "Transform.h"
void CVampireCommander_TurnL180::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
	pActor->m_bTurn = false;
}

CState<CVampireCommander>* CVampireCommander_TurnL180::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	//_float fAngle = pActor->Target_Contained_Angle(pActor->Get_Transform()->Get_Look(), pActor->Get_Target()->Get_Transform()->Get_Pos());
	//
	//if (0 <= fAngle && fAngle <= 45)
	//	pActor->Look_At_Target_Lerp(fTimeDelta * 2);
	//else if (-45 <= fAngle && fAngle < 0)
	//	pActor->Look_At_Target_Lerp(fTimeDelta * 2);

	if (pActor->Is_Inputable_Front(35))
	{
		pActor->Look_At_Target();

		return new CVampireCommander_Idle;
		//return nullptr;
	}
	return nullptr;
}

void CVampireCommander_TurnL180::Release(CVampireCommander* pActor)
{
	__super::Release(pActor);
}
