#include "VampireCommander_Ranged3.h"
#include "VampireCommander_Idle.h"
#include "GameInstance.h"
#include "Body_VampireCommander.h"

void CVampireCommander_Ranged3::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

	CBody_VampireCommander* pBody = dynamic_cast<CBody_VampireCommander*>(pActor->Get_Body());
	pBody->Set_RenderState(CBody_VampireCommander::RENDER_STATE::ATTACK);

	pActor->m_bLookAt = false;
}

CState<CVampireCommander>* CVampireCommander_Ranged3::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	if (m_bFlags[1] == false)
	{
		//pActor->m_bLookAt = true;
	}

	if (pActor->Is_Inputable_Front(66))
	{
		m_bFlags[1] = true;
		if (m_bfirstCheck)
		{
			m_pGameInstance->Add_CloneObject(LEVEL_INTRO_BOSS, L"Layer_Monster", L"Prototype_GameObject_VampireCommander_Projectile_Range3");
			m_bfirstCheck = false;
		}
	}
	else if (pActor->Is_Inputable_Front(68))
	{
		m_bFlags[1] = false;
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

	pActor->m_bLookAt = true;

}
