#include "..\Public\Player_Revolver_Ironsights_ReloadFast.h"
#include "GameInstance.h"

#include "Player_Revolver_WeaponHolster.h"

void CPlayer_Revolver_Ironsights_ReloadFast::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation_Upper(g_iAnimIndex, CModel::ANIM_STATE_STOP);
	pActor->Set_Splitted(true);
}

CState<CPlayer>* CPlayer_Revolver_Ironsights_ReloadFast::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	pActor->Aim_Walk(fTimeDelta);

	if (m_pGameInstance->Mouse_Down(DIM_LB))
	{
		pActor->Activate_ShootingReaction(15.f);
	}

	if (m_pGameInstance->Key_Down(DIK_E))
	{
		pActor->Set_Animation_Upper(g_iAnimIndex, CModel::ANIM_STATE_NORMAL);
	}

	//return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);

	if (pActor->Is_UpperAnimation_End())
	{
		return new CPlayer_Revolver_WeaponHolster();
	}

	return nullptr;
}

void CPlayer_Revolver_Ironsights_ReloadFast::Release(CPlayer* pActor)
{
	__super::Release(pActor);
	//pActor->Set_Splitted(false);
}
