#include "Player_MeleeSwap_02.h"
#include "Weapon.h"
#include "GameInstance.h"
#include "Clone_Manager.h"
#include "Effect.h"
#include "Bone.h"

void CPlayer_MeleeSwap_02::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);
	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

	//string Test = "Data_Animation/";
	//pActor->Set_EventNotify(Test, "Test2_AnimationData.json");


	//CWeapon* pWeapon = pActor->Get_Weapon(TEXT("Weapon_Punch_R"));

	//pWeapon
	//	->Set_Damage(0.f)
	//	->Set_Direction(Direction::Right)
	//	->Set_Power(Power::Medium)
	//	->Set_Force(0.0f);

	//pWeapon->Set_Enable(true);

}

CState<CPlayer>* CPlayer_MeleeSwap_02::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	if (pActor->Is_Inputable_Front(37))
	{
		return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
	}
	return nullptr;
}

void CPlayer_MeleeSwap_02::Release(CPlayer* pActor)
{
	__super::Release(pActor);

	//CWeapon* pWeapon = pActor->Get_Weapon(TEXT("Weapon_Punch_R"));
	//pWeapon->Set_Enable(false);
}
