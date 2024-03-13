#include "..\Public\Player_State_Death.h"

void CPlayer_State_Death::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);
	pActor->Set_Splitted(false);
}

CState<CPlayer>* CPlayer_State_Death::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CPlayer_State_Death::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}

CState<CPlayer>* CPlayer_State_Death::Update_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Death_State(pActor, fTimeDelta, _iAnimIndex);
}
