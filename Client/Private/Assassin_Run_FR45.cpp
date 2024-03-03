#include "..\Public\Assassin_Run_FR45.h"

void CAssassin_Run_FR45::Initialize(CAssassin* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CAssassin>* CAssassin_Run_FR45::Update(CAssassin* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CAssassin_Run_FR45::Release(CAssassin* pActor)
{
	__super::Release(pActor);
}
