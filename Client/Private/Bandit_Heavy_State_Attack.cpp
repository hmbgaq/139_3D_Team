#include "..\Public\Bandit_Heavy_State_Attack.h"

void CBandit_Heavy_State_Attack::Initialize(CBandit_Heavy* pActor)
{
	__super::Initialize(pActor);
	pActor->Look_At_Target();
}

CState<CBandit_Heavy>* CBandit_Heavy_State_Attack::Update(CBandit_Heavy* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CBandit_Heavy_State_Attack::Release(CBandit_Heavy* pActor)
{
	__super::Release(pActor);
	pActor->Set_Weapon_Collisions_Enable(BANDIT_HEAVY_WEAPON, false);
}

CState<CBandit_Heavy>* CBandit_Heavy_State_Attack::Update_State(CBandit_Heavy* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Attack_State(pActor, fTimeDelta, _iAnimIndex);
}
