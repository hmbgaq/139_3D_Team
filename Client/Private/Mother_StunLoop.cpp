#include "Mother_StunLoop.h"
#include "Mother_StunEnd.h"
#include "Mother_Hit.h"
#include "GameInstance.h"
#include "UI_Weakness.h"
#include "Bone.h"

void CMother_StunLoop::Initialize(CMother* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true);
}

CState<CMother>* CMother_StunLoop::Update(CMother* pActor, _float fTimeDelta)
{
	//구슬같은거 약점으로 다 파괴해야지만 다음으로 넘어감 
	if (m_bFlags[0] == false)
	{
		CUI_Weakness* pWeakneesUI = dynamic_cast<CUI_Weakness*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Weakness")));
		pActor->m_pWeakneesUIs.push_back(pWeakneesUI);
		m_bFlags[0] = true;
	}
	if (m_bFlags[1] == false)
	{
		CUI_Weakness* pWeakneesUI = dynamic_cast<CUI_Weakness*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Weakness")));
		pActor->m_pWeakneesUIs.push_back(pWeakneesUI);
		m_bFlags[1] = true;
	}

	if (pActor->m_pWeakneesUIs.size() > 0)
	{
		{//뼈의 위치 갱신1
			_float4x4 BoneMatrix = {};
			_float4x4 temp = {};

			BoneMatrix = pActor->Get_Body()->Get_BonePtr("Jaws_RD01")->Get_CombinedTransformationMatrix();
			_float4x4 pCharacterPos = pActor->Get_Transform()->Get_WorldMatrix();

			XMStoreFloat4x4(&temp, BoneMatrix * pCharacterPos);

			pActor->m_pWeakneesUIs[0]->SetUp_WorldToScreen(temp);
		}

		{//뼈의 위치 갱신2
			_float4x4 BoneMatrix = {};
			_float4x4 temp = {};

			BoneMatrix = pActor->Get_Body()->Get_BonePtr("Jaws_LD01")->Get_CombinedTransformationMatrix();
			_float4x4 pCharacterPos = pActor->Get_Transform()->Get_WorldMatrix();

			XMStoreFloat4x4(&temp, BoneMatrix * pCharacterPos);

			pActor->m_pWeakneesUIs[1]->SetUp_WorldToScreen(temp);
		}
	}

	if (m_bFlags[2] == false && pActor->m_pWeakneesUIs[0]->Get_Enable() == false)
	{
		++pActor->m_iShootingCount;
		m_bFlags[2] = true;
	}
	if (m_bFlags[3] == false && pActor->m_pWeakneesUIs[1]->Get_Enable() == false)
	{
		++pActor->m_iShootingCount;
		m_bFlags[3] = true;
	}

	if (pActor->m_iShootingCount == 1)
	{
		return new CMother_Hit;
	}
	else if (pActor->m_iShootingCount == 2)
	{
		pActor->m_iShootingCount = 0;
		return new CMother_StunEnd;
	}
	return nullptr;
}

void CMother_StunLoop::Release(CMother* pActor)
{
	__super::Release(pActor);
}	
