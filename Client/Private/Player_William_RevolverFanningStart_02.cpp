#include "..\Public\Player_William_RevolverFanningStart_02.h"
#include "GameInstance.h"

#include "Player_William_RevolverFanningLoop_02.h"

void CPlayer_William_RevolverFanningStart_02::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	//pActor->Set_Animation_Upper(g_iAnimIndex, CModel::ANIM_STATE_NORMAL);
	//pActor->Set_Splitted(true);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CPlayer>* CPlayer_William_RevolverFanningStart_02::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	//pActor->Aim_Walk(fTimeDelta);

	//return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);

	if (pActor->Is_Animation_End())
	{
		return new CPlayer_William_RevolverFanningLoop_02();
	}

	return nullptr;

}

void CPlayer_William_RevolverFanningStart_02::Release(CPlayer* pActor)
{
	__super::Release(pActor);
	//pActor->Set_Splitted(false);
}
