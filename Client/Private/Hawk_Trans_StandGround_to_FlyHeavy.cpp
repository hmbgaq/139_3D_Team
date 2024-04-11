#include "..\Public\Hawk_Trans_StandGround_to_FlyHeavy.h"
#include "Hawk_Fly_Heavy.h"

void CHawk_Trans_StandGround_to_FlyHeavy::Initialize(CHawk* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);


}

CState<CHawk>* CHawk_Trans_StandGround_to_FlyHeavy::Update(CHawk* pActor, _float fTimeDelta)
{
	pActor->Go_Straight(fTimeDelta);

	if (true == pActor->Is_Animation_End())
	{
		return new CHawk_Fly_Heavy();
	}

	return nullptr;
}

void CHawk_Trans_StandGround_to_FlyHeavy::Release(CHawk* pActor)
{
	__super::Release(pActor);
}
