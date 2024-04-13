#include "Mother_VomitLoop.h"
#include "Mother_VomitEnd.h"
#include "GameInstance.h"

#include "Effect_Manager.h"

void CMother_VomitLoop::Initialize(CMother* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true);
}

CState<CMother>* CMother_VomitLoop::Update(CMother* pActor, _float fTimeDelta)
{
	//일정 시간이 지나가면 End로 전환 
	m_fCloneTime += fTimeDelta;
	m_fTimeDelta += fTimeDelta;

	if (m_fCloneTime >= 0.1f)
	{
		m_fCloneTime = 0.f;
		m_pGameInstance->Add_CloneObject(LEVEL_SNOWMOUNTAINBOSS, L"Layer_Boss", L"Prototype_GameObject_MotherVomit");
	}

	if (m_fTimeDelta >= 7.f)
	{
		//m_fTimeDelta = 0.0f;
		return new CMother_VomitEnd;
	}

	return nullptr;
}

void CMother_VomitLoop::Release(CMother* pActor)
{
	__super::Release(pActor);
}
