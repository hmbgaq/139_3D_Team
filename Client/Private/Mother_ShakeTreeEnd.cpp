#include "Mother_ShakeTreeEnd.h"
#include "Mother_Idle.h"
#include "GameInstance.h"

void CMother_ShakeTreeEnd::Initialize(CMother* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CMother>* CMother_ShakeTreeEnd::Update(CMother* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return new CMother_Idle;
	}

	return nullptr;
}

void CMother_ShakeTreeEnd::Release(CMother* pActor)
{
	__super::Release(pActor);

	pActor->m_iSonDead = 0;
	pActor->m_bPhase = false;

	
	//Son1
	pActor->m_pMonster1 = m_pGameInstance->Add_CloneObject_And_Get(LEVEL_SNOWMOUNTAINBOSS, L"Layer_Boss", TEXT("Prototype_GameObject_Son"));
	pActor->m_pMonster1->Set_Position(_float3(86.f, 0.f, 80.36f));
	pActor->m_pMonster1->Get_Transform()->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-180.f));
	//Son2
	pActor->m_pMonster2 = m_pGameInstance->Add_CloneObject_And_Get(LEVEL_SNOWMOUNTAINBOSS, L"Layer_Boss", TEXT("Prototype_GameObject_Son"));
	pActor->m_pMonster2->Set_Position(_float3(120.f, 0.f, 80.36f));
	pActor->m_pMonster2->Get_Transform()->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-180.f));
	
}
