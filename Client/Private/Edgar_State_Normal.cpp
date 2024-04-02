#include "..\Public\Edgar_State_Normal.h"

void CEdgar_State_Normal::Initialize(CEdgar* pActor)
{
	__super::Initialize(pActor);
}

CState<CEdgar>* CEdgar_State_Normal::Update(CEdgar* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CEdgar_State_Normal::Release(CEdgar* pActor)
{
	__super::Release(pActor);
}

CState<CEdgar>* CEdgar_State_Normal::Update_State(CEdgar* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Normal_State(pActor, fTimeDelta, _iAnimIndex);
}
