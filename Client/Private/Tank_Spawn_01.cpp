#include "..\Public\Tank_Spawn_01.h"

void CTank_Spawn_01::Initialize(CTank* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CTank>* CTank_Spawn_01::Update(CTank* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CTank_Spawn_01::Release(CTank* pActor)
{
	__super::Release(pActor);
}
