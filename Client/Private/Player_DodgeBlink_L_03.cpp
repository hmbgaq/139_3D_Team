#include "..\Public\Player_DodgeBlink_L_03.h"

#include "Effect.h"
#include "Effect_Manager.h"

void CPlayer_DodgeBlink_L_03::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);


	// 이펙트 플레이
	EFFECT_MANAGER->Play_Effect("Player/DodgeBlink/", "DodgeBlink_L_18.json", pActor);

}

CState<CPlayer>* CPlayer_DodgeBlink_L_03::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_DodgeBlink_L_03::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
