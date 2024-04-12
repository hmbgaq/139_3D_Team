#include "Mother_Spawn.h"
#include "Mother_Idle.h"
#include "GameInstance.h"
#include "MasterCamera.h"
#include "Data_Manager.h"
#include "Effect_Manager.h"
#include "Player.h"

void CMother_Spawn::Initialize(CMother* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
	//CSpringCamera* pSpringCam = CData_Manager::GetInstance()->Get_MasterCamera()->Get_SpringCamera();

	//pSpringCam->Set_CameraOffset(_float3(3.f, 1.5f, -9.f));

}

CState<CMother>* CMother_Spawn::Update(CMother* pActor, _float fTimeDelta)
{
	CSpringCamera* pSpringCam = CData_Manager::GetInstance()->Get_MasterCamera()->Get_SpringCamera();

	//m_fOffSetX += fTimeDelta;
	if (m_bFlags[7] == false)
	{
		m_fOffSetY += (fTimeDelta*0.5f);
		if (m_fOffSetY >= 1.5f)
			m_fOffSetY = 1.5f;
		m_fOffSetZ -= (fTimeDelta * 3.f);
		if (m_fOffSetZ <= -9.f)
			m_fOffSetZ = -9.f;

		m_fOffSet = _float3(1.f, m_fOffSetY, m_fOffSetZ);

		pSpringCam->Set_CameraOffset(m_fOffSet);
	}

	if (m_fOffSetY >= 1.5f && m_fOffSetZ <= -9.f)
		m_bFlags[7] = true;

	//if (m_bFlags[5] == false && pActor->Is_Inputable_Front(1))
	//{
	//	pSpringCam->Set_CameraOffset(_float3(3.f, 1.5f, -9.f));
	//	m_bFlags[5] = true;
	//}

	if (m_bFlags[1] == false && pActor->Is_Inputable_Front(20))
	{
		pSpringCam->Set_ShakeCameraTime(0.4f);
		pSpringCam->Set_ShakeCameraMinMax(_float2(0.f, 0.5f));
		pActor->Apply_Shake_And_Blur(Power::Medium);
		EFFECT_MANAGER->Play_Effect("Parasiter/", "SY_Falling_Leaves_02.json", nullptr, CData_Manager::GetInstance()->Get_Player()->Get_Position());


		m_bFlags[1] = true;
	}



	if (m_bFlags[2] == false && pActor->Is_Inputable_Front(100))
	{
		pSpringCam->Set_ShakeCameraTime(1.f);
		pSpringCam->Set_ShakeCameraMinMax(_float2(0.f, 0.2f));
		pActor->Apply_Shake_And_Blur(Power::Medium);

		m_bFlags[2] = true;
	}

	if (m_bFlags[3] == false && pActor->Is_Inputable_Front(190))
	{
		pSpringCam->Set_ShakeCameraTime(1.f);
		pSpringCam->Set_ShakeCameraMinMax(_float2(0.f, 0.2f));
		pActor->Apply_Shake_And_Blur(Power::Medium);

		m_bFlags[3] = true;
	}
	//if (m_pSpringCamera != nullptr)
	//{
	//	m_fCameraFovy += (fTimeDelta);
	//	if (m_fCurrentFovy+m_fCameraFovy >= 90.f)
	//		m_fCameraFovy = 30.f;
	//	m_pSpringCamera->Set_Fovy(m_fCurrentFovy + m_fCameraFovy);
	//}


	if (m_bFlags[0] == false && pActor->Is_Inputable_Front(192))
	{
		//Son1
		pActor->m_pMonster1 = m_pGameInstance->Add_CloneObject_And_Get(LEVEL_SNOWMOUNTAINBOSS, L"Layer_Boss", TEXT("Prototype_GameObject_Son"));
		//CData_Manager::GetInstance()->Set_Son(pActor->m_pMonster1);
		pActor->m_pMonster1->Set_Position(_float3(90.f, 0.f, 88.f));
		pActor->m_pMonster1->Get_Transform()->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-180.f));
		m_bFlags[0] = true;
	}
	if (m_bFlags[4] == false && pActor->Is_Inputable_Front(203))
	{
		//Son2
		pActor->m_pMonster2 = m_pGameInstance->Add_CloneObject_And_Get(LEVEL_SNOWMOUNTAINBOSS, L"Layer_Boss", TEXT("Prototype_GameObject_Son"));
		//CData_Manager::GetInstance()->Set_Son2(pActor->m_pMonster2);
		pActor->m_pMonster2->Set_Position(_float3(110.f, 0.f, 88.f));//임시 좌표값 이거 승용이가 맵 깔면 다시 설정해야 할수 있음. 
		pActor->m_pMonster2->Get_Transform()->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-180.f));
		m_bFlags[4] = true;
	}
	if (pActor->Is_Animation_End())
	{
		return new CMother_Idle;
	}

	return nullptr;
}

void CMother_Spawn::Release(CMother* pActor)
{
	__super::Release(pActor);
	CSpringCamera* pSpringCam = CData_Manager::GetInstance()->Get_MasterCamera()->Get_SpringCamera();
	pSpringCam->Set_CameraOffset(_float3(1.f, 0.5f, -3.f));
 
}
