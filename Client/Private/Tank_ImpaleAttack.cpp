#include "..\Public\Tank_ImpaleAttack.h"

void CTank_ImpaleAttack::Initialize(CTank* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CTank>* CTank_ImpaleAttack::Update(CTank* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CTank_ImpaleAttack::Release(CTank* pActor)
{
	__super::Release(pActor);
}
