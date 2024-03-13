#include "stdafx.h"
#include "SMath.h"
#include "Sniper_Taunt_01.h"
#include "Sniper_Taunt_02.h"
#include "Sniper_Taunt_03.h"
#include "Sniper_Crouch_Stop.h"

void CSniper_Crouch_Stop::Initialize(CBandit_Sniper* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CBandit_Sniper>* CSniper_Crouch_Stop::Update(CBandit_Sniper* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		_int iRandomTunt = SMath::Random(1, 3);

		switch (iRandomTunt)
		{
		case 1:
			return new CSniper_Taunt_01();
			break;
		case 2:
			return new CSniper_Taunt_02();
			break;
		case 3:
			return new CSniper_Taunt_03();
			break;
		}	
	}

	return nullptr;
}

void CSniper_Crouch_Stop::Release(CBandit_Sniper* pActor)
{
	__super::Release(pActor);
}
