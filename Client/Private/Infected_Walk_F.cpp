	#include "stdafx.h"
#include "Transform.h"
#include "GameInstance.h"
#include "Infected_Walk_F.h"

void CInfected_Walk_F::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true, false);
}

CState<CInfected>* CInfected_Walk_F::Update(CInfected* pActor, _float fTimeDelta)
{
	pActor->Look_At_Target();
	_float fDist = pActor->Calc_Distance(m_pGameInstance->Get_Player());
	_float WalkDistance = pActor->Get_Info().fWalk_Distance;	 // 10.f
	_float AttackDistance = pActor->Get_Info().fAttack_Distance; // 3.5f

	// 0 ~ Attack ~ Walk  
	if (fDist > WalkDistance) // 거리 > 10 
	{
		return __super::PreRun_State(pActor, fTimeDelta, g_iAnimIndex);
	}
	else if ( fDist < AttackDistance) // 거리 < 3.5 
	{
		return __super::PreAttack_State(pActor, fTimeDelta, g_iAnimIndex);
	}

	return nullptr;
}

void CInfected_Walk_F::Release(CInfected* pActor)
{
	__super::Release(pActor);
}
