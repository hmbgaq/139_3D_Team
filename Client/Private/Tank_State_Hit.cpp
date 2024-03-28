#include "..\Public\Tank_State_Hit.h"

void CTank_State_Hit::Initialize(CTank* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_ShieldBroken();
}

CState<CTank>* CTank_State_Hit::Update(CTank* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CTank_State_Hit::Release(CTank* pActor)
{
	__super::Release(pActor);

}

CState<CTank>* CTank_State_Hit::Update_State(CTank* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Hit_State(pActor, fTimeDelta, _iAnimIndex);
}
