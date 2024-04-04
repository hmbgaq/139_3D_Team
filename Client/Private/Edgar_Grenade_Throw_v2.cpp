#include "..\Public\Edgar_Grenade_Throw_v2.h"

void CEdgar_Grenade_Throw_v2::Initialize(CEdgar* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CEdgar>* CEdgar_Grenade_Throw_v2::Update(CEdgar* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CEdgar_Grenade_Throw_v2::Release(CEdgar* pActor)
{
	__super::Release(pActor);
}
