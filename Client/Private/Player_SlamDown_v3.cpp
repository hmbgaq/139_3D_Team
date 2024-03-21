#include "..\Public\Player_SlamDown_v3.h"

#include "Player_IdleLoop.h"

void CPlayer_SlamDown_v3::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true, false);
}

CState<CPlayer>* CPlayer_SlamDown_v3::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	if (pActor->Is_Inputable_Front(60))
	{
		return new CPlayer_IdleLoop();
	}

	if (pActor->Is_Inputable_Front(26))
	{
		return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
	}

	return nullptr;
}

void CPlayer_SlamDown_v3::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
