#include "..\Public\Tank_Walk_FR.h"

void CTank_Walk_FR::Initialize(CTank* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CTank>* CTank_Walk_FR::Update(CTank* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CTank_Walk_FR::Release(CTank* pActor)
{
	__super::Release(pActor);
}
