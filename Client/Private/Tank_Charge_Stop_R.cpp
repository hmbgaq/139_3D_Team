#include "..\Public\Tank_Charge_Stop_R.h"

void CTank_Charge_Stop_R::Initialize(CTank* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CTank>* CTank_Charge_Stop_R::Update(CTank* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CTank_Charge_Stop_R::Release(CTank* pActor)
{
	__super::Release(pActor);
}
