#include "..\Public\Tank_Walk_F.h"

void CTank_Walk_F::Initialize(CTank* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CTank>* CTank_Walk_F::Update(CTank* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CTank_Walk_F::Release(CTank* pActor)
{
	__super::Release(pActor);
}
