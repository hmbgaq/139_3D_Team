#include "..\Public\Player_Finisher_BanditHeavy.h"

void CPlayer_Finisher_BanditHeavy::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CPlayer>* CPlayer_Finisher_BanditHeavy::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_Finisher_BanditHeavy::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
