#include "..\Public\Tank_Stun_Start.h"
#include "Tank_Stun_Loop.h"

void CTank_Stun_Start::Initialize(CTank* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CTank>* CTank_Stun_Start::Update(CTank* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return new CTank_Stun_Loop();
	}

	return nullptr;
	//return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CTank_Stun_Start::Release(CTank* pActor)
{
	__super::Release(pActor);
}
