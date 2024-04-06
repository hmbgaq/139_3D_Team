#include "VampireCommander_Spawn1.h"
#include "VampireCommander_Idle.h"
#include "Body_VampireCommander.h"

#include "MasterCamera.h"
#include "Data_Manager.h"
#include "Player.h"
#include "Transform.h"
#include "Bone.h"

void CVampireCommander_Spawn1::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

	CBody_VampireCommander* pBody = dynamic_cast<CBody_VampireCommander*>(pActor->Get_Body());
	pBody->Set_RenderState(CBody_VampireCommander::RENDER_STATE::ATTACK);

	

}

CState<CVampireCommander>* CVampireCommander_Spawn1::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	CSpringCamera* pSpringCam = CData_Manager::GetInstance()->Get_MasterCamera()->Get_SpringCamera();
	if (m_bFlags[0] == false)
	{
		pSpringCam->Set_pTargetCharacter(pActor);
		m_bFlags[0] = true;
	}


	if (pActor->Is_Animation_End())
	{
		return new CVampireCommander_Idle;
	}

	return nullptr;
}

void CVampireCommander_Spawn1::Release(CVampireCommander* pActor)
{
	__super::Release(pActor);

	CSpringCamera* pSpringCam = CData_Manager::GetInstance()->Get_MasterCamera()->Get_SpringCamera();

	pSpringCam->Set_pTargetCharacter(CData_Manager::GetInstance()->Get_Player());
	//CPlayer* pPlayer = CData_Manager::GetInstance()->Get_Player();
	//pPlayer->Set_Rotate_In_CameraDir(true);
	//pPlayer->m_bPlayerCheck = true;
	//CCamera* pCam;
	//pCam = CData_Manager::GetInstance()->Get_MasterCamera()->Get_vectorCamera()[1];
	//CSpringCamera* pSpringCam = dynamic_cast<CSpringCamera*>(pCam);
	//pSpringCam->Set_pTarget(pPlayer->Get_Transform());
	//pSpringCam->Set_CameraOffset(_float3(1, 0.5f, -3.0f));
}
