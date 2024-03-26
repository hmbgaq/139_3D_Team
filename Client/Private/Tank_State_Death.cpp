#include "..\Public\Tank_State_Death.h"

void CTank_State_Death::Initialize(CTank* pActor)
{
	__super::Initialize(pActor);
}

CState<CTank>* CTank_State_Death::Update(CTank* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CTank_State_Death::Release(CTank* pActor)
{
	__super::Release(pActor);
}

CState<CTank>* CTank_State_Death::Update_State(CTank* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Death_State(pActor, fTimeDelta, _iAnimIndex);
}
