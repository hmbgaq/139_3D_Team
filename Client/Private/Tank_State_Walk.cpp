#include "..\Public\Tank_State_Walk.h"

void CTank_State_Walk::Initialize(CTank* pActor)
{
	__super::Initialize(pActor);
}

CState<CTank>* CTank_State_Walk::Update(CTank* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CTank_State_Walk::Release(CTank* pActor)
{
	__super::Release(pActor);
}

CState<CTank>* CTank_State_Walk::Update_State(CTank* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Walk_State(pActor, fTimeDelta, _iAnimIndex);
}
