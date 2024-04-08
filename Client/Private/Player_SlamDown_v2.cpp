#include "..\Public\Player_SlamDown_v2.h"

#include "Player_IdleLoop.h"

#include "Effect.h"
#include "Effect_Manager.h"

#include "MasterCamera.h"
#include "Data_Manager.h"

void CPlayer_SlamDown_v2::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CPlayer>* CPlayer_SlamDown_v2::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);


	if (pActor->Is_Inputable_Front(56))
	{
		return new CPlayer_IdleLoop();
	}

	if (false == m_bFlags[0])
	{
		_float fDiff = (_float)(24 - pActor->Get_CurrentKeyFrames());
		pActor->Go_Straight(fTimeDelta * 0.0015f * fDiff * fDiff);
		m_bFlags[0] = pActor->Is_Inputable_Front(11);
	}
	else if (false == m_bFlags[1])
	{
		m_bFlags[1] = pActor->Is_Inputable_Front(20);
		if (true == m_bFlags[1])
		{
			EFFECT_MANAGER->Play_Effect("Player/SlamDown/", "SlamDown_v1_03_Rock.json", pActor->Get_Position());

			//CameraSetting
			CSpringCamera* pSpringCam = CData_Manager::GetInstance()->Get_MasterCamera()->Get_SpringCamera();
			pSpringCam->Set_ShakeCameraTime(0.1f);
			pSpringCam->Set_ShakeCameraMinMax(_float2(0.f, 0.5f));

			pActor->Apply_Shake_And_Blur(Power::Medium);
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

void CPlayer_SlamDown_v2::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
