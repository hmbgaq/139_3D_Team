#include "Mother_ShakeTreeLoop.h"
#include "Mother_ShakeTreeEnd.h"
#include "MasterCamera.h"
#include "Data_Manager.h"

void CMother_ShakeTreeLoop::Initialize(CMother* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true);
}

CState<CMother>* CMother_ShakeTreeLoop::Update(CMother* pActor, _float fTimeDelta)
{
	//일정한 시간이 지나고 나서 부터 End 로  전환 ! 5번정도 충돌하면 될거 같기는 함 노말로 해놓고 카운트 다운으로 한번 애니메이션 다돌고 나서 한번더 돌때마다
	//--변수  이런식으로 체크해서 그 숫자가 0이 되었을때 루프를 벗어나서 End로 전환 ! 
	m_fTimeDelta += fTimeDelta;
	if (m_bFlags[0] == false && pActor->Is_Inputable_Front(37))
	{
		CSpringCamera* pSpringCam = CData_Manager::GetInstance()->Get_MasterCamera()->Get_SpringCamera();
		pSpringCam->Set_ShakeCameraMinMax(_float2(0.f, 0.2f));
		pSpringCam->Set_ShakeCameraTime(0.3f);
		pActor->Apply_Shake_And_Blur(Power::Medium);
		//이떄 카메라 쉐이킹 하면서 맵에 전체 공격 패턴 추가하면 될 거같음 
		m_bFlags[0] = true;
	}

	if(pActor->Is_Inputable_Front(41))
		m_bFlags[0] = false;

	if (m_fTimeDelta >= 10.f && pActor->Is_Inputable_Front(41))
	{
		m_fTimeDelta = 0.f;
		return new CMother_ShakeTreeEnd;
	}
	return nullptr;
}

void CMother_ShakeTreeLoop::Release(CMother* pActor)
{
	__super::Release(pActor);
	CSpringCamera* pSpringCam = CData_Manager::GetInstance()->Get_MasterCamera()->Get_SpringCamera();
	pSpringCam->Set_ShakeCameraMinMax(_float2(0.3f, 0.3f));

	pSpringCam->Set_ShakeCameraTime(0.2f);
}
