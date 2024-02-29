#include "..\Public\VampireTank_KnockFrontLight_F_01.h"

void CVampireTank_KnockFrontLight_F_01::Initialize(CBandit_Heavy* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CBandit_Heavy>* CVampireTank_KnockFrontLight_F_01::Update(CBandit_Heavy* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CVampireTank_KnockFrontLight_F_01::Release(CBandit_Heavy* pActor)
{
	__super::Release(pActor);
}
