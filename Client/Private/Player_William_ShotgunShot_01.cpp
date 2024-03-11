#include "..\Public\Player_William_ShotgunShot_01.h"
#include "GameInstance.h"

void CPlayer_William_ShotgunShot_01::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation_Upper(g_iAnimIndex, CModel::ANIM_STATE_LOOP);
	pActor->Set_Splitted(true);
}

CState<CPlayer>* CPlayer_William_ShotgunShot_01::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	pActor->Aim_Walk(fTimeDelta);

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_William_ShotgunShot_01::Release(CPlayer* pActor)
{
	__super::Release(pActor);
	pActor->Set_Splitted(false);
}
