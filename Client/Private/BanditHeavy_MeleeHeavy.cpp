#include "..\Public\BanditHeavy_MeleeHeavy.h"

void CBanditHeavy_MeleeHeavy::Initialize(CBandit_Heavy* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CBandit_Heavy>* CBanditHeavy_MeleeHeavy::Update(CBandit_Heavy* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CBanditHeavy_MeleeHeavy::Release(CBandit_Heavy* pActor)
{
	__super::Release(pActor);
}
