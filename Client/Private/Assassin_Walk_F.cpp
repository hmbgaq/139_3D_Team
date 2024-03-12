#include "..\Public\Assassin_Walk_F.h"

void CAssassin_Walk_F::Initialize(CAssassin* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CAssassin>* CAssassin_Walk_F::Update(CAssassin* pActor, _float fTimeDelta)
{
	pActor->Go_Straight(fTimeDelta * 0.5f);

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CAssassin_Walk_F::Release(CAssassin* pActor)
{
	__super::Release(pActor);
}
