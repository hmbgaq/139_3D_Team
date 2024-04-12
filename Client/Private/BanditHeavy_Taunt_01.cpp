#include "..\Public\BanditHeavy_Taunt_01.h"

void CBanditHeavy_Taunt_01::Initialize(CBandit_Heavy* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

	pActor->Look_At_Target();

}

CState<CBandit_Heavy>* CBanditHeavy_Taunt_01::Update(CBandit_Heavy* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	if (pActor->Is_Animation_End())
	{
		return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
	}
	return nullptr;
}

void CBanditHeavy_Taunt_01::Release(CBandit_Heavy* pActor)
{
	__super::Release(pActor);
}
