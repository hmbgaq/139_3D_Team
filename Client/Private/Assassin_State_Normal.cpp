#include "..\Public\Assassin_State_Normal.h"

void CAssassin_State_Normal::Initialize(CAssassin* pActor)
{
	__super::Initialize(pActor);
}

CState<CAssassin>* CAssassin_State_Normal::Update(CAssassin* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CAssassin_State_Normal::Release(CAssassin* pActor)
{
	__super::Release(pActor);
}

CState<CAssassin>* CAssassin_State_Normal::Update_State(CAssassin* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Normal_State(pActor, fTimeDelta, _iAnimIndex);
}
