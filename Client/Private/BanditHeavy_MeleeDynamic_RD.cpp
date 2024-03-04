#include "..\Public\BanditHeavy_MeleeDynamic_RD.h"

void CBanditHeavy_MeleeDynamic_RD::Initialize(CBandit_Heavy* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CBandit_Heavy>* CBanditHeavy_MeleeDynamic_RD::Update(CBandit_Heavy* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CBanditHeavy_MeleeDynamic_RD::Release(CBandit_Heavy* pActor)
{
	__super::Release(pActor);
}
