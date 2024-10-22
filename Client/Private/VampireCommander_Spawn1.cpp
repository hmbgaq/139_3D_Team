#include "VampireCommander_Spawn1.h"
#include "VampireCommander_Idle.h"
#include "Body_VampireCommander.h"
#include "VampireCommander_Weapon.h"
#include "VampireCommander_Taunt1.h"

#include "MasterCamera.h"
#include "Data_Manager.h"
#include "Player.h"
#include "Transform.h"
#include "Bone.h"
#include "GameInstance.h"

void CVampireCommander_Spawn1::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
	CBody_VampireCommander* pBody = dynamic_cast<CBody_VampireCommander*>(pActor->Get_Body());
	pBody->Set_RenderState(CBody_VampireCommander::RENDER_STATE::ATTACK);

	CWeapon* pWeapon_R = pActor->Get_Weapon(TEXT("Weapon_hand_R"));
	dynamic_cast<CVampireCommander_Weapon*>(pWeapon_R)->Play_Trail(true);	// 트레일 켜기

	CWeapon* pWeapon_L = pActor->Get_Weapon(TEXT("Weapon_hand_L"));
	dynamic_cast<CVampireCommander_Weapon*>(pWeapon_L)->Play_Trail(true);	// 트레일 켜기

	pActor->Set_UseGravity(false);
}

CState<CVampireCommander>* CVampireCommander_Spawn1::Update(CVampireCommander* pActor, _float fTimeDelta)
{


	CSpringCamera* pSpringCam = CData_Manager::GetInstance()->Get_MasterCamera()->Get_SpringCamera();

	if (m_bFlags[0] == false)
	{
		m_bFlags[0] = pActor->Is_Inputable_Front(21);

		if (true == m_bFlags[0])
		{
			m_pGameInstance->Play_Sound(L"VAMPIRE_BODYFALL", L"commander_lesser_mvm_bodyfall002.wav", SOUND_ENEMY_BODYMOVE, 10.f);
		}
		
	}

	if (m_bFlags[7] == false)
	{
		//m_fOffSetY += (fTimeDelta * 0.5f);
		//if (m_fOffSetY >= 1.5f)
		//	m_fOffSetY = 1.5f;
		m_fOffSetZ += (fTimeDelta * 10.f);
		if (m_fOffSetZ >= 22.f)
			m_fOffSetZ = 22.f;

		m_fOffSet = _float3(1.f, 0.5f, m_fOffSetZ);

		
		pSpringCam->Set_CameraOffset(m_fOffSet);
	}

	if (m_fOffSetZ >= 22.f)
		m_bFlags[7] = true;
	//if (m_bFlags[0] == false)
	//{
	//	pSpringCam->Set_pTargetCharacter(pActor);
	//	m_bFlags[0] = true;
	//}


	if (pActor->Is_Animation_End())
	{
		pActor->Set_UseGravity(true);
		return new CVampireCommander_Taunt1();
	}

	return nullptr;
}

void CVampireCommander_Spawn1::Release(CVampireCommander* pActor)
{
	__super::Release(pActor);

	CWeapon* pWeapon_R = pActor->Get_Weapon(TEXT("Weapon_hand_R"));
	dynamic_cast<CVampireCommander_Weapon*>(pWeapon_R)->Play_Trail(false);	// 트레일 끄기
	CWeapon* pWeapon_L = pActor->Get_Weapon(TEXT("Weapon_hand_L"));
	dynamic_cast<CVampireCommander_Weapon*>(pWeapon_L)->Play_Trail(false);	// 트레일 끄기

	
	//CPlayer* pPlayer = CData_Manager::GetInstance()->Get_Player();
	//pPlayer->Set_Rotate_In_CameraDir(true);
	//pPlayer->m_bPlayerCheck = true;
	//CCamera* pCam;
	//pCam = CData_Manager::GetInstance()->Get_MasterCamera()->Get_vectorCamera()[1];
	//CSpringCamera* pSpringCam = dynamic_cast<CSpringCamera*>(pCam);
	//pSpringCam->Set_pTarget(pPlayer->Get_Transform());
	//pSpringCam->Set_CameraOffset(_float3(1, 0.5f, -3.0f));
}
