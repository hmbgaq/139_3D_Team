#include "Mother_ShakeTreeStart.h"
#include "Mother_ShakeTreeLoop.h"
#include "Data_Manager.h"
#include "MasterCamera.h"

void CMother_ShakeTreeStart::Initialize(CMother* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CMother>* CMother_ShakeTreeStart::Update(CMother* pActor, _float fTimeDelta)
{
	if (m_bFlags[0] == false && pActor->Is_Inputable_Front(29))
	{
		CSpringCamera* pSpringCam = CData_Manager::GetInstance()->Get_MasterCamera()->Get_SpringCamera();
		pSpringCam->Set_ShakeCameraTime(0.3f);
		pActor->Apply_Shake_And_Blur(Power::Light);
		//이떄 카메라 쉐이킹 하면서 맵에 전체 공격 패턴 추가하면 될 거같음 
		m_bFlags[0] = true;
	}

	if (pActor->Is_Animation_End())
	{
		return new CMother_ShakeTreeLoop;
	}


	return nullptr;
}

void CMother_ShakeTreeStart::Release(CMother* pActor)
{
	__super::Release(pActor);
}
