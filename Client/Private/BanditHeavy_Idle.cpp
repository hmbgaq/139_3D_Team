#include "..\Public\BanditHeavy_Idle.h"

void CBanditHeavy_Idle::Initialize(CBandit_Heavy* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true);

}

CState<CBandit_Heavy>* CBanditHeavy_Idle::Update(CBandit_Heavy* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	m_fDelay += fTimeDelta;

	pActor->Look_At_Target_Lerp(fTimeDelta);

	if (0.3f <= m_fDelay)
	{
		return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
	}
	return nullptr;
}

void CBanditHeavy_Idle::Release(CBandit_Heavy* pActor)
{
	__super::Release(pActor);
}
