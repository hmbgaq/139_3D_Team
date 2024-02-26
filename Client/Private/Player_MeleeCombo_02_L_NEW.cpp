#include "..\Public\Player_MeleeCombo_02_L_NEW.h"

#include "Player_Empowered_Idle.h"

void CPlayer_MeleeCombo_02_L_NEW::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CPlayer>* CPlayer_MeleeCombo_02_L_NEW::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_MeleeCombo_02_L_NEW::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
