#include "stdafx.h"
#include "GameInstance.h"
#include "Infected_Sprint_F_01.h"
#include "Infected_Electrocute_Loop.h"

void CInfected_Sprint_F_01::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true, false); // 루트애니메이션x

	pActor->Get_Transform()->Set_Speed(4.5f);	/* 움직이는 속도 제어 */
	pActor->Set_StiffnessRate_Upper(3.f);		/* 애니메이션 재생속도 제어 */
}

CState<CInfected>* CInfected_Sprint_F_01::Update(CInfected* pActor, _float fTimeDelta)
{
	pActor->Look_At_Target();
	pActor->Go_Straight(fTimeDelta);

	_float fDist = pActor->Calc_Distance(m_pGameInstance->Get_Player());

	if (fDist <= pActor->Get_Info().fAttack_Distance ) // <=2
	{
		_int iRemain = pActor->Get_Info().RandomNumber % 2;
		
		pActor->Get_Transform()->Set_Speed(10.f);	/* 움직이는 속도 제어 */
		pActor->Set_StiffnessRate_Upper(1.f);		/* 애니메이션 재생속도 제어 */
		pActor->Set_MonsterAttackState(true);		/* 공격상태 설정 */
		
		return new CInfected_Electrocute_Loop();
	}

	return nullptr;
}

void CInfected_Sprint_F_01::Release(CInfected* pActor)
{
	__super::Release(pActor);
}
