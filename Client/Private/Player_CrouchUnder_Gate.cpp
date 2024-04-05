#include "..\Public\Player_CrouchUnder_Gate.h"

void CPlayer_CrouchUnder_Gate::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CPlayer>* CPlayer_CrouchUnder_Gate::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_CrouchUnder_Gate::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
