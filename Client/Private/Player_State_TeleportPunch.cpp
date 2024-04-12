#include "..\Public\Player_State_TeleportPunch.h"
#include "Player_IdleLoop.h"
#include "Transform.h"
#include "Data_Manager.h"

#include "Effect_Manager.h"
#include "Effect.h"

void CPlayer_State_TeleportPunch::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);


	// ÅÚ·¹Æ÷Æ® ÆÝÄ¡ ÀÌÆåÆ®
	m_pEffect = EFFECT_MANAGER->Play_Effect("Player/TeleportPunch/", "TeleportPunch_01.json", pActor, true, "Head");
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
		m_bFlags[0] = pActor->Is_Inputable_Front(2);
	}
	else if (false == m_bFlags[1])
	{
		pActor->Chasing_Attack(fTimeDelta, 30.f, 40);
		m_bFlags[1] = pActor->Is_Inputable_Front(4) || 1.5f >= pActor->Calc_Distance();
	}
	else if (false == m_bFlags[2] && 1.5f >= pActor->Calc_Distance())
	{
		

		//_vector vPlayerPos = pActor->Get_Position_Vector();
		//_vector vDir = pTarget->Calc_Look_Dir_XZ(vPlayerPos);
		
		_float fDamage = 30.f + CData_Manager::GetInstance()->Get_Additional_ElectricDamage();
		_float fForce = 1.f;
		Direction eDir = Direction::Front;
		Power ePower = Power::Heavy;
		_bool bIsMelee = true;

		//pTarget->Set_Hitted(fDamage, vDir, fForce, 1.f, eDir, ePower, bIsMelee);
		//pActor->Create_Hitting_Effect(pTarget->Get_Position(), ePower);

		pActor->Hit_Direct(pTarget, fDamage, fForce, eDir, ePower, bIsMelee);

		//pTarget->Look_At_And_Knockback(pActor->Get_Position(), 1.0f);
		//pTarget->Hitted_Dead(Power::Heavy);
		//pTarget->Set_Invincible(true);
		m_bFlags[2] = true;
	}




	return nullptr;
}

void CPlayer_State_TeleportPunch::Release(CPlayer* pActor)
{
	__super::Release(pActor);

	if (m_pEffect != nullptr)	// ÀÌÆåÆ® ÇØÁ¦
		EFFECT_MANAGER->Return_ToPool(m_pEffect);

	pActor->Set_Target(nullptr);
}

CState<CPlayer>* CPlayer_State_TeleportPunch::Update_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return TeleportPunch_State(pActor, fTimeDelta, _iAnimIndex);
}
