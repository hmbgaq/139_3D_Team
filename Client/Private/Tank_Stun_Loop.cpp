#include "..\Public\Tank_Stun_Loop.h"

void CTank_Stun_Loop::Initialize(CTank* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true);

}

CState<CTank>* CTank_Stun_Loop::Update(CTank* pActor, _float fTimeDelta)
{
	return nullptr;

	//return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CTank_Stun_Loop::Release(CTank* pActor)
{
	__super::Release(pActor);
}
