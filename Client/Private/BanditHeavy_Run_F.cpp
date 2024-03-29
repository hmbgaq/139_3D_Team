#include "..\Public\BanditHeavy_Run_F.h"

void CBanditHeavy_Run_F::Initialize(CBandit_Heavy* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true, false);

}

CState<CBandit_Heavy>* CBanditHeavy_Run_F::Update(CBandit_Heavy* pActor, _float fTimeDelta)
{
	pActor->Go_Straight(fTimeDelta);

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CBanditHeavy_Run_F::Release(CBandit_Heavy* pActor)
{
	__super::Release(pActor);
}
