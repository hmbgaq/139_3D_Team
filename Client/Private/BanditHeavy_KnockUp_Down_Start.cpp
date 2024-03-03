#include "..\Public\BanditHeavy_KnockUp_Down_Start.h"

void CBanditHeavy_KnockUp_Down_Start::Initialize(CBandit_Heavy* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CBandit_Heavy>* CBanditHeavy_KnockUp_Down_Start::Update(CBandit_Heavy* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CBanditHeavy_KnockUp_Down_Start::Release(CBandit_Heavy* pActor)
{
	__super::Release(pActor);
}
