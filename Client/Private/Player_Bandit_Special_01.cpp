#include "..\Public\Player_Bandit_Special_01.h"
#include "GameInstance.h"

#include "Player_IdleLoop.h"
#include "Player_Revolver_WeaponHolster.h"
#include "Player_Revolver_Hip_ReloadFast_Alt03.h"

void CPlayer_Bandit_Special_01::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	//pActor->Set_Animation_Upper(g_iAnimIndex, CModel::ANIM_STATE_NORMAL);

	pActor->Set_Splitted(false);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true, false, 17);
}

CState<CPlayer>* CPlayer_Bandit_Special_01::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	//17
	if (false == m_bFlags[0])
	{
		m_bFlags[0] = pActor->Is_Inputable_Front(22);
	}
	else if (false == m_bFlags[1])
	{
		m_bFlags[1] = pActor->Is_Inputable_Front(28);
	}
	else if (false == m_bFlags[2])
	{
		m_bFlags[2] = pActor->Is_Inputable_Front(34);
	}
	else if (false == m_bFlags[3])
	{
		m_bFlags[3] = pActor->Is_Inputable_Front(39);
	}
	else if (false == m_bFlags[4])
	{
		m_bFlags[4] = pActor->Is_Inputable_Front(39);
	}
	else if (false == m_bFlags[5])
	{
		m_bFlags[5] = pActor->Is_Inputable_Front(45);
	}
	else if (true == m_bFlags[5])
	{
		return new CPlayer_Revolver_Hip_ReloadFast_Alt03();
	}

	//pActor->Aim_Walk(fTimeDelta);

	//if (pActor->Is_UpperAnimation_End())
	//{
	//	return new CPlayer_Bandit_Reload_02();
	//}

	//return nullptr;


	return nullptr;


}

void CPlayer_Bandit_Special_01::Release(CPlayer* pActor)
{
	__super::Release(pActor);
	//pActor->Set_Splitted(false);
}
