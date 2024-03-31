#include "..\Public\Tank_WhipHold.h"

void CTank_WhipHold::Initialize(CTank* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CTank>* CTank_WhipHold::Update(CTank* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CTank_WhipHold::Release(CTank* pActor)
{
	__super::Release(pActor);
}
