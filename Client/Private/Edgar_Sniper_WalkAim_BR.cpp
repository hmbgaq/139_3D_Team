#include "..\Public\Edgar_Sniper_WalkAim_BR.h"

void CEdgar_Sniper_WalkAim_BR::Initialize(CEdgar* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true);

}

CState<CEdgar>* CEdgar_Sniper_WalkAim_BR::Update(CEdgar* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CEdgar_Sniper_WalkAim_BR::Release(CEdgar* pActor)
{
	__super::Release(pActor);
}
