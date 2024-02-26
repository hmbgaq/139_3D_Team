#include "..\Public\Player_DodgeBlink_CCW.h"

void CPlayer_DodgeBlink_CCW::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CPlayer>* CPlayer_DodgeBlink_CCW::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return Normal(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_DodgeBlink_CCW::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
