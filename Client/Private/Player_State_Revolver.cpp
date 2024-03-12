#include "..\Public\Player_State_Revolver.h"

void CPlayer_State_Revolver::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);
}

CState<CPlayer>* CPlayer_State_Revolver::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CPlayer_State_Revolver::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}

CState<CPlayer>* CPlayer_State_Revolver::Update_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Revolver_State(pActor, fTimeDelta, _iAnimIndex);
}
