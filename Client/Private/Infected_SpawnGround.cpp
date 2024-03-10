#include "Infected_SpawnGround.h"
#include "Infected_Idle.h"

void CInfected_SpawnGround::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CInfected>* CInfected_SpawnGround::Update(CInfected* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return new CInfected_Idle();
	}

	return nullptr;




	//return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CInfected_SpawnGround::Release(CInfected* pActor)
{
	__super::Release(pActor);
}

