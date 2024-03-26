#include "..\Public\Player_Shotgun_Fire_LongRange.h"
#include "GameInstance.h"

#include "Player_Shotgun_WeaponHolster.h"

void CPlayer_Shotgun_Fire_LongRange::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CPlayer>* CPlayer_Shotgun_Fire_LongRange::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	
	if (pActor->Is_Animation_End())
	{
		return new CPlayer_Shotgun_WeaponHolster();
		//return Normal(pActor, fTimeDelta, g_iAnimIndex);
	}

	return nullptr;

}

void CPlayer_Shotgun_Fire_LongRange::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
