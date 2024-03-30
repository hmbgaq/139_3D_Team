#include "..\Public\Tank_Stun_Get_Up.h"

void CTank_Stun_Get_Up::Initialize(CTank* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CTank>* CTank_Stun_Get_Up::Update(CTank* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CTank_Stun_Get_Up::Release(CTank* pActor)
{
	__super::Release(pActor);
}
