#include "..\Public\BanditHeavy_DeathLight_F_01.h"

void CBanditHeavy_DeathLight_F_01::Initialize(CBandit_Heavy* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CBandit_Heavy>* CBanditHeavy_DeathLight_F_01::Update(CBandit_Heavy* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CBanditHeavy_DeathLight_F_01::Release(CBandit_Heavy* pActor)
{
	__super::Release(pActor);
}
