#include "stdafx.h"
#include "Event_MonsterSpawnTrigger.h"

#include "GameInstance.h"
#include "Imgui_Manager.h"
#include "Window_MapTool.h"

CEvent_MosnterSpawnTrigger::CEvent_MosnterSpawnTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEvent_Trigger(pDevice, pContext)
{
}

HRESULT CEvent_MosnterSpawnTrigger::Initialize(void* pArg)
{
	if(FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(__super::Ready_Component()))
		return E_FAIL;

	m_tMonsterTriggerDesc = *(CEvent_MosnterSpawnTrigger::MONSTERSPAWN_TRIGGERDESC*)pArg;

	
	Set_ColliderSize(m_tMonsterTriggerDesc.vColliderSize);
	Set_ColliderCenter(m_tMonsterTriggerDesc.vColliderCenter);

	_int iCurrentLevel = m_pGameInstance->Get_NextLevel();

	json LoadJson;

	string strLoadJsonPath;

	if (iCurrentLevel == (_uint)LEVEL_GAMEPLAY)
	{
		strLoadJsonPath = "../Bin/DataFiles/Data_Map/Stage1Final_MapData.json";
	}
	else if (iCurrentLevel == (_uint)LEVEL_INTRO_BOSS)
	{
		//strLoadJsonPath = "../Bin/DataFiles/Data_Map/Stage1Boss_Temp_MapData.json";
		strLoadJsonPath = "../Bin/DataFiles/Data_Map/Stage1Boss_MapData.json";

	}
	else if (iCurrentLevel == (_uint)LEVEL_SNOWMOUNTAIN)
	{
		strLoadJsonPath = "../Bin/DataFiles/Data_Map/SnowMountain_MapData.json";
	}
	else if (iCurrentLevel == (_uint)LEVEL_SNOWMOUNTAINBOSS)
	{
		strLoadJsonPath = "../Bin/DataFiles/Data_Map/Stage2Boss_MapData.json";
	}





	if (SUCCEEDED(CJson_Utility::Load_Json(strLoadJsonPath.c_str(), LoadJson)))
	{
		json MonsterJson = LoadJson["Monster_Json"];

		_uint iMonsterJsonFullSize = (_uint)MonsterJson.size();


		for (_uint i = 0; i < iMonsterJsonFullSize; ++i)
		{
			if (m_tMonsterTriggerDesc.iSpawnGroupIndex == MonsterJson[i]["MonsterGroupIndex"])
			{
				CMonster_Character::MONSTER_DESC MonsterDesc;

				MonsterDesc.bPreview = false;
				MonsterDesc.eDescType = CGameObject::MONSTER_DESC;
				MonsterDesc.iMonsterGroupIndex = MonsterJson[i]["MonsterGroupIndex"];
				MonsterDesc.iStartNaviIndex = MonsterJson[i]["StartNaviIndex"];

				const json& TransformJson = MonsterJson[i]["Component"]["Transform"];
				_float4x4 WorldMatrix;

				for (_int TransformLoopIndex = 0; TransformLoopIndex < 4; ++TransformLoopIndex)
				{
					for (_int TransformSecondLoopIndex = 0; TransformSecondLoopIndex < 4; ++TransformSecondLoopIndex)
					{
						WorldMatrix.m[TransformLoopIndex][TransformSecondLoopIndex] = TransformJson[TransformLoopIndex][TransformSecondLoopIndex];
					}
				}

				MonsterDesc.WorldMatrix = WorldMatrix;


				wstring strProtoTypeTag;
				m_pGameInstance->String_To_WString(MonsterJson[i]["PrototypeTag"], strProtoTypeTag);
				MonsterDesc.strProtoTypeTag = strProtoTypeTag;

				m_vecCreateMonsterDesc.push_back(MonsterDesc);
			}
		}
		m_bJsonReady = true;
	}

	return S_OK;
}

