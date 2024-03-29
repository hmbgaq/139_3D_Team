#include "..\Public\Player_IdleLoop.h"

void CPlayer_IdleLoop::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);
	pActor->Set_Weapons_Enable_False();

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true);

}

CState<CPlayer>* CPlayer_IdleLoop::Update(CPlayer* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_IdleLoop::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
