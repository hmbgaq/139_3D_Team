#include "..\Public\Tank_Stun_Start.h"

void CTank_Stun_Start::Initialize(CTank* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CTank>* CTank_Stun_Start::Update(CTank* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CTank_Stun_Start::Release(CTank* pActor)
{
	__super::Release(pActor);
}
