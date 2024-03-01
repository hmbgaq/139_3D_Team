#include "..\Public\Assassin_Riposte_02.h"

void CAssassin_Riposte_02::Initialize(CAssassin* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CAssassin>* CAssassin_Riposte_02::Update(CAssassin* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CAssassin_Riposte_02::Release(CAssassin* pActor)
{
	__super::Release(pActor);
}
