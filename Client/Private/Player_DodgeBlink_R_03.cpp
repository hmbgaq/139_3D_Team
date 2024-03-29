#include "..\Public\Player_DodgeBlink_R_03.h"
#include "Effect.h"
#include "Effect_Manager.h"

void CPlayer_DodgeBlink_R_03::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);


	// ÀÌÆåÆ® »ý¼º
	CEffect* pEffect = EFFECT_MANAGER->Create_Effect("Player/DodgeBlink/", "DodgeBlink_R_12.json", pActor);
	//pEffect->Set_Position(pActor->Get_Position());
	//pEffect->Get_Transform()->Look_At(pActor->Get_Position());
}

CState<CPlayer>* CPlayer_DodgeBlink_R_03::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_DodgeBlink_R_03::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
