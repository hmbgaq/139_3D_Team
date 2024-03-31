#include "..\Public\Tank_Run_FR.h"

void CTank_Run_FR::Initialize(CTank* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true);

}

CState<CTank>* CTank_Run_FR::Update(CTank* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CTank_Run_FR::Release(CTank* pActor)
{
	__super::Release(pActor);
}
