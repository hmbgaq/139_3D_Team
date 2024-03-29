#include "..\Public\Player_L06_Wagon_Jump.h"

void CPlayer_L06_Wagon_Jump::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CPlayer>* CPlayer_L06_Wagon_Jump::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_L06_Wagon_Jump::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
