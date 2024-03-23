#include "MotherMouth_State_Normal.h"

void CMotherMouth_State_Normal::Initialize(CMotherMouth* pActor)
{
	__super::Initialize(pActor);
}

CState<CMotherMouth>* CMotherMouth_State_Normal::Update(CMotherMouth* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CMotherMouth_State_Normal::Release(CMotherMouth* pActor)
{
	__super::Release(pActor);
}

CState<CMotherMouth>* CMotherMouth_State_Normal::Update_State(CMotherMouth* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Normal_State(pActor, fTimeDelta, _iAnimIndex);
}
