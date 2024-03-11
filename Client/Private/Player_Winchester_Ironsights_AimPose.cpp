#include "..\Public\Player_Winchester_Ironsights_AimPose.h"
#include "GameInstance.h"
#include "Player_Empowered_Winchester_IdleFire.h"
#include "Player_Empowered_Winchester_Stop.h"

void CPlayer_Winchester_Ironsights_AimPose::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation_Upper(g_iAnimIndex, CModel::ANIM_STATE_NORMAL);
	pActor->Set_Splitted(true);
}

CState<CPlayer>* CPlayer_Winchester_Ironsights_AimPose::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	pActor->Aim_Walk(fTimeDelta);

	//if (false == m_pGameInstance->Key_Down(DIM_RB))
	//{
	//	return new CPlayer_Empowered_Winchester_Stop();
	//}

	//if (m_pGameInstance->Key_Down(DIM_LB))
	//{
	//	return new CPlayer_Empowered_Winchester_IdleFire();
	//}

	//return nullptr;

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_Winchester_Ironsights_AimPose::Release(CPlayer* pActor)
{
	__super::Release(pActor);
	pActor->Set_Splitted(false);
}
