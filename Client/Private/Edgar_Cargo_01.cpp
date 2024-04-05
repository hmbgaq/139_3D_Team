#include "..\Public\Edgar_Cargo_01.h"

void CEdgar_Cargo_01::Initialize(CEdgar* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CEdgar>* CEdgar_Cargo_01::Update(CEdgar* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CEdgar_Cargo_01::Release(CEdgar* pActor)
{
	__super::Release(pActor);
}
