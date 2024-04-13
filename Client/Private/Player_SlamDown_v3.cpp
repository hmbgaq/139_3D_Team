#include "..\Public\Player_SlamDown_v3.h"
#include "GameInstance.h"
#include "Player_IdleLoop.h"
#include "MasterCamera.h"
#include "Data_Manager.h"

#include "Effect_Manager.h"

void CPlayer_SlamDown_v3::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true, true, 7);

	m_pGameInstance->Play_Sound(L"PLAYER_IMPACT", L"Player_GroundSlamAttack_Impact_02.wav", CHANNELID::SOUND_PLAYER_IMPACT, 10.f);
}

CState<CPlayer>* CPlayer_SlamDown_v3::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	// ÀÌÆåÆ®
	EFFECT_MANAGER->Generate_Effect_AttachBone(&m_fEffectTimeAcc, 0.5f, fTimeDelta, "Player/SuperCharge/", "SuperCharge_Always_02.json", pActor, true, "Head");

	if (pActor->Is_Inputable_Front(59))
	{
		return new CPlayer_IdleLoop();
	}

	if (false == m_bFlags[0])
	{
		_float fDiff = (_float)(24 - pActor->Get_CurrentKeyFrames());
		pActor->Go_Straight(fTimeDelta * 0.0015f * fDiff * fDiff);
		//pActor->Go_Straight(fTimeDelta * 0.9f);
		m_bFlags[0] = pActor->Is_Inputable_Front(11);
	}
	else if (false == m_bFlags[1])
	{
		m_bFlags[1] = pActor->Is_Inputable_Front(19);
		if (true == m_bFlags[1])
		{
			//CameraSetting
			CSpringCamera* pSpringCam = CData_Manager::GetInstance()->Get_MasterCamera()->Get_SpringCamera();
			pSpringCam->Set_ShakeCameraTime(0.1f);
			pSpringCam->Set_ShakeCameraMinMax(_float2(0.f, 0.5f));

			pActor->Apply_Shake_And_Blur(Power::Medium);
			EFFECT_MANAGER->Play_Effect("Player/SlamDown/", "New_SlamDown_v3_01.json", nullptr, pActor->Get_Position()); // ÀÌÆåÆ®
			pActor->Slam();
		}
	}
	else if (false == m_bFlags[2])
	{
		m_bFlags[2] = pActor->Is_Inputable_Front(24);
	}
	else
	{
		return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
	}



	return nullptr;
}

void CPlayer_SlamDown_v3::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
