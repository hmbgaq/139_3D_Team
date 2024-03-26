#include "..\Public\Tank_State_Run.h"

void CTank_State_Run::Initialize(CTank* pActor)
{
	__super::Initialize(pActor);
}

CState<CTank>* CTank_State_Run::Update(CTank* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CTank_State_Run::Release(CTank* pActor)
{
	__super::Release(pActor);
}

CState<CTank>* CTank_State_Run::Update_State(CTank* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Run_State(pActor, fTimeDelta, _iAnimIndex);
}
