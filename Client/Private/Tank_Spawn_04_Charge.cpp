#include "..\Public\Tank_Spawn_04_Charge.h"

void CTank_Spawn_04_Charge::Initialize(CTank* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CTank>* CTank_Spawn_04_Charge::Update(CTank* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CTank_Spawn_04_Charge::Release(CTank* pActor)
{
	__super::Release(pActor);
}
