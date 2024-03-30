#include "..\Public\Tank_Walk_BR.h"

void CTank_Walk_BR::Initialize(CTank* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true);

}

CState<CTank>* CTank_Walk_BR::Update(CTank* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CTank_Walk_BR::Release(CTank* pActor)
{
	__super::Release(pActor);
}
