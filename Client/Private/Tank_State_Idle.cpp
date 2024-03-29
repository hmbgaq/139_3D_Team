#include "..\Public\Tank_State_Idle.h"
#include "SMath.h"


void CTank_State_Idle::Initialize(CTank* pActor)
{
	__super::Initialize(pActor);
}

CState<CTank>* CTank_State_Idle::Update(CTank* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CTank_State_Idle::Release(CTank* pActor)
{
	__super::Release(pActor);
}

CState<CTank>* CTank_State_Idle::Update_State(CTank* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Normal_State(pActor, fTimeDelta, _iAnimIndex);
}
