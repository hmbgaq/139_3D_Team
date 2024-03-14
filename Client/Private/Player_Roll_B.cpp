#include "..\Public\Player_Roll_B.h"

void CPlayer_Roll_B::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CPlayer>* CPlayer_Roll_B::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);


	if (pActor->Is_Inputable_Front(26))
	{
		return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
	}

	return nullptr;

}

void CPlayer_Roll_B::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
