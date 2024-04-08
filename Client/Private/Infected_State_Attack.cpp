#include "stdafx.h"
#include "Infected_Walk_B.h"
#include "Infected_State_Attack.h"
#include "Body_Infected.h"
#include "Weapon_Infected.h"

void CInfected_State_Attack::Initialize(CInfected* pActor)
{
	m_eType = pActor->Get_Info().eType;
	m_pWeapon = pActor->Get_Weapon(TEXT("Weapon_Punch")); /* 이름 이걸로 공통사항으로 밀어야함 */
	Safe_AddRef(m_pWeapon);

	AttackState_Setting(pActor);

	dynamic_cast<CWeapon_Infected*>(m_pWeapon)->Play_Trail(true);	// 트레일 켜기

	__super::Initialize(pActor);
}

CState<CInfected>* CInfected_State_Attack::Update(CInfected* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CInfected_State_Attack::Release(CInfected* pActor)
{
	if (nullptr != m_pWeapon) 
	{
		dynamic_cast<CWeapon_Infected*>(m_pWeapon)->Play_Trail(false); // 트레일 끄기
		m_pWeapon->Set_Enable(false);

		Safe_Release(m_pWeapon);
	}
		
	__super::Release(pActor);
}

void CInfected_State_Attack::AttackState_Setting(CInfected* pActor)
{
	CBody_Infected* pBody = dynamic_cast<CBody_Infected*>(pActor->Get_Body());
	pBody->Set_RenderState(CBody_Infected::RENDER_STATE::ATTACK);
}

CState<CInfected>* CInfected_State_Attack::Update_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return new CInfected_Walk_B();
}
