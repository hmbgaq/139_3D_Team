#include "..\Public\Player_State_Roll.h"

void CPlayer_State_Roll::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);
}

CState<CPlayer>* CPlayer_State_Roll::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CPlayer_State_Roll::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}

CState<CPlayer>* CPlayer_State_Roll::Update_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Roll_State(pActor, fTimeDelta, _iAnimIndex);
}
