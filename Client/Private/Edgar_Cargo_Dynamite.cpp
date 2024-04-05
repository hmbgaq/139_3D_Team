#include "..\Public\Edgar_Cargo_Dynamite.h"

void CEdgar_Cargo_Dynamite::Initialize(CEdgar* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CEdgar>* CEdgar_Cargo_Dynamite::Update(CEdgar* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CEdgar_Cargo_Dynamite::Release(CEdgar* pActor)
{
	__super::Release(pActor);
}
