#include "..\Public\Tank_GroundSlam_AoE_Stomp_01.h"

void CTank_GroundSlam_AoE_Stomp_01::Initialize(CTank* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CTank>* CTank_GroundSlam_AoE_Stomp_01::Update(CTank* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return Idle(pActor, fTimeDelta, g_iAnimIndex);
	}

	return nullptr;

	//return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CTank_GroundSlam_AoE_Stomp_01::Release(CTank* pActor)
{
	__super::Release(pActor);
}
