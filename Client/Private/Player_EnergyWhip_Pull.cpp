#include "Player_EnergyWhip_Pull.h"
#include "Weapon.h"
#include "GameInstance.h"
#include "Effect_Manager.h"
#include "Effect.h"
#include "Bone.h"

void CPlayer_EnergyWhip_Pull::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);
	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true, true, 15);

	pActor->Search_Target(20.f);
}

CState<CPlayer>* CPlayer_EnergyWhip_Pull::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	CCharacter* pTarget = pActor->Get_Target();

	if (nullptr != pTarget)
	{
		//_float3 vFront = pActor->Get_Transform()->Calc_Front_Pos(_float3(0.f, 0.f, 2.f));
		//_float3 vPlayerPos = pActor->Get_Position();

		//pTarget->Dragged(fTimeDelta, vFront);
	}

	if (pActor->Is_Animation_End())
	{
		return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
	}

	return nullptr;
}

void CPlayer_EnergyWhip_Pull::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
