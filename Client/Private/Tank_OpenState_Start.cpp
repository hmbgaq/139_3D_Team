#include "..\Public\Tank_OpenState_Start.h"

void CTank_OpenState_Start::Initialize(CTank* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CTank>* CTank_OpenState_Start::Update(CTank* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CTank_OpenState_Start::Release(CTank* pActor)
{
	__super::Release(pActor);
}
