#include "..\Public\Assassin_State_Spawn.h"

void CAssassin_State_Spawn::Initialize(CAssassin* pActor)
{
	__super::Initialize(pActor);
}

CState<CAssassin>* CAssassin_State_Spawn::Update(CAssassin* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CAssassin_State_Spawn::Release(CAssassin* pActor)
{
	__super::Release(pActor);
}

CState<CAssassin>* CAssassin_State_Spawn::Update_State(CAssassin* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Spawn_State(pActor, fTimeDelta, _iAnimIndex);
}
