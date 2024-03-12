#include "VampireCommander_Ranged3.h"
#include "VampireCommander_Idle.h"
#include "GameInstance.h"

void CVampireCommander_Ranged3::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CVampireCommander>* CVampireCommander_Ranged3::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	if (pActor->Is_Inputable_Front(66))
	{
		if (m_bfirstCheck)
		{
			m_pGameInstance->Add_CloneObject(LEVEL_INTRO_BOSS, L"Layer_Monster", L"Prototype_GameObject_VampireCommander_Projectile_Range1");
			m_bfirstCheck = false;
		}
	}

	if (pActor->Is_Animation_End())
	{
		return new CVampireCommander_Idle();
	}

	return nullptr;
}

void CVampireCommander_Ranged3::Release(CVampireCommander* pActor)
{
	__super::Release(pActor);
}
