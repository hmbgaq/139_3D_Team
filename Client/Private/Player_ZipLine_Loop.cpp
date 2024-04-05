#include "..\Public\Player_ZipLine_Loop.h"

void CPlayer_ZipLine_Loop::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true);

}

CState<CPlayer>* CPlayer_ZipLine_Loop::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;

}

void CPlayer_ZipLine_Loop::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
