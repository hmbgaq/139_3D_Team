#include "..\Public\Tank_State_Attack.h"

void CTank_State_Attack::Initialize(CTank* pActor)
{
	__super::Initialize(pActor);
	pActor->Set_MonsterAttackState(true);
}

CState<CTank>* CTank_State_Attack::Update(CTank* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);
	pActor->Set_MonsterAttackState(true);

	return nullptr;
}

void CTank_State_Attack::Release(CTank* pActor)
{
	__super::Release(pActor);
	pActor->Set_MonsterAttackState(false);
}

CState<CTank>* CTank_State_Attack::Update_State(CTank* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Attack_State(pActor, fTimeDelta, _iAnimIndex);
}
