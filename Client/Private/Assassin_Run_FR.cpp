#include "..\Public\Assassin_Run_FR.h"

void CAssassin_Run_FR::Initialize(CAssassin* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CAssassin>* CAssassin_Run_FR::Update(CAssassin* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CAssassin_Run_FR::Release(CAssassin* pActor)
{
	__super::Release(pActor);
}
