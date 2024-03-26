#include "..\Public\Tank_Run_F.h"

void CTank_Run_F::Initialize(CTank* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CTank>* CTank_Run_F::Update(CTank* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CTank_Run_F::Release(CTank* pActor)
{
	__super::Release(pActor);
}
