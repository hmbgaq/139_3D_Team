#include "..\Public\Infected_State_Dodge.h"

void CInfected_State_Dodge::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);
}

CState<CInfected>* CInfected_State_Dodge::Update(CInfected* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CInfected_State_Dodge::Release(CInfected* pActor)
{
	__super::Release(pActor);
}

CState<CInfected>* CInfected_State_Dodge::Update_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Normal_State(pActor, fTimeDelta, _iAnimIndex);
}
