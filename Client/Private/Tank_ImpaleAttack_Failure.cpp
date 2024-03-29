#include "..\Public\Tank_ImpaleAttack_Failure.h"

void CTank_ImpaleAttack_Failure::Initialize(CTank* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CTank>* CTank_ImpaleAttack_Failure::Update(CTank* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CTank_ImpaleAttack_Failure::Release(CTank* pActor)
{
	__super::Release(pActor);
}
