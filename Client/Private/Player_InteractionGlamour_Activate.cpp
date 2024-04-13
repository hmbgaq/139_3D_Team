#include "..\Public\Player_InteractionGlamour_Activate.h"
#include "Data_Manager.h"
#include "Body_Player.h"
#include "Bone.h"
#include "Effect.h"
#include "Effect_Manager.h"

void CPlayer_InteractionGlamour_Activate::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation_Upper(g_iAnimIndex, CModel::ANIM_STATE_NORMAL);
	pActor->Set_Splitted(true);

	//// !성희 추가 : 회복 쿨타임 적용
	//pActor->LeftHUDCoolDown("LeftHUD_Right", pActor->Get_LeftHUDMaxCoolTime("LeftHUD_Right"));
	
	//pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);


	EFFECT_MANAGER->Play_Effect("Player/Heal/", "Heal_07_Light_03.json", pActor, true, "RightHandIK");
	EFFECT_MANAGER->Play_Effect("Player/Heal/", "Heal_08.json", pActor);
	EFFECT_MANAGER->Play_Effect("Player/Heal/", "Heal_Particle_07_Reverse.json", pActor);
	EFFECT_MANAGER->Play_Effect("Player/Heal/", "Heal_Particle_07.json", pActor);


	//pActor->Activate_HUD_Skill(CPlayer::HUD::LEFT_RIGHT);


	//pActor->Set_HUD_Cooltime(CPlayer::HUD::LEFT_RIGHT, 5.f);
	//pActor->Set_HUD_MaxCooltime(CPlayer::HUD::LEFT_RIGHT, 5.f);
}

CState<CPlayer>* CPlayer_InteractionGlamour_Activate::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	pActor->Aim_Walk(fTimeDelta);

	if (false == m_bFlags[0])
	{
		m_bFlags[0] = pActor->Is_Upper_Inputable_Front(24);
		if (true == m_bFlags[0])
		{
			pActor->Set_BodyRender(1); // Heal RenderPass 
			pActor->Set_Hp(CData_Manager::GetInstance()->Get_HpRegen());
		}
	}
	
	if (false == m_bFlags[1])
	{
		if (true == pActor->Is_Upper_Inputable_Front(34)) // 44 가 끝 
		{
			pActor->Set_BodyRender(0); // Origin RenderPass 
			m_bFlags[1] = true;
		}
	}

	if (pActor->Is_UpperAnimation_End())
	{
		//pActor->Set_BodyRender(0); // Origin RenderPass 
		return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
	}

	return nullptr;              
}

void CPlayer_InteractionGlamour_Activate::Release(CPlayer* pActor)
{
	__super::Release(pActor);
	pActor->Set_Splitted(false);
}
