#include "Player_EnergyWhip_Leap.h"
#include "Weapon.h"
#include "GameInstance.h"
#include "Effect_Manager.h"
#include "Effect.h"
#include "Bone.h"

void CPlayer_EnergyWhip_Leap::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);
	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true, true, 15);
}

CState<CPlayer>* CPlayer_EnergyWhip_Leap::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	if (false == m_bFlags[0])
	{
		pActor->Chasing_Attack(fTimeDelta, 30.f, 8);
		m_bFlags[0] = pActor->Is_Inputable_Front(22);
	}

	if (pActor->Is_Animation_End())
	{
		return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
	}

	return nullptr;
}

void CPlayer_EnergyWhip_Leap::Release(CPlayer* pActor)
{
	__super::Release(pActor);
	pActor->Set_Target(nullptr);
}
