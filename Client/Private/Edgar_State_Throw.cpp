#include "..\Public\Edgar_State_Throw.h"

void CEdgar_State_Throw::Initialize(CEdgar* pActor)
{
	__super::Initialize(pActor);
}

CState<CEdgar>* CEdgar_State_Throw::Update(CEdgar* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CEdgar_State_Throw::Release(CEdgar* pActor)
{
	__super::Release(pActor);
}

CState<CEdgar>* CEdgar_State_Throw::Update_State(CEdgar* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Throw_State(pActor, fTimeDelta, _iAnimIndex);
}
