#include "..\Public\Tank_State_Spawn.h"

void CTank_State_Spawn::Initialize(CTank* pActor)
{
	__super::Initialize(pActor);
}

CState<CTank>* CTank_State_Spawn::Update(CTank* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CTank_State_Spawn::Release(CTank* pActor)
{
	__super::Release(pActor);
}

CState<CTank>* CTank_State_Spawn::Update_State(CTank* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Spawn_State(pActor, fTimeDelta, _iAnimIndex);
}
