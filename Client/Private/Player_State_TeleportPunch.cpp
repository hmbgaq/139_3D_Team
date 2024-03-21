#include "..\Public\Player_State_TeleportPunch.h"
#include "Player_IdleLoop.h"
#include "Transform.h"

void CPlayer_State_TeleportPunch::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);
}

CState<CPlayer>* CPlayer_State_TeleportPunch::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	CCharacter* pTarget = pActor->Get_Target();
	if (nullptr == pTarget) 
	{
		return new CPlayer_IdleLoop();
	}

	if (false == m_bFlags[0])
	{
		pActor->Chasing_Attack(fTimeDelta, 30.f, 13);
		m_bFlags[0] = pActor->Is_Inputable_Front(4) || 2.5f >= pActor->Calc_Distance();
	}
	else if (false == m_bFlags[1] && 1.5f >= pActor->Calc_Distance())
	{
		//pTarget->Set_Position(pActor->Calc_Front_Pos(_float3(0.f, 0.f, 3.f)));

		//pActor->Apply_Shake_And_Blur(Power::Heavy);
		pActor->Create_Hitting_Effect(pTarget->Get_Position(), Power::Heavy);

		//pTarget->Add_Force(pActor->Calc_Look_Dir_XZ(pTarget->Get_Position_Vector()) * -1, 0.3f);
		//pTarget->Look_At_OnLand(pActor->Get_Position_Vector());
		pTarget->Look_At_And_Knockback(pActor->Get_Position(), 1.0f);
		pTarget->Hitted_Dead(Power::Heavy);
		pTarget->Set_Invincible(true);
		m_bFlags[1] = true;
	}

	return nullptr;
}

void CPlayer_State_TeleportPunch::Release(CPlayer* pActor)
{
	__super::Release(pActor);
	pActor->Set_Target(nullptr);
}

CState<CPlayer>* CPlayer_State_TeleportPunch::Update_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return TeleportPunch_State(pActor, fTimeDelta, _iAnimIndex);
}
