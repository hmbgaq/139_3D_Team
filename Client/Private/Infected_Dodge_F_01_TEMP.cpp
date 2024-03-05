#include "..\Public\Infected_Dodge_F_01_TEMP.h"

void CInfected_Dodge_F_01_TEMP::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CInfected>* CInfected_Dodge_F_01_TEMP::Update(CInfected* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CInfected_Dodge_F_01_TEMP::Release(CInfected* pActor)
{
	__super::Release(pActor);
}
