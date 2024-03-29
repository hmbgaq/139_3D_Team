#include "..\Public\Tank_State_Attack.h"

void CTank_State_Attack::Initialize(CTank* pActor)
{
	__super::Initialize(pActor);
}

CState<CTank>* CTank_State_Attack::Update(CTank* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CTank_State_Attack::Release(CTank* pActor)
{
	__super::Release(pActor);
}

CState<CTank>* CTank_State_Attack::Update_State(CTank* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Attack_State(pActor, fTimeDelta, _iAnimIndex);
}
