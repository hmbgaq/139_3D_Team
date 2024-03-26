#include "..\Public\Tank_Run_BL.h"

void CTank_Run_BL::Initialize(CTank* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true);

}

CState<CTank>* CTank_Run_BL::Update(CTank* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CTank_Run_BL::Release(CTank* pActor)
{
	__super::Release(pActor);
}
