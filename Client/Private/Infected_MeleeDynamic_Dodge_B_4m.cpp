#include "..\Public\Infected_MeleeDynamic_Dodge_B_4m.h"

void CInfected_MeleeDynamic_Dodge_B_4m::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true);
}

CState<CInfected>* CInfected_MeleeDynamic_Dodge_B_4m::Update(CInfected* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CInfected_MeleeDynamic_Dodge_B_4m::Release(CInfected* pActor)
{
	__super::Release(pActor);
}
