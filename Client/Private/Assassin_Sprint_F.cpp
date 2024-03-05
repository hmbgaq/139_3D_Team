#include "..\Public\Assassin_Sprint_F.h"

void CAssassin_Sprint_F::Initialize(CAssassin* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CAssassin>* CAssassin_Sprint_F::Update(CAssassin* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CAssassin_Sprint_F::Release(CAssassin* pActor)
{
	__super::Release(pActor);
}
