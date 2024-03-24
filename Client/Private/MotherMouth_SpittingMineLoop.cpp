#include "MotherMouth_SpittingMineLoop.h"
#include "MotherMouth_SpittingMineEnd.h"

void CMotherMouth_SpittingMineLoop::Initialize(CMotherMouth* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true);
}

CState<CMotherMouth>* CMotherMouth_SpittingMineLoop::Update(CMotherMouth* pActor, _float fTimeDelta)
{
	//몇번루프를 돌고 나서 몇번 루프를 돌았을때 알을 생성하고 알은 일정시간이내에 삭제하지 못하면 자폭몬스터가 생성이 되면서 튀어 나와야 함 

	return nullptr;
}

void CMotherMouth_SpittingMineLoop::Release(CMotherMouth* pActor)
{
	__super::Release(pActor);
}
