#include "Player_OpenStateCombo_8hit.h"
#include "Weapon.h"
#include "GameInstance.h"
#include "Clone_Manager.h"
#include "Effect.h"
#include "Bone.h"

#include "Player_IdleLoop.h"

#define HIT_FIRST		0
#define HIT_SECOND		1
#define HIT_THIRD		2
#define HIT_FOURTH		3
#define HIT_FIFTH		4
#define HIT_SIXTH		5
#define HIT_SEVENTH		6
#define HIT_EIGHTH		7

#define INPUT_FIRST		11
#define INPUT_SECOND	12
#define INPUT_THIRD		13
#define INPUT_FOURTH	14
#define INPUT_FIFTH		15
#define INPUT_SIXTH		16
#define INPUT_SEVENTH	17



void CPlayer_OpenStateCombo_8hit::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);
	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
	pActor->Set_Invincible(true);

	CCharacter* pTarget = pActor->Get_Target();
	if (pTarget)
	{
		pTarget->Set_Position(pActor->Calc_Front_Pos());
	}
}

CState<CPlayer>* CPlayer_OpenStateCombo_8hit::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	pActor->Search_Target(1.f);
	CCharacter* pTarget = pActor->Get_Target();
	if (nullptr == pTarget)
	{
		return new CPlayer_IdleLoop();
	}	

	_uint iTiming = 8;

	if (false == m_bFlags[HIT_FIRST])
	{
		CState<CPlayer>* pState = Hit(pActor, HIT_FIRST, INPUT_FIRST, 13 - iTiming);
		if (pState) return pState;
	}
	else if (false == m_bFlags[HIT_SECOND])
	{
		CState<CPlayer>* pState = Hit(pActor, HIT_SECOND, INPUT_SECOND, 20 - iTiming);
		if (pState) return pState;
	}
	else if (false == m_bFlags[HIT_THIRD])
	{
		CState<CPlayer>* pState = Hit(pActor, HIT_THIRD, INPUT_THIRD, 28 - iTiming);
		if (pState) return pState;
	}
	else if (false == m_bFlags[HIT_FOURTH])
	{
		CState<CPlayer>* pState = Hit(pActor, HIT_FOURTH, INPUT_FOURTH, 33 - iTiming);
		if (pState) return pState;
	}
	else if (false == m_bFlags[HIT_FIFTH])
	{
		CState<CPlayer>* pState = Hit(pActor, HIT_FIFTH, INPUT_FIFTH, 41 - iTiming);
		if (pState) return pState;
	}
	else if (false == m_bFlags[HIT_SIXTH])
	{
		CState<CPlayer>* pState = Hit(pActor, HIT_SIXTH, INPUT_SIXTH, 50 - iTiming);
		if (pState) return pState;
	}
	else if (false == m_bFlags[HIT_SEVENTH])
	{
		CState<CPlayer>* pState = Hit(pActor, HIT_SEVENTH, INPUT_SEVENTH, 57 - iTiming);
		if (pState) return pState;
	}
	else if (false == m_bFlags[HIT_EIGHTH])
	{
		if (pActor->Is_Inputable_Front(62 - iTiming))
		{
			//_float fDamage = 50.f;
			//_float fForce = 0.7f;
			//Direction eDir = Direction::Front;
			//Power ePower = Power::Heavy;
			//_bool bIsMelee = true;

			//pActor->Hit_Direct(pTarget, fDamage, fForce, eDir, ePower, bIsMelee);

			pActor->Create_Hitting_Effect(pTarget->Get_Position(), Power::Heavy);
			m_pGameInstance->Set_RadialBlurTime(0.7f);
			pTarget->Look_At_And_Knockback(pActor->Get_Position(), 0.7f);
			pTarget->Hitted_Dead(Power::Heavy);
			pTarget->Get_Damaged(30.f);
			pTarget->Set_Invincible(true);

			
			m_bFlags[HIT_EIGHTH] = true;
		}
	}

	if (pActor->Is_Animation_End())
	{
		return new CPlayer_IdleLoop();
	}

	return nullptr;
}

void CPlayer_OpenStateCombo_8hit::Release(CPlayer* pActor)
{
	__super::Release(pActor);
	pActor->Set_Target(nullptr);
	pActor->Set_Invincible(false);

}

CState<CPlayer>* CPlayer_OpenStateCombo_8hit::Hit(CPlayer* pActor, _uint iHitCount, _uint iInputCount, _uint iAnimEnd)
{
	//if (m_pGameInstance->Mouse_Pressing(DIM_LB) || m_pGameInstance->Mouse_Down(DIM_LB))
	if (true)
	{
		m_bFlags[iInputCount] = true;
	}

	if (pActor->Is_Inputable_Front(iAnimEnd))
	{
		CCharacter* pTarget = pActor->Get_Target();

		if (false == m_bFlags[iInputCount] || nullptr == pTarget)
		{
			return new CPlayer_IdleLoop();
		}

		_bool bIsEven = (iHitCount + 1) % 2 == 0;
		if (bIsEven) 
		{
			pTarget->Hitted_Opened(Direction::Left);
		}
		else 
		{
			pTarget->Hitted_Opened(Direction::Right);
		}
		pTarget->Get_Damaged(3.f);


		//pActor->Set_Invincible(true);

		//pActor->Apply_Shake_And_Blur();

		m_pGameInstance->Set_RadialBlurTime(0.05f);

		m_bFlags[iHitCount] = true;
	}

	return nullptr;

}
