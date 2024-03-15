#include "..\Public\Player_Sprint_F.h"
#include "Player_IdleLoop.h"

void CPlayer_Sprint_F::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CPlayer>* CPlayer_Sprint_F::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	if (pActor->Is_Inputable_Front(15))
	{
		return new CPlayer_IdleLoop();
	}

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_Sprint_F::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
