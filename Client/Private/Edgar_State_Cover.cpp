#include "..\Public\Edgar_State_Cover.h"

void CEdgar_State_Cover::Initialize(CEdgar* pActor)
{
	__super::Initialize(pActor);
}

CState<CEdgar>* CEdgar_State_Cover::Update(CEdgar* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CEdgar_State_Cover::Release(CEdgar* pActor)
{
	__super::Release(pActor);
}

CState<CEdgar>* CEdgar_State_Cover::Update_State(CEdgar* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Cover_State(pActor, fTimeDelta, _iAnimIndex);
}
