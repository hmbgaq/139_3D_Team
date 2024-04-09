#include "Son_ShortAttack.h"
#include "Son_Idle.h"

void CSon_ShortAttack::Initialize(CSon* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

	pActor->Get_Weapon(L"Weapon_head")->Set_Enable_Collisions(true);
	pActor->Set_MonsterAttackState(true);
}

CState<CSon>* CSon_ShortAttack::Update(CSon* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return new CSon_Idle;
	}

	return nullptr;
}

void CSon_ShortAttack::Release(CSon* pActor)
{
	__super::Release(pActor);
}
