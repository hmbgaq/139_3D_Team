#include "..\Public\Player_Bandit_Special_01.h"
#include "GameInstance.h"

#include "Player_IdleLoop.h"
#include "Player_Revolver_WeaponHolster.h"
#include "Player_Revolver_Hip_ReloadFast_Alt03.h"
#include "Player_Bandit_Reload_02.h"

void CPlayer_Bandit_Special_01::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Weapon_Enable(PLAYER_WEAPON_REVOLVER, true);

	pActor->Set_Animation_Upper(g_iAnimIndex, CModel::ANIM_STATE_NORMAL);

	pActor->Set_Splitted(false);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_STOP, true, false, 17);
	Create_Bullet(pActor);
}

CState<CPlayer>* CPlayer_Bandit_Special_01::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	fInputWaitTime += fTimeDelta;
	if (1.f <= fInputWaitTime)
	{
		return new CPlayer_IdleLoop();
	}

	if (m_pGameInstance->Key_Down(DIK_E) || m_pGameInstance->Key_Pressing(DIK_E))
	{
		pActor->Set_AnimState(CModel::ANIM_STATE_NORMAL);
		fInputWaitTime = 0.f;
	}

	//17
	if (false == m_bFlags[0])
	{
		m_bFlags[0] = pActor->Is_Inputable_Front(22);
		if (true == m_bFlags[0])
		{
			Create_Bullet(pActor);
			pActor->Set_AnimState(CModel::ANIM_STATE_STOP);
		}
	}
	else if (false == m_bFlags[1])
	{
		m_bFlags[1] = pActor->Is_Inputable_Front(28);
		if (true == m_bFlags[1])
		{
			Create_Bullet(pActor);
			pActor->Set_AnimState(CModel::ANIM_STATE_STOP);
		}
	}
	else if (false == m_bFlags[2])
	{
		m_bFlags[2] = pActor->Is_Inputable_Front(34);
		if (true == m_bFlags[2])
		{
			Create_Bullet(pActor);
			pActor->Set_AnimState(CModel::ANIM_STATE_STOP);
		}
	}
	else if (false == m_bFlags[3])
	{
		m_bFlags[3] = pActor->Is_Inputable_Front(39);
		if (true == m_bFlags[3])
		{
			Create_Bullet(pActor);
			pActor->Set_AnimState(CModel::ANIM_STATE_STOP);
		}
	}
	else if (false == m_bFlags[4])
	{
		m_bFlags[4] = pActor->Is_Inputable_Front(39);
		if (true == m_bFlags[4])
		{
			Create_Bullet(pActor);
			pActor->Set_AnimState(CModel::ANIM_STATE_STOP);
		}
	}
	else if (false == m_bFlags[5])
	{
		m_bFlags[5] = pActor->Is_Inputable_Front(45);
		if (true == m_bFlags[5])
		{
			Create_Bullet(pActor);
			pActor->Set_AnimState(CModel::ANIM_STATE_STOP);
			
		}
	}
	else if (true == m_bFlags[5])
	{
		//return new CPlayer_Bandit_Reload_02();
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

	pActor->Set_Weapon_Enable(PLAYER_WEAPON_REVOLVER, false);
}

void CPlayer_Bandit_Special_01::Create_Bullet(CPlayer* pActor)
{
	pActor->Search_Target(30.f);
	CCharacter* pTarget = pActor->Get_Target();
	if (pActor->Get_Target()) 
	{
		_uint iLevel = m_pGameInstance->Get_NextLevel();

		CGameObject* pBullet = m_pGameInstance->Add_CloneObject_And_Get(iLevel, LAYER_PLAYER_BULLET, L"Prototype_GameObject_Bullet_Revolver");
		_float3 vSpawnPos = pTarget->Get_WeaknessPos();
		pBullet->Set_Position(vSpawnPos);

		m_pGameInstance->Set_RadialBlurTime(0.1f);
		//pActor->Apply_Shake_And_Blur(Power::Light);
	}
	pActor->Set_Target(nullptr);

}
