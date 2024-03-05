#include "..\Public\Assassin_Walk_BR.h"

void CAssassin_Walk_BR::Initialize(CAssassin* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CAssassin>* CAssassin_Walk_BR::Update(CAssassin* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CAssassin_Walk_BR::Release(CAssassin* pActor)
{
	__super::Release(pActor);
}
