#include "..\Public\Player_State_Interaction.h"
#include "Navigation.h"
#include "GameInstance.h"

void CPlayer_State_Interaction::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);
	pActor->Set_Interection(true);
	//pActor->Get_Navigation()->Set_InteractMoveMode(true);
}

CState<CPlayer>* CPlayer_State_Interaction::Update(CPlayer* pActor, _float fTimeDelta)
{
	//__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CPlayer_State_Interaction::Release(CPlayer* pActor)
{
	__super::Release(pActor);
	pActor->Set_UseGravity(true);
	pActor->Reset_RootMoveRate();
	pActor->Set_Interection(false);

	CNavigation* pNavigation = pActor->Get_Navigation();

	

	//pNavigation->Set_InteractMoveMode(false);

}

CState<CPlayer>* CPlayer_State_Interaction::Update_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Interaction_State(pActor, fTimeDelta, _iAnimIndex);
}
