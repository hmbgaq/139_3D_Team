#include "..\Public\BanditHeavy_Walk_FL.h"

void CBanditHeavy_Walk_FL::Initialize(CBandit_Heavy* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CBandit_Heavy>* CBanditHeavy_Walk_FL::Update(CBandit_Heavy* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CBanditHeavy_Walk_FL::Release(CBandit_Heavy* pActor)
{
	__super::Release(pActor);
}
