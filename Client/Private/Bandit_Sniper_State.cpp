#include "stdafx.h"
#include "SMath.h"
#include "GameInstance.h"
#include "Bandit_Sniper_State.h"

#include "Sniper_IdlePose.h"
#include "Sniper_Idle.h"
#include "Sniper_CoverHigh_Side_R_Start.h"
#include "Sniper_CoverHigh_Side_L_Start.h"
#include "Sniper_CoverLow_Over_Start.h"
#include "Sniper_CoverLow_Side_L_Start.h"
#include "Sniper_CoverLow_Side_R_Start.h"
#include "Sniper_Crouch_Start.h"
#include "Sniper_CoverLow_Idle.h"
#include "Sniper_CoverLow_Reload.h"
#include "Sniper_CoverHigh_Reload.h"
#include "Sniper_CoverHigh_Idle.h"

void CBandit_Sniper_State::Initialize(CBandit_Sniper* pActor)
{
	iAttackCnt = 0;
	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);
}

CState<CBandit_Sniper>* CBandit_Sniper_State::Update(CBandit_Sniper* pActor, _float fTimeDelta)
{
	return nullptr;
}

void CBandit_Sniper_State::Release(CBandit_Sniper* pActor)
{
	Safe_Release(m_pGameInstance);
}

_bool CBandit_Sniper_State::Calculation_Direcion(CBandit_Sniper* pActor, _float4 vCurrentDir)
{
	/* 패턴을 잘못짜서 기껏 각도계산하는거 만들었더니 거의 무용지물이네 하 .. */

	_float fAngle = pActor->Target_Contained_Angle(pActor->Get_Target()->Get_Transform()->Get_Pos());

	if (0 <= fAngle && fAngle <= 90)
		return true;
	else if (-90 <= fAngle && fAngle < 0)
		return true;
	else if ( fAngle > 90 )
		return false;
	else if ( fAngle < -90 )
		return false;
	else
	{
		cout << "Bandit_Sniper : 각도계산안됨 Target_Contained_Angle 함수 다시체크 " << endl;
		return false;
	}
}


CState<CBandit_Sniper>* CBandit_Sniper_State::Normal_State(CBandit_Sniper* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	CState<CBandit_Sniper>* pState = { nullptr };

	pState = Normal(pActor, fTimeDelta, _iAnimIndex);
	if (pState)	return pState;

	return nullptr;
}

CState<CBandit_Sniper>* CBandit_Sniper_State::Walk_State(CBandit_Sniper* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	CState<CBandit_Sniper>* pState = { nullptr };

	pState = Normal(pActor, fTimeDelta, _iAnimIndex);
	if (pState)	return pState;

	return nullptr;
}

CState<CBandit_Sniper>* CBandit_Sniper_State::Cover_State(CBandit_Sniper* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	cout << "IdlePose" << endl;
	return new CSniper_IdlePose();
}

CState<CBandit_Sniper>* CBandit_Sniper_State::Hit_State(CBandit_Sniper* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	/* 애니메이션 혼동방지를 위해 2프레임짜리 Idle로 갔다가 돌아가도록하기 */
	if (pActor->Is_Animation_End())
	{
		return new CSniper_IdlePose();
	}

	return nullptr;
}


CState<CBandit_Sniper>* CBandit_Sniper_State::Death_State(CBandit_Sniper* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (pActor->Is_Animation_End())
	{
		return nullptr;
	}

	return nullptr;
}

CState<CBandit_Sniper>* CBandit_Sniper_State::Melee_State(CBandit_Sniper* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	/* Close Attack 상태 */
	CState<CBandit_Sniper>* pState = { nullptr };

	if (pActor->Is_Animation_End())
	{
		return new CSniper_IdlePose();
	}

	return nullptr;
}

/* 중앙제어 */
CState<CBandit_Sniper>* CBandit_Sniper_State::Normal(CBandit_Sniper* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	CState<CBandit_Sniper>* pState = { nullptr };
	
	/* 움직이질 않는데 이런게 왜필요해  */

	pState = Attack(pActor, fTimeDelta, _iAnimIndex);
	if (pState)	return pState;

	pState = Rest(pActor, fTimeDelta, _iAnimIndex);
	if (pState)	return pState;

	if (pActor->Is_Animation_End())
	{
		return new CSniper_IdlePose();
	}

	return nullptr;
}

CState<CBandit_Sniper>* CBandit_Sniper_State::Attack(CBandit_Sniper* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	/* 스나이퍼 기준 오른쪽 90도 왼쪽 90도 안에 플레이어 없으면 공격안함 */
	if(false ==  Calculation_Direcion(pActor, pActor->Get_Transform()->Get_Look()))
		return nullptr;

	if (pActor->Get_ProtectExist()) /* 쉴드 있음 */
	{
		if (iAttackCnt >= 5)
		{
			iAttackCnt = 0;
			return new CSniper_CoverLow_Reload();
		}
		iAttackCnt += 1;
		return new CSniper_CoverLow_Over_Start(); // 앉아있다가 정면 공격
	}
	else
	{
		/* 쉴드 없음 */
		if (iAttackCnt >= 5)
		{
			iAttackCnt = 0;
			return new CSniper_CoverHigh_Reload();
		}

		iAttackCnt += 1;
		/* 여기서 제자리공격하셈 */

	}

	return nullptr;
}

CState<CBandit_Sniper>* CBandit_Sniper_State::Rest(CBandit_Sniper* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	/* 공격안하고 숨고르고 있는 아이들만하는거임 */

	if (pActor->Get_ProtectExist())
	{
		/* 공격x , 쉴드 있음 */
		return new CSniper_CoverLow_Idle();
	}
	else
	{
		/* 공격x, 쉴드 없음 */
		return new CSniper_CoverHigh_Idle();
	}
	return nullptr;
}
