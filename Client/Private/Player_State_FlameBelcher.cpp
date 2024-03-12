#include "..\Public\Player_State_FlameBelcher.h"

void CPlayer_State_FlameBelcher::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);
}

CState<CPlayer>* CPlayer_State_FlameBelcher::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CPlayer_State_FlameBelcher::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}

CState<CPlayer>* CPlayer_State_FlameBelcher::Update_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return FlameBelcher_State(pActor, fTimeDelta, _iAnimIndex);
}
