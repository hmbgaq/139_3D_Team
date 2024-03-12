#include "..\Public\Player_State_Aim.h"

void CPlayer_State_Aim::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);
}

CState<CPlayer>* CPlayer_State_Aim::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CPlayer_State_Aim::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}

CState<CPlayer>* CPlayer_State_Aim::Update_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Aim_State(pActor, fTimeDelta, _iAnimIndex);
}
