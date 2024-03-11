#include "..\Public\Player_State_Crossbow.h"

void CPlayer_State_Crossbow::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);
}

CState<CPlayer>* CPlayer_State_Crossbow::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CPlayer_State_Crossbow::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}

CState<CPlayer>* CPlayer_State_Crossbow::Update_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Crossbow_State(pActor, fTimeDelta, _iAnimIndex);
}
