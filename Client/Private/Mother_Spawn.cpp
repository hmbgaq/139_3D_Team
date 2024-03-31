#include "Mother_Spawn.h"
#include "Mother_Idle.h"
#include "GameInstance.h"

void CMother_Spawn::Initialize(CMother* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CMother>* CMother_Spawn::Update(CMother* pActor, _float fTimeDelta)
{
	if (m_bFlags[0] == false && pActor->Is_Inputable_Front(92))
	{
	
		//Son1
		pActor->m_pMonster1 = m_pGameInstance->Add_CloneObject_And_Get(LEVEL_SNOWMOUNTAINBOSS, L"Layer_Boss", TEXT("Prototype_GameObject_Son"));
		pActor->m_pMonster1->Set_Position(_float3(86.f, 0.f, 80.36f));
		pActor->m_pMonster1->Get_Transform()->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-180.f));

		//Son2
		pActor->m_pMonster2 = m_pGameInstance->Add_CloneObject_And_Get(LEVEL_SNOWMOUNTAINBOSS, L"Layer_Boss", TEXT("Prototype_GameObject_Son"));
		pActor->m_pMonster2->Set_Position(_float3(120.f, 0.f, 80.36f));//임시 좌표값 이거 승용이가 맵 깔면 다시 설정해야 할수 있음. 
		pActor->m_pMonster2->Get_Transform()->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-180.f));
		m_bFlags[0] = true;
	}
	if (pActor->Is_Animation_End())
	{
		return new CMother_Idle;
	}

	return nullptr;
}

void CMother_Spawn::Release(CMother* pActor)
{
	__super::Release(pActor);
}
