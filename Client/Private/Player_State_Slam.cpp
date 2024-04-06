#include "..\Public\Player_State_Slam.h"

#include "Data_Manager.h"
#include "SpringCamera.h"
#include "MasterCamera.h"

void CPlayer_State_Slam::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	CSpringCamera* pSpringCam = CData_Manager::GetInstance()->Get_MasterCamera()->Get_SpringCamera();
	if (pSpringCam)
	{
		pSpringCam->Set_CameraOffset(_float3(1.f, 0.5f, -5.0f));
	}
}

CState<CPlayer>* CPlayer_State_Slam::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CPlayer_State_Slam::Release(CPlayer* pActor)
{
	__super::Release(pActor);

	CSpringCamera* pSpringCam = CData_Manager::GetInstance()->Get_MasterCamera()->Get_SpringCamera();
	if (pSpringCam)
	{
		pSpringCam->Set_CameraOffset(_float3(1.f, 0.5f, -3.f));
	}
}

CState<CPlayer>* CPlayer_State_Slam::Update_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Slam_State(pActor, fTimeDelta, _iAnimIndex);
}
