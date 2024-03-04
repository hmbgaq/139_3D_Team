#include "..\Public\BanditHeavy_HitHeavy_B_01.h"

void CBanditHeavy_HitHeavy_B_01::Initialize(CBandit_Heavy* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CBandit_Heavy>* CBanditHeavy_HitHeavy_B_01::Update(CBandit_Heavy* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CBanditHeavy_HitHeavy_B_01::Release(CBandit_Heavy* pActor)
{
	__super::Release(pActor);
}
