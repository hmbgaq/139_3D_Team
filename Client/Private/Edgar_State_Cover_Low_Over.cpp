#include "..\Public\Edgar_State_Cover_Low_Over.h"

void CEdgar_State_Cover_Low_Over::Initialize(CEdgar* pActor)
{
	__super::Initialize(pActor);
}

CState<CEdgar>* CEdgar_State_Cover_Low_Over::Update(CEdgar* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CEdgar_State_Cover_Low_Over::Release(CEdgar* pActor)
{
	__super::Release(pActor);
}

CState<CEdgar>* CEdgar_State_Cover_Low_Over::Update_State(CEdgar* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Cover_Low_Over_State(pActor, fTimeDelta, _iAnimIndex);
}
