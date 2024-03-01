#include "..\Public\BanditHeavy_MeleeDynamic_LM.h"

void CBanditHeavy_MeleeDynamic_LM::Initialize(CBandit_Heavy* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CBandit_Heavy>* CBanditHeavy_MeleeDynamic_LM::Update(CBandit_Heavy* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CBanditHeavy_MeleeDynamic_LM::Release(CBandit_Heavy* pActor)
{
	__super::Release(pActor);
}