void CEvent_MosnterSpawnTrigger::Activate()
{
	_uint iMonsterDescVectorSize = (_uint)m_vecCreateMonsterDesc.size();
	_uint iCurrentLevel = m_pGameInstance->Get_NextLevel();



	for (_uint i = 0; i < iMonsterDescVectorSize; ++i)
	{
		CMonster_Character* pMonster = { nullptr };
			


		wstring strLayerTag = L"Layer_Monster";

			if (iCurrentLevel == _uint(LEVEL_INTRO_BOSS) || iCurrentLevel == _uint(LEVEL_SNOWMOUNTAINBOSS))
			{
				strLayerTag = L"Layer_Boss";
			}

			pMonster = dynamic_cast<CMonster_Character*>(m_pGameInstance->Add_CloneObject_And_Get(iCurrentLevel, strLayerTag, m_vecCreateMonsterDesc[i].strProtoTypeTag, &m_vecCreateMonsterDesc[i]));

			if (strLayerTag == L"Layer_Boss" && iCurrentLevel == _uint(LEVEL_INTRO_BOSS))
			{
				//m_pGameInstance->Play_BGM(L"BGM", L"IntroBossTriggerBGM.wav", 5.f);
			}
			else if (strLayerTag == L"Layer_Boss" && iCurrentLevel == _uint(LEVEL_SNOWMOUNTAINBOSS))
			{
				//m_pGameInstance->Play_BGM(L"BGM", L"SnowMountainBossTriggerBGM.wav", 5.f);
			}

		if (pMonster == nullptr)
		{
			MSG_BOX("Event_MonsterTrigger.cpp, 몬스터 스폰실패");
		}
		else if (iCurrentLevel == (_uint)LEVEL_TOOL)
		{

			CWindow_MapTool* pMapTool = dynamic_cast<CWindow_MapTool*>(CImgui_Manager::GetInstance()->Find_Window(CImgui_Manager::IMGUI_WINDOW_TYPE::IMGUI_MAPTOOL_WINDOW));

			pMapTool->Add_Monster_ForTrigger(pMonster);
		}

	}

	m_tTriggerDesc.bOnTrigger = true;
}

_bool CEvent_MosnterSpawnTrigger::Activate_Condition()
{
	if(m_bForceActivate == true)
		return true;

	_bool bSuperCondition = __super::Activate_Condition();

	

	

	if(bSuperCondition == false)
		return false;
	
	return true;
	
}

_bool CEvent_MosnterSpawnTrigger::End_Condition()
{
	if (m_tTriggerDesc.bOnTrigger == true)
		return true;
	else
		return false;
}

_bool CEvent_MosnterSpawnTrigger::Write_Json(json& Out_Json)
{
	json Temp_Json = Out_Json;

	__super::Write_Json(Out_Json);

	return false;
}

void CEvent_MosnterSpawnTrigger::Load_FromJson(const json& In_Json)
{
	__super::Load_FromJson(In_Json);

}

HRESULT CEvent_MosnterSpawnTrigger::Ready_Component()
{

	return S_OK;
}

CEvent_MosnterSpawnTrigger* CEvent_MosnterSpawnTrigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CEvent_MosnterSpawnTrigger* pInstance = new CEvent_MosnterSpawnTrigger(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CEvent_MosnterSpawnTrigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_MosnterSpawnTrigger::Set_ColliderSize(_float3 vColliderSize)
{
	__super::Set_ColliderSize(vColliderSize);

	m_tMonsterTriggerDesc.vColliderSize = vColliderSize;
	
}

void CEvent_MosnterSpawnTrigger::Set_ColliderCenter(_float3 vColliderCenter)
{
	__super::Set_ColliderCenter(vColliderCenter);

	m_tMonsterTriggerDesc.vColliderCenter = vColliderCenter;
}

void CEvent_MosnterSpawnTrigger::Free()
{
	__super::Free();
}
