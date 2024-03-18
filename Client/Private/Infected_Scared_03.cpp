#include "Infected_Scared_03.h"
#include "Body_Infected.h"

void CInfected_Scared_03::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
	
	//CBody_Infected* pBody = dynamic_cast<CBody_Infected*>(pActor->Get_Body());
	//pBody->Set_StateHit();
}

CState<CInfected>* CInfected_Scared_03::Update(CInfected* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
	}
	return nullptr;
}

void CInfected_Scared_03::Release(CInfected* pActor)
{
	__super::Release(pActor);
}
