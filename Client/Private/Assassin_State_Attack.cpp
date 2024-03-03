#include "..\Public\Assassin_State_Attack.h"

void CAssassin_State_Attack::Initialize(CAssassin* pActor)
{
	__super::Initialize(pActor);
}

CState<CAssassin>* CAssassin_State_Attack::Update(CAssassin* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CAssassin_State_Attack::Release(CAssassin* pActor)
{
	__super::Release(pActor);
}

CState<CAssassin>* CAssassin_State_Attack::Update_State(CAssassin* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Attack_State(pActor, fTimeDelta, _iAnimIndex);
}
