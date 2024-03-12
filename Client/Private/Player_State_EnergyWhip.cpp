#include "..\Public\Player_State_EnergyWhip.h"

void CPlayer_State_EnergyWhip::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);
}

CState<CPlayer>* CPlayer_State_EnergyWhip::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CPlayer_State_EnergyWhip::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}

CState<CPlayer>* CPlayer_State_EnergyWhip::Update_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return EnergyWhip_State(pActor, fTimeDelta, _iAnimIndex);
}
