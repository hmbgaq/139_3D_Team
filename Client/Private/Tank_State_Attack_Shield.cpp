#include "..\Public\Tank_State_Attack_Shield.h"

void CTank_State_Attack_Shield::Initialize(CTank* pActor)
{
	__super::Initialize(pActor);
}

CState<CTank>* CTank_State_Attack_Shield::Update(CTank* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CTank_State_Attack_Shield::Release(CTank* pActor)
{
	__super::Release(pActor);
}

CState<CTank>* CTank_State_Attack_Shield::Update_State(CTank* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Shield_State(pActor, fTimeDelta, _iAnimIndex);
}
