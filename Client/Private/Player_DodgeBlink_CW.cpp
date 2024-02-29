#include "..\Public\Player_DodgeBlink_CW.h"

void CPlayer_DodgeBlink_CW::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CPlayer>* CPlayer_DodgeBlink_CW::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_DodgeBlink_CW::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
