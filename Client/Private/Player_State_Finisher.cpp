#include "..\Public\Player_State_Finisher.h"
#include "Collider.h"

void CPlayer_State_Finisher::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);
	pActor->Set_Invincible(true);
	pActor->Get_Body()->Collider_Off();
}

CState<CPlayer>* CPlayer_State_Finisher::Update(CPlayer* pActor, _float fTimeDelta)
{
	//__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CPlayer_State_Finisher::Release(CPlayer* pActor)
{
	__super::Release(pActor);
	pActor->Set_Invincible(false);
	pActor->Get_Body()->Get_Collider()->Set_Enable(true);
}

CState<CPlayer>* CPlayer_State_Finisher::Update_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Finisher_State(pActor, fTimeDelta, _iAnimIndex);
}
