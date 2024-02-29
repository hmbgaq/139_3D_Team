#include "..\Public\BanditHeavy_Run_B.h"

void CBanditHeavy_Run_B::Initialize(CBandit_Heavy* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CBandit_Heavy>* CBanditHeavy_Run_B::Update(CBandit_Heavy* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CBanditHeavy_Run_B::Release(CBandit_Heavy* pActor)
{
	__super::Release(pActor);
}
