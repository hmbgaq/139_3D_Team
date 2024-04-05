#include "..\Public\Edgar_Cargo_02.h"

void CEdgar_Cargo_02::Initialize(CEdgar* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CEdgar>* CEdgar_Cargo_02::Update(CEdgar* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CEdgar_Cargo_02::Release(CEdgar* pActor)
{
	__super::Release(pActor);
}
