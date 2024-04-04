#include "..\Public\Player_CrouchUnder_End.h"

void CPlayer_CrouchUnder_End::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CPlayer>* CPlayer_CrouchUnder_End::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_CrouchUnder_End::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
