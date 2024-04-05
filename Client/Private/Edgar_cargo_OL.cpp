#include "..\Public\Edgar_cargo_OL.h"

void CEdgar_cargo_OL::Initialize(CEdgar* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CEdgar>* CEdgar_cargo_OL::Update(CEdgar* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CEdgar_cargo_OL::Release(CEdgar* pActor)
{
	__super::Release(pActor);
}
