#include "VampireCommander_Ranged2.h"
#include "VampireCommander_Idle.h"
#include "Body_VampireCommander.h"
void CVampireCommander_Ranged2::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

	CWeapon* pWeapon = pActor->Get_Weapon(TEXT("Weapon_hand_L"));

	pWeapon
		->Set_Damage(30)
		->Set_Direction(Direction::Left)
		->Set_Power(Power::Medium)
		->Set_Force(0.f);

	CBody_VampireCommander* pBody = dynamic_cast<CBody_VampireCommander*>(pActor->Get_Body());
	pBody->Set_RenderState(CBody_VampireCommander::RENDER_STATE::ATTACK);
}

CState<CVampireCommander>* CVampireCommander_Ranged2::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	CWeapon* pWeapon = pActor->Get_Weapon(TEXT("Weapon_hand_L"));

	if (m_bFlags[1] == false)
	{
		pActor->m_bLookAt = true;
	}

	if (m_bFlags[2] == false && pActor->Is_Inputable_Front(37))
	{
		m_bFlags[1] = true;
		m_bFlags[2] = true;
		pWeapon->Set_Enable(true);
	}

	if (m_bFlags[3] == false && pActor->Is_Inputable_Front(38))
	{
		m_bFlags[1] = false;
		m_bFlags[3] = true;
		pWeapon->Set_Enable(false);

	}

	if (pActor->Is_Animation_End())
	{
		return new CVampireCommander_Idle();
	}

	return nullptr;
}

void CVampireCommander_Ranged2::Release(CVampireCommander* pActor)
{
	__super::Release(pActor);

	CWeapon* pWeapon = pActor->Get_Weapon(TEXT("Weapon_hand_L"));
	//pWeapon->Set_Enable(false);
}
