#include "..\Public\Tank_Run_BR.h"

void CTank_Run_BR::Initialize(CTank* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true);

}

CState<CTank>* CTank_Run_BR::Update(CTank* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CTank_Run_BR::Release(CTank* pActor)
{
	__super::Release(pActor);
}
