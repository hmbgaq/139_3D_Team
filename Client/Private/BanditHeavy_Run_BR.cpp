#include "..\Public\BanditHeavy_Run_BR.h"

void CBanditHeavy_Run_BR::Initialize(CBandit_Heavy* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CBandit_Heavy>* CBanditHeavy_Run_BR::Update(CBandit_Heavy* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CBanditHeavy_Run_BR::Release(CBandit_Heavy* pActor)
{
	__super::Release(pActor);
}
