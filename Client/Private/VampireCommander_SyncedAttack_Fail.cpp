#include "VampireCommander_SyncedAttack_Fail.h"
#include "VampireCommander_SyncedAttack.h"
#include "VampireCommander_Idle.h"

void CVampireCommander_SyncedAttack_Fail::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CVampireCommander>* CVampireCommander_SyncedAttack_Fail::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	//여기서 먼저 충돌을 검사를 하고 충돌에서 잡기 성공판정이면 분기점 Trackposition을 받아서 syncedAttack 으로 전환! syncedAttack에서는 분기점 trackposition부터 스타트
	if (pActor->Is_Animation_End())
	{
		return new CVampireCommander_Idle();
	}
	//else
	//{
	//
	//}

	return nullptr;
}

void CVampireCommander_SyncedAttack_Fail::Release(CVampireCommander* pActor)
{
	__super::Release(pActor);
}
