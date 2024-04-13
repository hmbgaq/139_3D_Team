#include "Tank_State.h"
#include "GameInstance.h"
#include "Tank_Idle.h"
#include "SMath.h"

#include "Tank_MeleeDynamicNoShield_01.h"
#include "Tank_MeleeDynamicNoShield_02.h"
#include "Tank_MeleeDynamicShield_04.h"
#include "Tank_MeleeDynamicShield_05.h"
#include "Tank_GroundSlam_AoE_Stomp_01.h"
#include "Tank_GroundWave_01.h"
#include "Tank_GroundWaveNoShield_01.h"

#include "Tank_Charge_Start.h"
#include "Tank_ShieldRegain.h"
#include "Tank_Run_F.h"




void CTank_State::Initialize(CTank* pActor)
{
	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);
}

CState<CTank>* CTank_State::Update(CTank* pActor, _float fTimeDelta)
{
	return nullptr;
}

void CTank_State::Release(CTank* pActor)
{
	Safe_Release(m_pGameInstance);
}

CState<CTank>* CTank_State::Normal_State(CTank* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Normal(pActor, fTimeDelta, _iAnimIndex);
}

CState<CTank>* CTank_State::Walk_State(CTank* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Normal(pActor, fTimeDelta, _iAnimIndex);
}

CState<CTank>* CTank_State::Run_State(CTank* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Normal(pActor, fTimeDelta, _iAnimIndex);
}

CState<CTank>* CTank_State::Attack_State(CTank* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CTank>* CTank_State::Shield_State(CTank* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CTank>* CTank_State::NoShield_State(CTank* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CTank>* CTank_State::Hit_State(CTank* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (pActor->Is_Animation_End())
	{
		return Idle(pActor, fTimeDelta, _iAnimIndex);
	}

	return nullptr;
}

CState<CTank>* CTank_State::Death_State(CTank* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (pActor->Is_Animation_End())
	{
		pActor->Set_EnemyHUD_Dead();
		pActor->Set_Dead(true);
	}

	return nullptr;
}

CState<CTank>* CTank_State::Spawn_State(CTank* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CTank>* CTank_State::Stun_State(CTank* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CTank>* CTank_State::Normal(CTank* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	CState<CTank>* pState = { nullptr };

	pState = ShieldRegain(pActor, fTimeDelta, _iAnimIndex);
	if (pState)	return pState;

	pState = Attack(pActor, fTimeDelta, _iAnimIndex);
	if (pState)	return pState;

	pState = Idle(pActor, fTimeDelta, _iAnimIndex);
	if (pState)	return pState;

	pState = Run(pActor, fTimeDelta, _iAnimIndex);
	if (pState)	return pState;


	//if (pActor->Is_Animation_End())
	//{
	//	return Idle(pActor, fTimeDelta, _iAnimIndex);
	//}

	return nullptr;
}

CState<CTank>* CTank_State::Idle(CTank* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	//CState<CTank>* pState = new CTank_Idle();

	//return pState;

	if (pActor->Is_Animation_End())
	{
		return new CTank_Idle();
	}

	return nullptr;
}

CState<CTank>* CTank_State::Run(CTank* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (pActor->Get_Target() && CTank_Run_F::g_iAnimIndex != _iAnimIndex)
		return new CTank_Run_F();

	return nullptr;
}

CState<CTank>* CTank_State::Attack(CTank* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	CCharacter* pTarget = pActor->Get_Target();
	if (pTarget)
	{
		pActor->Look_At_Target();
		_float fDistance = pActor->Calc_Distance();
		_bool Is_ShieldBroken = pActor->Is_ShieldBroken();

		if (3.f > fDistance)
		{
			return MeleeDynamic(pActor, fTimeDelta, _iAnimIndex);
		}
		else if (6.f > fDistance)
		{
			if (true == Is_ShieldBroken)
			{
				_uint iRand = SMath::Random(0, 2);
				_uint iCount = pActor->Get_AttackCount();

				switch (iRand)
				{
				case 0:
					return GroundWave(pActor, fTimeDelta, _iAnimIndex);
				case 1:
					if (1 > iCount)
					{
						return GroundSlam(pActor, fTimeDelta, _iAnimIndex);
					}
					
				default:
					pActor->Reset_AttackCount();
					return GroundWave(pActor, fTimeDelta, _iAnimIndex);
				}
				
			}
			else 
			{
				return Charge(pActor, fTimeDelta, _iAnimIndex);
			}
			
		}
		else if (12.f > fDistance)
		{
			if (true == Is_ShieldBroken)
			{
				return GroundWave(pActor, fTimeDelta, _iAnimIndex);
			}

			_uint iRand = SMath::Random(0, 2);
			_uint iCount = pActor->Get_AttackCount();
			if (1 == iCount)
			{
				return GroundWave(pActor, fTimeDelta, _iAnimIndex);
			}
			else if (2 == iCount) 
			{
				return Charge(pActor, fTimeDelta, _iAnimIndex);
			}
			else 
			{
				switch (iRand)
				{
				case 0:
					return GroundWave(pActor, fTimeDelta, _iAnimIndex);
				case 1:
					return Charge(pActor, fTimeDelta, _iAnimIndex);
				default:
					return GroundWave(pActor, fTimeDelta, _iAnimIndex);
				}
			}
		}
	}

	return nullptr;
}

CState<CTank>* CTank_State::MeleeDynamic(CTank* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	_bool Is_ShieldBroken = pActor->Is_ShieldBroken();
	_uint iRand = SMath::Random(0, 2);

	if (true == Is_ShieldBroken)
	{
		switch (iRand)
		{
		case 0:
			return new CTank_MeleeDynamicNoShield_01();
		case 1:
			return new CTank_MeleeDynamicNoShield_02();
		default:
			return new CTank_MeleeDynamicNoShield_01();
		}
	}
	else 
	{
		switch (iRand)
		{
		case 0:
			return new CTank_MeleeDynamicShield_04();
		case 1:
			return new CTank_MeleeDynamicShield_05();
		default:
			return new CTank_MeleeDynamicShield_04();
		}
	}

	return nullptr;
}

CState<CTank>* CTank_State::GroundSlam(CTank* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return new CTank_GroundSlam_AoE_Stomp_01();
}

CState<CTank>* CTank_State::GroundWave(CTank* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	_bool Is_ShieldBroken = pActor->Is_ShieldBroken();
	if (true == Is_ShieldBroken)
	{
		return new CTank_GroundWaveNoShield_01();
	}
	else 
	{
		return new CTank_GroundWave_01();
	}

	
}

CState<CTank>* CTank_State::Charge(CTank* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return new CTank_Charge_Start();
}

CState<CTank>* CTank_State::ShieldRegain(CTank* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (false == pActor->Get_Shield_Follow() && false == pActor->Is_ShieldBroken())
	{
		return new CTank_ShieldRegain();
	}

	return nullptr;
}

