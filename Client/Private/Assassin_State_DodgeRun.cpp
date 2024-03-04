#include "..\Public\Assassin_State_DodgeRun.h"

void CAssassin_State_DodgeRun::Initialize(CAssassin* pActor)
{
	__super::Initialize(pActor);
}

CState<CAssassin>* CAssassin_State_DodgeRun::Update(CAssassin* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CAssassin_State_DodgeRun::Release(CAssassin* pActor)
{
	__super::Release(pActor);
}

CState<CAssassin>* CAssassin_State_DodgeRun::Update_State(CAssassin* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return DodgeRun_State(pActor, fTimeDelta, _iAnimIndex);
}
