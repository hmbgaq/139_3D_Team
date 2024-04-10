#include "VampireCommander_Dead.h"
#include "Data_Manager.h"

void CVampireCommander_Dead::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CVampireCommander>* CVampireCommander_Dead::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	//임시 죽음 나중에 디졸브 체크하면서 해야 함 
	if (pActor->Is_Animation_End())
	{
		pActor->m_bCntDead_Active = true;
	}

	return nullptr;
}

void CVampireCommander_Dead::Release(CVampireCommander* pActor)
{
	__super::Release(pActor);
	CData_Manager::GetInstance()->Add_CurEXP(VAMPIRE_COMMANDER_EXP);
}
