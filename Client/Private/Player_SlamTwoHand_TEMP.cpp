#include "Player_SlamTwoHand_TEMP.h"
#include "Weapon.h"
#include "GameInstance.h"
#include "Clone_Manager.h"
#include "Effect.h"
#include "Bone.h"

#include "Effect_Manager.h"

void CPlayer_SlamTwoHand_TEMP::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);
	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CPlayer>* CPlayer_SlamTwoHand_TEMP::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	if (false == m_bFlags[0] && true == pActor->Is_Inputable_Front(20))
	{
		EFFECT_MANAGER->Play_Effect("SlamDown_v2_25_Rock.json", pActor->Get_Position());
		pActor->Apply_Shake_And_Blur(Power::Heavy);
		pActor->Slam();
		m_bFlags[0] = true;
	}
	else if (false == m_bFlags[1])
	{
		m_bFlags[1] = pActor->Is_Inputable_Front(28);
	}
	else if (true == m_bFlags[1])
	{
		return Normal(pActor, fTimeDelta, g_iAnimIndex);
	}

	return nullptr;
}

void CPlayer_SlamTwoHand_TEMP::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
