#include "..\Public\Player_SlamDown_ManHero.h"

#include "Player_Empowered_Idle.h"

void CPlayer_SlamDown_ManHero::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CPlayer>* CPlayer_SlamDown_ManHero::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_SlamDown_ManHero::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
