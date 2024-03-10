#include "..\Public\Player_Finisher_VampireCommander_VS.h"

void CPlayer_Finisher_VampireCommander_VS::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CPlayer>* CPlayer_Finisher_VampireCommander_VS::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_Finisher_VampireCommander_VS::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
