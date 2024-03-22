#include "..\Public\Player_Revolver_WeaponUnholster.h"
#include "GameInstance.h"

//#include "Player_Bandit_Special_01.h"
#include "Player_Revolver_WeaponHolster.h"
#include "Player_Revolver_Ironsights_ReloadFast.h"
//#include "Player_William_RevolverFanningStart_02.h"

void CPlayer_Revolver_WeaponUnholster::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation_Upper(g_iAnimIndex, CModel::ANIM_STATE_NORMAL);
	pActor->Set_Splitted(true);

	//pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL);
}

CState<CPlayer>* CPlayer_Revolver_WeaponUnholster::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	pActor->Aim_Walk(fTimeDelta);

	if (pActor->Is_UpperAnimation_End())
	{
		return new CPlayer_Revolver_Ironsights_ReloadFast();
		//return new CPlayer_Revolver_WeaponHolster();
	}

	return nullptr;

	//return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_Revolver_WeaponUnholster::Release(CPlayer* pActor)
{
	__super::Release(pActor);
	//pActor->Set_Splitted(false);
}
