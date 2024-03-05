#include "..\Public\BanditHeavy_HitLight_F_03.h"

void CBanditHeavy_HitLight_F_03::Initialize(CBandit_Heavy* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CBandit_Heavy>* CBanditHeavy_HitLight_F_03::Update(CBandit_Heavy* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CBanditHeavy_HitLight_F_03::Release(CBandit_Heavy* pActor)
{
	__super::Release(pActor);
}
