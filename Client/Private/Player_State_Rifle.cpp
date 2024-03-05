#include "..\Public\Player_State_Rifle.h"

void CPlayer_State_Rifle::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);
}

CState<CPlayer>* CPlayer_State_Rifle::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CPlayer_State_Rifle::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}

CState<CPlayer>* CPlayer_State_Rifle::Update_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Rifle_State(pActor, fTimeDelta, _iAnimIndex);
}
