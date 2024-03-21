#include "Player_SlamTwoHand_TEMP.h"
#include "Weapon.h"
#include "GameInstance.h"
#include "Clone_Manager.h"
#include "Effect.h"
#include "Bone.h"

void CPlayer_SlamTwoHand_TEMP::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);
	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CPlayer>* CPlayer_SlamTwoHand_TEMP::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	if (pActor->Is_Inputable_Front(28))
	{
		return Normal(pActor, fTimeDelta, g_iAnimIndex);
	}

	return nullptr;
}

void CPlayer_SlamTwoHand_TEMP::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
