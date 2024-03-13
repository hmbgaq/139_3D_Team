#include "Player_MeleeCombo_01.h"
#include "Weapon.h"
#include "GameInstance.h"
#include "Effect_Manager.h"
#include "Effect.h"
#include "Bone.h"

void CPlayer_MeleeCombo_01::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	string Test = "Data_Animation/";
	pActor->Set_EventNotify(Test, "Test2_AnimationData.json");

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

	CWeapon* pWeapon = pActor->Get_Weapon(WEAPON_PUNCH_R);
	pWeapon
		->Set_Damage(0)
		->Set_Direction(Direction::Right)
		->Set_Power(Power::Medium)
		->Set_Force(0.0f);

	pWeapon->Set_Enable(true);
	pWeapon->Set_Enable_Collisions(true);

}

CState<CPlayer>* CPlayer_MeleeCombo_01::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);
	
	if (false == m_bFlags[0] && pActor->Check_EffectOnTrackPosition())
	{
		CWeapon* pWeapon = pActor->Get_Weapon(WEAPON_PUNCH_R);
		_float3 vPos = pWeapon->Get_WorldPosition();

		pActor->Create_Effect(vPos);

		m_bFlags[0] = true;
	}

	if (pActor->Is_Inputable_Front(30)) 
	{
		return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
	}
	return nullptr;



	//콜라이더 제어 예시
	// 	if (m_fCurrentTrackPosition >= m_iColliderOnTrackPosition && m_fCurrentTrackPosition < m_iColliderOffTrackPosition)
	// 	{
	// 		m_pBoneCollider[m_iSelectColliderIndex]->Set_Enable(true);
	// 	}
	// 	else
	// 	{
	// 		m_pBoneCollider[m_iSelectColliderIndex]->Set_Enable(false);
	// 	}
}

void CPlayer_MeleeCombo_01::Release(CPlayer* pActor)
{
	__super::Release(pActor);

	pActor->Set_Weapon_Collisions_Enable(WEAPON_PUNCH_R, false);

	//CWeapon* pWeapon = pActor->Get_Weapon(TEXT("Weapon_Punch_R"));
	//pWeapon->Set_Enable_Collisions(false);
	// 
	//pWeapon->Set_Enable(false);
}
