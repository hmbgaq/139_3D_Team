#include "..\Public\Edgar_ThrowDynamite.h"

void CEdgar_ThrowDynamite::Initialize(CEdgar* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CEdgar>* CEdgar_ThrowDynamite::Update(CEdgar* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CEdgar_ThrowDynamite::Release(CEdgar* pActor)
{
	__super::Release(pActor);
}
