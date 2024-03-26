#include "..\Public\Tank_Charge_Fail_L.h"

void CTank_Charge_Fail_L::Initialize(CTank* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CTank>* CTank_Charge_Fail_L::Update(CTank* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CTank_Charge_Fail_L::Release(CTank* pActor)
{
	__super::Release(pActor);
}
