#include "Son_Idle.h"

void CSon_Idle::Initialize(CSon* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true);
	pActor->m_bLookAt = true;

	pActor->Get_Weapon(L"Weapon_head")->Set_Enable_Collisions(false);

}

CState<CSon>* CSon_Idle::Update(CSon* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CSon_Idle::Release(CSon* pActor)
{
	__super::Release(pActor);
}
