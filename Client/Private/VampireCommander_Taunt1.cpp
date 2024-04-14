#include "VampireCommander_Taunt1.h"
#include "VampireCommander_Idle.h"
#include "MasterCamera.h"
#include "Data_Manager.h"
#include "GameInstance.h"

void CVampireCommander_Taunt1::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
	
}

CState<CVampireCommander>* CVampireCommander_Taunt1::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return new CVampireCommander_Idle;
	}

	return nullptr;
}

void CVampireCommander_Taunt1::Release(CVampireCommander* pActor)
{
	__super::Release(pActor);
	CSpringCamera* pSpringCam = CData_Manager::GetInstance()->Get_MasterCamera()->Get_SpringCamera();
	pSpringCam->Set_CameraOffset(_float3(1.f, 0.5f, -3.f));
}
