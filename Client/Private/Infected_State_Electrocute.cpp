#include "Infected_State_Electrocute.h"
#include "Body_Infected.h"

void CInfected_State_Electrocute::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_ElectrocuteTime(5.f);

	//CBody_Infected* pBody = dynamic_cast<CBody_Infected*>(pActor->Get_Body());
	//pBody->Set_StateHit();
}

CState<CInfected>* CInfected_State_Electrocute::Update(CInfected* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CInfected_State_Electrocute::Release(CInfected* pActor)
{
	__super::Release(pActor);
}

CState<CInfected>* CInfected_State_Electrocute::Update_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Electrocute_State(pActor, fTimeDelta, _iAnimIndex);
}
