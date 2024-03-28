#include "..\Public\Tank_Charge_Loop.h"
#include "SMath.h"
#include "Tank_Charge_Fail_L.h"
#include "Tank_Charge_Hit_L.h"

void CTank_Charge_Loop::Initialize(CTank* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true, false);

}

CState<CTank>* CTank_Charge_Loop::Update(CTank* pActor, _float fTimeDelta)
{
	m_fTime += fTimeDelta;

	pActor->Go_Straight(fTimeDelta);
	
	if (3.f <= m_fTime)
	{
		//_uint iRand = SMath::Random(0, 2);
		return new CTank_Charge_Fail_L();

		//return Idle(pActor, fTimeDelta, g_iAnimIndex);
	}

	CCharacter* pTarget = pActor->Get_Target();
	if (pTarget)
	{
		_float fDistance = pActor->Calc_Distance();
		if (1.f > fDistance && false == pTarget->Is_Invincible())
		{
			return new CTank_Charge_Hit_L();
		}

	}

	return nullptr;

	//return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CTank_Charge_Loop::Release(CTank* pActor)
{
	__super::Release(pActor);
}
