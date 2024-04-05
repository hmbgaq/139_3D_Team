#include "..\Public\Edgar_State_Hit.h"

void CEdgar_State_Hit::Initialize(CEdgar* pActor)
{
	__super::Initialize(pActor);
}

CState<CEdgar>* CEdgar_State_Hit::Update(CEdgar* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CEdgar_State_Hit::Release(CEdgar* pActor)
{
	__super::Release(pActor);
}

CState<CEdgar>* CEdgar_State_Hit::Update_State(CEdgar* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Hit_State(pActor, fTimeDelta, _iAnimIndex);
}
