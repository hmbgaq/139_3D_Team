#include "..\Public\Player_State_Finisher.h"

void CPlayer_State_Finisher::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);
}

CState<CPlayer>* CPlayer_State_Finisher::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CPlayer_State_Finisher::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}

CState<CPlayer>* CPlayer_State_Finisher::Update_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Finisher_State(pActor, fTimeDelta, _iAnimIndex);
}
