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

	CGameObject* pMonster = { nullptr };

	pMonster = m_pGameInstance->Add_CloneObject_And_Get(LEVEL_SNOWMOUNTAINBOSS, L"Layer_Boss", TEXT("Prototype_GameObject_Son"));
	
	pMonster->Set_Position(_float3(86.f, 0.f, 80.36f));
	pMonster->Get_Transform()->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-180.f));

	pMonster = m_pGameInstance->Add_CloneObject_And_Get(LEVEL_SNOWMOUNTAINBOSS, L"Layer_Boss", TEXT("Prototype_GameObject_Son"));
	
	pMonster->Set_Position(_float3(120.f, 0.f, 80.36f));
	pMonster->Get_Transform()->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-180.f));
	
}
