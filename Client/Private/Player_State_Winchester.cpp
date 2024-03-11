#include "..\Public\Player_State_Winchester.h"

void CPlayer_State_Winchester::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);
}

CState<CPlayer>* CPlayer_State_Winchester::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CPlayer_State_Winchester::Release(CPlayer* pActor)
{
	__super::Release(pActor);
	pActor->Set_StiffnessRate_Upper(1.f);
}

CState<CPlayer>* CPlayer_State_Winchester::Update_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Winchester_State(pActor, fTimeDelta, _iAnimIndex);
}
