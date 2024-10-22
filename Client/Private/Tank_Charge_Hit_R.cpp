#include "..\Public\Tank_Charge_Hit_R.h"

void CTank_Charge_Hit_R::Initialize(CTank* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
	pActor->Look_At_Target();
}

CState<CTank>* CTank_Charge_Hit_R::Update(CTank* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return Idle(pActor, fTimeDelta, g_iAnimIndex);
	}

	return nullptr;

	//return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CTank_Charge_Hit_R::Release(CTank* pActor)
{
	__super::Release(pActor);
}
