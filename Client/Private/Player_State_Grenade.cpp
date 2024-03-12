#include "..\Public\Player_State_Grenade.h"

void CPlayer_State_Grenade::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);
}

CState<CPlayer>* CPlayer_State_Grenade::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CPlayer_State_Grenade::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}

CState<CPlayer>* CPlayer_State_Grenade::Update_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Grenade_State(pActor, fTimeDelta, _iAnimIndex);
}
