#include "Player_State_Death.h"
#include "SpringCamera.h"
#include "Data_Manager.h"
#include "MasterCamera.h"
#include "GameInstance.h"
#include "Renderer.h"

void CPlayer_State_Death::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Splitted(false);
	pActor->Set_UseMouseMove(false);
	pActor->Set_Weapons_Enable_False();
	m_pGameInstance->Set_PlayerDead(true); 

	CSpringCamera* pSpringCam = CData_Manager::GetInstance()->Get_MasterCamera()->Get_SpringCamera();
	if (pSpringCam)
	{
		pSpringCam->Set_CameraOffset(_float3(1.f, 0.5f, -3.f));
	}

	pActor->Play_Dead_Sound();
}

CState<CPlayer>* CPlayer_State_Death::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CPlayer_State_Death::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}

CState<CPlayer>* CPlayer_State_Death::Update_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Death_State(pActor, fTimeDelta, _iAnimIndex);
}
