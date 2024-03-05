#include "..\Public\Assassin_Run_FL.h"

void CAssassin_Run_FL::Initialize(CAssassin* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CAssassin>* CAssassin_Run_FL::Update(CAssassin* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CAssassin_Run_FL::Release(CAssassin* pActor)
{
	__super::Release(pActor);
}
