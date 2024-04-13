#include "..\Public\Player_State_Attack.h"

void CPlayer_State_Attack::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	
}

CState<CPlayer>* CPlayer_State_Attack::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CPlayer_State_Attack::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}

CState<CPlayer>* CPlayer_State_Attack::Update_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Attack_State(pActor, fTimeDelta, _iAnimIndex);
}
