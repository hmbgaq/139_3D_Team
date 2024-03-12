#include "..\Public\Player_State_Shotgun.h"

void CPlayer_State_Shotgun::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);
}

CState<CPlayer>* CPlayer_State_Shotgun::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CPlayer_State_Shotgun::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}

CState<CPlayer>* CPlayer_State_Shotgun::Update_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Shotgun_State(pActor, fTimeDelta, _iAnimIndex);
}
