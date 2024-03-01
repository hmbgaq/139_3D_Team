#include "..\Public\Assassin_State_Walk.h"

void CAssassin_State_Walk::Initialize(CAssassin* pActor)
{
	__super::Initialize(pActor);
}

CState<CAssassin>* CAssassin_State_Walk::Update(CAssassin* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CAssassin_State_Walk::Release(CAssassin* pActor)
{
	__super::Release(pActor);
}

CState<CAssassin>* CAssassin_State_Walk::Update_State(CAssassin* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Walk_State(pActor, fTimeDelta, _iAnimIndex);
}
