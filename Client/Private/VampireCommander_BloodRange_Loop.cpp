#include "VampireCommander_BloodRange_Loop.h"
#include "VampireCommander_BloodRange_Stop.h"
#include "VampireCommander_BloodRange_Stun_Start.h"

#include "Effect.h"
#include "Effect_Manager.h"
#include "UI_Weakness.h"
#include "Bone.h"
#include "SMath.h"

void CVampireCommander_BloodRange_Loop::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);
	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true);
	m_iLoopescape = 0;
	m_fPreHP = pActor->Get_Hp();

	m_pEffect = EFFECT_MANAGER->Create_Effect("VampireCommander/BloodRange_Loop/", "BloodRange_Loop_22_Smoke_Smoke.json", pActor);

}

CState<CVampireCommander>* CVampireCommander_BloodRange_Loop::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	if (m_fHealHP>=200.f) //  && false == pActor->Is_Revealed_Weakness()
	{
		return new CVampireCommander_BloodRange_Stop();
	}
	else if (m_bSuccessShooting == true)//총으로 약점 공격 성공했을시
	{
		pActor->Set_Hp(m_fPreHP);
		return new CVampireCommander_BloodRange_Stun_Start();
	}
	//체력회복 
	m_fHealHP += 0.2f;

	pActor->Set_Hp(m_fPreHP + m_fHealHP);

	++m_iLoopescape;
	
	//벡터에 담아둔 뼈를 랜덤으로 불러내기
	if (m_bFlags[0] == false&& m_fHealHP >= 10.f&&m_fHealHP <70.f)
	{
		
		CUI_Weakness* pWeakneesUI = dynamic_cast<CUI_Weakness*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Weakness")));
		pActor->m_pWeakneesUIs.push_back(pWeakneesUI);
		m_iRandom[0] = SMath::Random(0, 12);
		m_bFlags[0] = true;
	}
	else if (m_bFlags[1] == false && m_fHealHP >= 70.f && m_fHealHP < 130.f)
	{
		CUI_Weakness* pWeakneesUI = dynamic_cast<CUI_Weakness*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Weakness")));
		pActor->m_pWeakneesUIs.push_back(pWeakneesUI);
		m_iRandom[1] = SMath::Random(0, 12);
		m_bFlags[1] = true;
	}
	else if (m_bFlags[2] == false && m_fHealHP >= 130.f)
	{
		CUI_Weakness* pWeakneesUI = dynamic_cast<CUI_Weakness*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Weakness")));
		pActor->m_pWeakneesUIs.push_back(pWeakneesUI);
		m_iRandom[2] = SMath::Random(0, 12);
		m_bFlags[2] = true;
	}

	

	if (pActor->m_pWeakneesUIs.size() > 0)
	{
		

		if(m_fHealHP >= 10.f&& pActor->m_pWeakneesUIs[0]->Get_Enable()==true)
		{//뼈의 위치 갱신1
			_float4x4 BoneMatrix = {};
			_float4x4 temp = {};

			BoneMatrix = pActor->Get_Body()->Get_BonePtr(pActor->m_vRandomBones[m_iRandom[0]].c_str())->Get_CombinedTransformationMatrix();
			_float4x4 pCharacterPos = pActor->Get_Transform()->Get_WorldMatrix();

			XMStoreFloat4x4(&temp, BoneMatrix * pCharacterPos);

			pActor->m_pWeakneesUIs[0]->SetUp_WorldToScreen(temp);
		}
		if (m_fHealHP >= 70.f && pActor->m_pWeakneesUIs[1]->Get_Enable() == true)
		{//뼈의 위치 갱신2
			_float4x4 BoneMatrix = {};
			_float4x4 temp = {};

			BoneMatrix = pActor->Get_Body()->Get_BonePtr(pActor->m_vRandomBones[m_iRandom[1]].c_str())->Get_CombinedTransformationMatrix();
			_float4x4 pCharacterPos = pActor->Get_Transform()->Get_WorldMatrix();

			XMStoreFloat4x4(&temp, BoneMatrix * pCharacterPos);

			pActor->m_pWeakneesUIs[1]->SetUp_WorldToScreen(temp);
		}
		if (m_fHealHP >= 130.f && pActor->m_pWeakneesUIs[2]->Get_Enable() == true)
		{//뼈의 위치 갱신3
			_float4x4 BoneMatrix = {};
			_float4x4 temp = {};

			BoneMatrix = pActor->Get_Body()->Get_BonePtr(pActor->m_vRandomBones[m_iRandom[2]].c_str())->Get_CombinedTransformationMatrix();
			_float4x4 pCharacterPos = pActor->Get_Transform()->Get_WorldMatrix();

			XMStoreFloat4x4(&temp, BoneMatrix * pCharacterPos);

			pActor->m_pWeakneesUIs[2]->SetUp_WorldToScreen(temp);
		}
		//카운트해서 3개가 다 쳐맞아서 꺼지면 스턴으로
		if (m_bFlags[3] == false && m_fHealHP >= 10.f &&  pActor->m_pWeakneesUIs[0]->Get_Enable() == false)
		{
			++m_iShootingCount;
			m_bFlags[3] = true;
		}
		if (m_bFlags[4] == false && m_fHealHP >= 70.f && pActor->m_pWeakneesUIs[1]->Get_Enable() == false)
		{
			++m_iShootingCount;
			m_bFlags[4] = true;
		}
		if (m_bFlags[5] == false && m_fHealHP >= 130.f && pActor->m_pWeakneesUIs[2]->Get_Enable() == false)
		{
			++m_iShootingCount;
			m_bFlags[5] = true;
		}

		if (m_iShootingCount >= 3)
			m_bSuccessShooting = true;
	}
	
	

	return nullptr;
}

void CVampireCommander_BloodRange_Loop::Release(CVampireCommander* pActor)
{
	__super::Release(pActor);
	m_pEffect->Set_Dead(true);
	for (int i = 0; i < pActor->m_pWeakneesUIs.size();++i)
	{
		pActor->m_pWeakneesUIs[i]->Set_Dead(true);
	}
	pActor->m_pWeakneesUIs.clear();
}
