#include "..\Public\Player_Empowered_Idle.h"

void CPlayer_Empowered_Idle::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true);

}

CState<CPlayer>* CPlayer_Empowered_Idle::Update(CPlayer* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_Empowered_Idle::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
