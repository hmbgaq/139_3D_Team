#include "..\Public\Edgar_State_Crouch.h"

void CEdgar_State_Crouch::Initialize(CEdgar* pActor)
{
	__super::Initialize(pActor);
}

CState<CEdgar>* CEdgar_State_Crouch::Update(CEdgar* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CEdgar_State_Crouch::Release(CEdgar* pActor)
{
	__super::Release(pActor);
}

CState<CEdgar>* CEdgar_State_Crouch::Update_State(CEdgar* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Crouch_State(pActor, fTimeDelta, _iAnimIndex);
}
