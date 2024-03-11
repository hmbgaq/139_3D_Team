#include "..\Public\Player_State_Gatling.h"

void CPlayer_State_Gatling::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);
}

CState<CPlayer>* CPlayer_State_Gatling::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CPlayer_State_Gatling::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}

CState<CPlayer>* CPlayer_State_Gatling::Update_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Gatling_State(pActor, fTimeDelta, _iAnimIndex);
}
