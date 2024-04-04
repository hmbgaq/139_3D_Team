#include "..\Public\Edgar_Cargo_DLG.h"

void CEdgar_Cargo_DLG::Initialize(CEdgar* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CEdgar>* CEdgar_Cargo_DLG::Update(CEdgar* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CEdgar_Cargo_DLG::Release(CEdgar* pActor)
{
	__super::Release(pActor);
}
