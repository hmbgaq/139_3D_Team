#include "..\Public\Player_State_Walk.h"

void CPlayer_State_Walk::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);
}

CState<CPlayer>* CPlayer_State_Walk::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CPlayer_State_Walk::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}

CState<CPlayer>* CPlayer_State_Walk::Update_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Walk_State(pActor, fTimeDelta, _iAnimIndex);
}
