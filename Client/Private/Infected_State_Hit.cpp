#include "Infected_State_Hit.h"
#include "Body_Infected.h"

void CInfected_State_Hit::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);

	CBody_Infected* pBody = dynamic_cast<CBody_Infected*>(pActor->Get_Body());
	pBody->Set_StateHit();
}

CState<CInfected>* CInfected_State_Hit::Update(CInfected* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CInfected_State_Hit::Release(CInfected* pActor)
{
	__super::Release(pActor);
}

CState<CInfected>* CInfected_State_Hit::Update_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Hit_State(pActor, fTimeDelta, _iAnimIndex);
}
