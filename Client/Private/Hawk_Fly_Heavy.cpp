#include "..\Public\Hawk_Fly_Heavy.h"
#include "Player.h"
#include "Data_Manager.h"
#include "Transform.h"

void CHawk_Fly_Heavy::Initialize(CHawk* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true);

	pActor->Get_Transform()->Set_Speed(7.f);
}

CState<CHawk>* CHawk_Fly_Heavy::Update(CHawk* pActor, _float fTimeDelta)
{
	pActor->Go_Straight(fTimeDelta);

	CPlayer* pPlayer = CData_Manager::GetInstance()->Get_Player();

	if (pPlayer)
	{
		_float3 vPlayerPos = pPlayer->Get_Position();

		_float fDistance = pActor->Calc_Distance(vPlayerPos);
		if (1000.f < fDistance) 
		{
			pActor->Set_Dead(true);
		}

	}

	return nullptr;

}

void CHawk_Fly_Heavy::Release(CHawk* pActor)
{
	__super::Release(pActor);
}
