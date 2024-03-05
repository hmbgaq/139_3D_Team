#include "..\Public\Infected_State_Knock.h"

void CInfected_State_Knock::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);
}

CState<CInfected>* CInfected_State_Knock::Update(CInfected* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CInfected_State_Knock::Release(CInfected* pActor)
{
	__super::Release(pActor);
}

CState<CInfected>* CInfected_State_Knock::Update_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Knock_State(pActor, fTimeDelta, _iAnimIndex);
}
