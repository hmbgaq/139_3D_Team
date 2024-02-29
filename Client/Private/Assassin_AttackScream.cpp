#include "..\Public\Assassin_AttackScream.h"

void CAssassin_AttackScream::Initialize(CAssassin* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CAssassin>* CAssassin_AttackScream::Update(CAssassin* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CAssassin_AttackScream::Release(CAssassin* pActor)
{
	__super::Release(pActor);
}
