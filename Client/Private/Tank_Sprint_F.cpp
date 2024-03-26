#include "..\Public\Tank_Sprint_F.h"

void CTank_Sprint_F::Initialize(CTank* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CTank>* CTank_Sprint_F::Update(CTank* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CTank_Sprint_F::Release(CTank* pActor)
{
	__super::Release(pActor);
}
