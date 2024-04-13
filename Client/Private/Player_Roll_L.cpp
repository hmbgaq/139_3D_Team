#include "..\Public\Player_Roll_L.h"

#include "Effect.h"
#include "Effect_Manager.h"

void CPlayer_Roll_L::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true, true, 2);

	// ÀÌÆåÆ® »ý¼º
	EFFECT_MANAGER->Play_Effect("Player/Roll/", "Roll_R_04.json", pActor);
}

CState<CPlayer>* CPlayer_Roll_L::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	if (pActor->Is_Inputable_Front(26))
	{
		return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
	}

	return nullptr;
}

void CPlayer_Roll_L::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
