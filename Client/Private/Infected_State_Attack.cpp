#include "stdafx.h"
#include "Infected_Walk_B.h"
#include "Infected_State_Attack.h"

void CInfected_State_Attack::Initialize(CInfected* pActor)
{
	m_eType = pActor->Get_Info().eType;
	m_pWeapon = pActor->Get_Weapon(TEXT("Weapon_Punch")); /* 이름 이걸로 공통사항으로 밀어야함 */
	Safe_AddRef(m_pWeapon);

	__super::Initialize(pActor);
}

CState<CInfected>* CInfected_State_Attack::Update(CInfected* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CInfected_State_Attack::Release(CInfected* pActor)
{
	if(nullptr != m_pWeapon)
		Safe_Release(m_pWeapon);

	__super::Release(pActor);
}

CState<CInfected>* CInfected_State_Attack::Update_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (pActor->Is_Animation_End()) /* 공격이 끝나면 무조건 뒷걸음질침 */
	{
		//cout << "공격이후 뒷걸음질 " << endl;
		return new CInfected_Walk_B();
	}
	return nullptr;
}
