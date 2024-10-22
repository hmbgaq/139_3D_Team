#include "..\Public\Player_State_Hit.h"
#include "SpringCamera.h"
#include "Data_Manager.h"
#include "MasterCamera.h"

void CPlayer_State_Hit::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);
	pActor->Set_Splitted(false);
	pActor->Set_UseMouseMove(false);
	pActor->Set_Weapons_Enable_False();

	CSpringCamera* pSpringCam = CData_Manager::GetInstance()->Get_MasterCamera()->Get_SpringCamera();
	if (pSpringCam)
	{
		pSpringCam->Set_CameraOffset(_float3(1.f, 0.5f, -3.f));
	}

	//CameraSetting
	pSpringCam->Set_ShakeCameraTime(0.05f);
	pSpringCam->Set_ShakeCameraMinMax(_float2(0.f, 0.3f));
	pActor->Apply_Shake_And_Blur(Power::Light);

	pActor->Play_Hitted_Sound();
}

CState<CPlayer>* CPlayer_State_Hit::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CPlayer_State_Hit::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}

CState<CPlayer>* CPlayer_State_Hit::Update_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Hit_State(pActor, fTimeDelta, _iAnimIndex);
}
