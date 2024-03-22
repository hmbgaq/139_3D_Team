#include "VampireCommander_SyncedAttack.h"
#include "VampireCommander_Idle.h"
#include "Body_VampireCommander.h"

void CVampireCommander_SyncedAttack::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);
	//여기서 Fail과의 분기점인 Trackposition 받아서 가져와야 함 
	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true,true,48);

	CWeapon* pWeapon = pActor->Get_Weapon(TEXT("Weapon_hand_R"));

	pWeapon
		->Set_Damage(50)
		->Set_Direction(Direction::Right)
		->Set_Power(Power::Medium)
		->Set_Force(0.f);

	pWeapon->Set_Enable(true);

	CBody_VampireCommander* pBody = dynamic_cast<CBody_VampireCommander*>(pActor->Get_Body());
	pBody->Set_RenderState(CBody_VampireCommander::RENDER_STATE::ATTACK);
}

CState<CVampireCommander>* CVampireCommander_SyncedAttack::Update(CVampireCommander* pActor, _float fTimeDelta)
{

	//  이스킬은 플레이어를 잡아다가 던져 버리는 거라서 플레이어와 연동되어야 함! 
	if (pActor->Is_Animation_End())
	{
		return new CVampireCommander_Idle();
	}


	return nullptr;
}

void CVampireCommander_SyncedAttack::Release(CVampireCommander* pActor)
{
	__super::Release(pActor);

	CWeapon* pWeapon = pActor->Get_Weapon(TEXT("Weapon_hand_R"));
	pWeapon->Set_Enable(false);


}
