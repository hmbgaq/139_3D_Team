#include "Player_SlamTwoHand_TEMP.h"
#include "Weapon.h"
#include "GameInstance.h"
#include "Clone_Manager.h"
#include "Effect.h"
#include "Bone.h"
#include "MasterCamera.h"
#include "Data_Manager.h"

#include "Effect_Manager.h"

void CPlayer_SlamTwoHand_TEMP::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);
	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true, true, 7);

	m_pGameInstance->Play_Sound(L"PLAYER_IMPACT", L"Player_GroundSlamAttack_Impact_04.wav", CHANNELID::SOUND_PLAYER_IMPACT, 10.f);
}

CState<CPlayer>* CPlayer_SlamTwoHand_TEMP::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	if (false == m_bFlags[0] && true == pActor->Is_Inputable_Front(20))
	{

		//CameraSetting
		CSpringCamera* pSpringCam = CData_Manager::GetInstance()->Get_MasterCamera()->Get_SpringCamera();
		pSpringCam->Set_ShakeCameraTime(0.1f);
		pSpringCam->Set_ShakeCameraMinMax(_float2(0.f, 0.5f));

		EFFECT_MANAGER->Play_Effect("Player/SlamDown/", "New_SlamTwoHand_TEMP_01.json", nullptr, pActor->Get_Position());
		pActor->Apply_Shake_And_Blur(Power::Heavy);
		pActor->Slam();

		

		m_bFlags[0] = true;
	}
	else if (false == m_bFlags[1])
	{
		m_bFlags[1] = pActor->Is_Inputable_Front(28);
	}
	else if (true == m_bFlags[1])
	{
		return Normal(pActor, fTimeDelta, g_iAnimIndex);
	}

	return nullptr;
}

void CPlayer_SlamTwoHand_TEMP::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
