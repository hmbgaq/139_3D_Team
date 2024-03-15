#include "Infected_State_Death.h"
#include "Body_Infected.h"

void CInfected_State_Death::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);

	CBody_Infected* pBody = dynamic_cast<CBody_Infected*>(pActor->Get_Body());
	pBody->Set_RenderState(CBody_Infected::RENDER_STATE::NAKED);

	pActor->Set_Invincible(true);
}

CState<CInfected>* CInfected_State_Death::Update(CInfected* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CInfected_State_Death::Release(CInfected* pActor)
{
	__super::Release(pActor);
}

CState<CInfected>* CInfected_State_Death::Update_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Death_State(pActor, fTimeDelta, _iAnimIndex);
}
