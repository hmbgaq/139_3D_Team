#include "MotherMouth_SpittingMineLoop.h"
#include "MotherMouth_SpittingMineEnd.h"

void CMotherMouth_SpittingMineLoop::Initialize(CMotherMouth* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CMotherMouth>* CMotherMouth_SpittingMineLoop::Update(CMotherMouth* pActor, _float fTimeDelta)
{
	//몇번루프를 돌고 나서 몇번 루프를 돌았을때 알을 생성하고 알은 일정시간이내에 삭제하지 못하면 자폭몬스터가 생성이 되면서 튀어 나와야 함 
	if (pActor->Is_Animation_End())
	{
		return new CMotherMouth_SpittingMineEnd;
	}

	return nullptr;
}

void CMotherMouth_SpittingMineLoop::Release(CMotherMouth* pActor)
{
	__super::Release(pActor);
	pActor->m_bLoopFinished = true;//mother에서 이게 끝났는지 안끝났는지 확인하기 위한 불변수 
	//이거 해제하면서 뼈 위치값 받아서 그 위치의 월드 값에다가 알을 생성한후에 알에서 다시일정시간이 지난후 몬스터 생성
}
