#include "..\Public\Player_State_Dodge.h"

void CPlayer_State_Dodge::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);
	pActor->Set_Invincible(true);

	pActor->Set_RootMoveRate(_float3(1.5f, 1.f, 1.5f));

	pActor->Play_Dodge_Sound();
}

CState<CPlayer>* CPlayer_State_Dodge::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CPlayer_State_Dodge::Release(CPlayer* pActor)
{
	__super::Release(pActor);
	pActor->Set_Invincible(false);

	pActor->Reset_RootMoveRate();
}

CState<CPlayer>* CPlayer_State_Dodge::Update_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Dodge_State(pActor, fTimeDelta, _iAnimIndex);
}
