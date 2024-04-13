#include "..\Public\Player_Roll_F.h"

void CPlayer_Roll_F::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true, true, 2);

}

CState<CPlayer>* CPlayer_Roll_F::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	if (pActor->Is_Inputable_Front(26))
	{
		return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
	}

	return nullptr;
}

void CPlayer_Roll_F::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
