#include "Mother_SpittingLoop.h"
#include "Mother_SpittingEnd.h"
#include "GameInstance.h"

void CMother_SpittingLoop::Initialize(CMother* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CMother>* CMother_SpittingLoop::Update(CMother* pActor, _float fTimeDelta)
{
	//MotherMouth의 루프가 일정 시간 지나가서 끝나고 나면 애가 이제 End로 전환 ! 
	if (m_bFlags[0] == false && pActor->Is_Inputable_Front(17))
	{
		m_pGameInstance->Add_CloneObject(LEVEL_SNOWMOUNTAINBOSS, L"Layer_Boss", L"Prototype_GameObject_Mother_Egg");
		m_bFlags[0] = true;
	}


	if (pActor->Is_Animation_End())
	{
		return new CMother_SpittingEnd;
	}
	return nullptr;
}

void CMother_SpittingLoop::Release(CMother* pActor)
{
	__super::Release(pActor);
}
