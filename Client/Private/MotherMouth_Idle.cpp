#include "MotherMouth_Idle.h"

void CMotherMouth_Idle::Initialize(CMotherMouth* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true);
}

CState<CMotherMouth>* CMotherMouth_Idle::Update(CMotherMouth* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CMotherMouth_Idle::Release(CMotherMouth* pActor)
{
	__super::Release(pActor);
}
