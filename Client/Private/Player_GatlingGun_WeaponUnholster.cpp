#include "..\Public\Player_GatlingGun_WeaponUnholster.h"
#include "GameInstance.h"

void CPlayer_GatlingGun_WeaponUnholster::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation_Upper(g_iAnimIndex, CModel::ANIM_STATE_LOOP);
	pActor->Set_Splitted(true);
}

CState<CPlayer>* CPlayer_GatlingGun_WeaponUnholster::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	pActor->Aim_Walk(fTimeDelta);

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_GatlingGun_WeaponUnholster::Release(CPlayer* pActor)
{
	__super::Release(pActor);
	pActor->Set_Splitted(false);
}
