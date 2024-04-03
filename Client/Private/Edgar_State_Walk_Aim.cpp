#include "..\Public\Edgar_State_Walk_Aim.h"

void CEdgar_State_Walk_Aim::Initialize(CEdgar* pActor)
{
	__super::Initialize(pActor);
}

CState<CEdgar>* CEdgar_State_Walk_Aim::Update(CEdgar* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CEdgar_State_Walk_Aim::Release(CEdgar* pActor)
{
	__super::Release(pActor);
}

CState<CEdgar>* CEdgar_State_Walk_Aim::Update_State(CEdgar* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Walk_Aim_State(pActor, fTimeDelta, _iAnimIndex);
}
