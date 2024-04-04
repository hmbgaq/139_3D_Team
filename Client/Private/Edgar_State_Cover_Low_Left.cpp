#include "..\Public\Edgar_State_Cover_Low_Left.h"

void CEdgar_State_Cover_Low_Left::Initialize(CEdgar* pActor)
{
	__super::Initialize(pActor);
}

CState<CEdgar>* CEdgar_State_Cover_Low_Left::Update(CEdgar* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CEdgar_State_Cover_Low_Left::Release(CEdgar* pActor)
{
	__super::Release(pActor);
}

CState<CEdgar>* CEdgar_State_Cover_Low_Left::Update_State(CEdgar* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Cover_Low_Left_State(pActor, fTimeDelta, _iAnimIndex);
}
