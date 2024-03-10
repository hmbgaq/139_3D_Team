#include "..\Public\Player_Finisher_CommanderLesser.h"

void CPlayer_Finisher_CommanderLesser::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CPlayer>* CPlayer_Finisher_CommanderLesser::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_Finisher_CommanderLesser::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
