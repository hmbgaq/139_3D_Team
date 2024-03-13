#include "..\Public\Player_VampireCommander_SyncedAttack.h"

#include "Player_Empowered_Idle.h"
#include "Data_Manager.h"
#include "MasterCamera.h"
void CPlayer_VampireCommander_SyncedAttack::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true ,true, 48);
}

CState<CPlayer>* CPlayer_VampireCommander_SyncedAttack::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	if (m_bFlags[0] == false && pActor->Is_Inputable_Front(99) )
	{
		pActor->Get_Damaged(50);
		m_bFlags[0] = true;
	}


	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_VampireCommander_SyncedAttack::Release(CPlayer* pActor)
{
	__super::Release(pActor);
	pActor->Set_Rotate_In_CameraDir(true);
	pActor->m_bPlayerCheck = true;
	CCamera* pCam;
	pCam = CData_Manager::GetInstance()->Get_MasterCamera()->Get_vectorCamera()[1];
	CSpringCamera* pSpringCam = dynamic_cast<CSpringCamera*>(pCam);
	pSpringCam->Set_pTarget(pActor->Get_Transform());
	pSpringCam->Set_CameraOffset(_float3(1, 0.5f, -3.0f));
	//CData_Manager::GetInstance()->Get_MasterCamera()->Set_CameraType(CMasterCamera::SpringCamera);
}
