#include "..\Public\Tank_State_Attack_NoShield.h"

void CTank_State_Attack_NoShield::Initialize(CTank* pActor)
{
	__super::Initialize(pActor);
}

CState<CTank>* CTank_State_Attack_NoShield::Update(CTank* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CTank_State_Attack_NoShield::Release(CTank* pActor)
{
	__super::Release(pActor);
}

CState<CTank>* CTank_State_Attack_NoShield::Update_State(CTank* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return NoShield_State(pActor, fTimeDelta, _iAnimIndex);
}
