#include "..\Public\Player_Revolver_Hip_ReloadFast_Alt03.h"
#include "GameInstance.h"

#include "Player_IdleLoop.h"
#include "Player_Revolver_WeaponHolster.h"

void CPlayer_Revolver_Hip_ReloadFast_Alt03::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation_Upper(g_iAnimIndex, CModel::ANIM_STATE_NORMAL);
	pActor->Set_Splitted(true);

	//pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL);
}

CState<CPlayer>* CPlayer_Revolver_Hip_ReloadFast_Alt03::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	pActor->Aim_Walk(fTimeDelta);

	if (pActor->Is_UpperAnimation_End())
	{
		return Normal(pActor, fTimeDelta, g_iAnimIndex);
		//return new CPlayer_IdleLoop();
		//return new CPlayer_Revolver_WeaponHolster();
	}

	return nullptr;
	//return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);


	//if (pActor->Is_Animation_End())
	//{
	//	return new CPlayer_IdleLoop();
	//	//return new CPlayer_Revolver_WeaponHolster();
	//}

	//return nullptr;
}

void CPlayer_Revolver_Hip_ReloadFast_Alt03::Release(CPlayer* pActor)
{
	__super::Release(pActor);
	pActor->Set_Splitted(false);
}
