#include "..\Public\Player_Empowered_Winchester_Stop.h"
#include "GameInstance.h"
#include "Player_Winchester_LowerHolster.h"

void CPlayer_Empowered_Winchester_Stop::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation_Upper(g_iAnimIndex, CModel::ANIM_STATE_NORMAL);
	pActor->Set_Splitted(true);
}

CState<CPlayer>* CPlayer_Empowered_Winchester_Stop::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	pActor->Aim_Walk(fTimeDelta);

	//if (pActor->Is_UpperAnimation_End())
	//{
	//	return new CPlayer_Winchester_LowerHolster();
	//}

	//return nullptr;

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_Empowered_Winchester_Stop::Release(CPlayer* pActor)
{
	__super::Release(pActor);
	pActor->Set_Splitted(false);
}
