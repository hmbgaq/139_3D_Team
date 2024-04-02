#include "..\Public\Edgar_State_Walk.h"

void CEdgar_State_Walk::Initialize(CEdgar* pActor)
{
	__super::Initialize(pActor);
}

CState<CEdgar>* CEdgar_State_Walk::Update(CEdgar* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CEdgar_State_Walk::Release(CEdgar* pActor)
{
	__super::Release(pActor);
}

CState<CEdgar>* CEdgar_State_Walk::Update_State(CEdgar* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Walk_State(pActor, fTimeDelta, _iAnimIndex);
}
