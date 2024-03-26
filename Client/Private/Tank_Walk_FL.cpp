#include "..\Public\Tank_Walk_FL.h"

void CTank_Walk_FL::Initialize(CTank* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true);

}

CState<CTank>* CTank_Walk_FL::Update(CTank* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CTank_Walk_FL::Release(CTank* pActor)
{
	__super::Release(pActor);
}
