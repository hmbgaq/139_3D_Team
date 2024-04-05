#include "..\Public\Edgar_State_Cover_Low_Right.h"

void CEdgar_State_Cover_Low_Right::Initialize(CEdgar* pActor)
{
	__super::Initialize(pActor);
}

CState<CEdgar>* CEdgar_State_Cover_Low_Right::Update(CEdgar* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CEdgar_State_Cover_Low_Right::Release(CEdgar* pActor)
{
	__super::Release(pActor);
}

CState<CEdgar>* CEdgar_State_Cover_Low_Right::Update_State(CEdgar* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Cover_Low_Right_State(pActor, fTimeDelta, _iAnimIndex);
}
