#include "..\Public\Player_Crossbow_WeaponUnholster_Fast.h"
#include "GameInstance.h"

void CPlayer_Crossbow_WeaponUnholster_Fast::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation_Upper(g_iAnimIndex, CModel::ANIM_STATE_LOOP);
	pActor->Set_Splitted(true);
}

CState<CPlayer>* CPlayer_Crossbow_WeaponUnholster_Fast::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	pActor->Aim_Walk(fTimeDelta);

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_Crossbow_WeaponUnholster_Fast::Release(CPlayer* pActor)
{
	__super::Release(pActor);
	pActor->Set_Splitted(false);
}
