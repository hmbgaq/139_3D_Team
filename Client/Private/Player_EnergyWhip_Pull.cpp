#include "Player_EnergyWhip_Pull.h"
#include "Weapon.h"
#include "GameInstance.h"
#include "Effect_Manager.h"
#include "Effect.h"
#include "Bone.h"

void CPlayer_EnergyWhip_Pull::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);
	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true, true, 15);

	pActor->Search_Target(20.f);
}

CState<CPlayer>* CPlayer_EnergyWhip_Pull::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	CCharacter* pTarget = pActor->Get_Target();

	if (nullptr != pTarget)
	{
		if (false == m_bFlags[0])
		{
			pTarget->Hitted_Electrocute();
			m_bFlags[0] = true;
		}

		if (false == m_bFlags[1]) 
		{
			_float fDistance = 1.f;
			if (fDistance < pActor->Calc_Distance())
			{
				_float3 vFront = pActor->Calc_Front_Pos(_float3(0.f, 0.f, fDistance / 2.f));
				pTarget->Dragged(fTimeDelta, vFront);
				//if (fDistance >= pActor->Calc_Distance()) 
				//{
				//	m_bFlags[1] = true;
				//}
			}
			else 
			{
				m_bFlags[1] = true;
			}
		}
	}

	if (pActor->Is_Animation_End())
	{
		return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
	}

	return nullptr;
}

void CPlayer_EnergyWhip_Pull::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
