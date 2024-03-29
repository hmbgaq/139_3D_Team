#include "Mother_ShakeTreeLoop.h"
#include "Mother_ShakeTreeEnd.h"

void CMother_ShakeTreeLoop::Initialize(CMother* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CMother>* CMother_ShakeTreeLoop::Update(CMother* pActor, _float fTimeDelta)
{
	//일정한 시간이 지나고 나서 부터 End 로  전환 ! 3번정도 충돌하면 될거 같기는 함 노말로 해놓고 카운트 다운으로 한번 애니메이션 다돌고 나서 한번더 돌때마다
	//--변수  이런식으로 체크해서 그 숫자가 0이 되었을때 루프를 벗어나서 End로 전환 ! 
	if (pActor->Is_Animation_End())
	{
		return new CMother_ShakeTreeEnd;
	}
	return nullptr;
}

void CMother_ShakeTreeLoop::Release(CMother* pActor)
{
	__super::Release(pActor);
}
