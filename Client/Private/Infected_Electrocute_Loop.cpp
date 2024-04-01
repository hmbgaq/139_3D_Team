#include "Infected_Electrocute_Loop.h"
#include "Infected_Idle.h"
#include "Body_Infected_D.h"
#include "Weapon_Infected_D.h"

void CInfected_Electrocute_Loop::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);

	if (CInfected::INFECTED_TYPE::INFECTED_WASTER == pActor->Get_Info().Get_Type())
		pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true, false);
	else
		pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true, false);

	pActor->Set_MonsterAttackState(true);
}

CState<CInfected>* CInfected_Electrocute_Loop::Update(CInfected* pActor, _float fTimeDelta)
{
	if (CInfected::INFECTED_TYPE::INFECTED_WASTER == pActor->Get_Info().Get_Type())
	{
		// 자폭하러 옴 - 애니메이션 기준으로 터짐 || 죽어서 옴 
		if (pActor->Is_Animation_End() || 0 >= pActor->Get_CurHP()) 
		{
			CBody_Infected_D* pBody = dynamic_cast<CBody_Infected_D*>(pActor->Get_Body());
			pBody->Set_DeadState(false); // 렌더링 비활성화 
			pBody->Collider_Off(); // 바디콜라이더 비활성화 

			//CWeapon_Infected_D* m_pWeapon = dynamic_cast<CWeapon_Infected_D*>(pActor->Get_Weapon(TEXT("Weapon_Bomb"))); /* 이름 이걸로 공통사항으로 밀어야함 */
			//m_pWeapon->Set_Enable(true);		// 무기콜라이더 활성화 = 자폭 콜라이더 
			pActor->Get_Weapon(TEXT("Weapon_Bomb"))->Set_Enable(true);
			
			__super::Bomb_State(pActor, fTimeDelta, g_iAnimIndex);
		}
	}
	else
	{
		// 다른몬스터는 감전이므로 감전시간 
		if (false == pActor->Is_ElectrocuteTime())
		{
			return new CInfected_Idle();
		}
	}
	return nullptr;
}

void CInfected_Electrocute_Loop::Release(CInfected* pActor)
{
	__super::Release(pActor);
}
