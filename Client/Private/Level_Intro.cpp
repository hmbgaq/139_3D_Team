#include "stdafx.h"
#include "Level_Intro.h"
#include "GameInstance.h"
#include "Player.h"
#include "Camera_Dynamic.h"
#include "Environment_Instance.h"
#include "Effect_Instance.h"

#pragma region UI
#include "UI_Player_HPBar.h"
#include "UI_Anything.h"
#include "UI_MonsterHp.h"
#include "UI_MonsterHpFrame.h"
#pragma endregion

#include "LandObject.h"

#pragma region MAP
#include "Environment_Object.h"
#include "Environment_Instance.h"

#pragma endregion


#pragma region Test
#include "Monster.h"
#include "Screamer.h"  
#include "InstanceMonster.h"
#pragma endregion

#pragma region Effect_Test
#include "Clone_Manager.h"
#include "Effect.h"
#pragma endregion

#include "Data_Manager.h"

CLevel_Intro::CLevel_Intro(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Intro::Initialize()
{
	m_pGameInstance->Set_CurrentLevel(m_pGameInstance->Get_NextLevel());

	FAILED_CHECK(Ready_LightDesc());
	FAILED_CHECK(Ready_Layer_Effect(TEXT("Layer_Effect")));
	FAILED_CHECK(Ready_Layer_BackGround(TEXT("Layer_BackGround")));
	FAILED_CHECK(Ready_LandObjects());
	FAILED_CHECK(Ready_Layer_Test(TEXT("Layer_Test")));
	FAILED_CHECK(Ready_Layer_Camera(TEXT("Layer_Camera")));

	CData_Manager::GetInstance()->Get_Player()->Set_Position(_float3(250.66f, 0.f, 2.38f));


	if (FAILED(Ready_UI()))
		return E_FAIL;

	return S_OK;
}

void CLevel_Intro::Tick(_float fTimeDelta)
{


#pragma region Effect_Test	

	if (m_pGameInstance->Key_Down(DIK_GRAVE))
	{
		CEffect* pEffect = CClone_Manager::GetInstance()->Create_Effect(LEVEL_INTRO, LAYER_EFFECT, "Hit_3.json");
		pEffect->Set_Position(_float3(0.f, 1.f, 0.f));
	}


	//if (m_pGameInstance->Key_Down(DIK_TAB))
	//{
	//	CEffect* pEffect = CClone_Manager::GetInstance()->Create_Effect(LEVEL_GAMEPLAY, LAYER_EFFECT, "Hit_3.json");
	//	if (nullptr != m_pGameInstance->Get_Player())
	//	{
	//		CTransform* pTransform = m_pGameInstance->Get_Player()->Get_Transform();
	//		_float3 vPos = pTransform->Get_Position();
	//		vPos.y += 1.f;
	//		pEffect->Set_Position(vPos);
	//	}
	//	else
	//	{
	//		pEffect->Set_Position(_float3(0.f, 1.f, 0.f));
	//	}
	//}
#pragma endregion


}

HRESULT CLevel_Intro::Render()
{
	SetWindowText(g_hWnd, TEXT("게임플레이레벨입니다."));

	return S_OK;
}

HRESULT CLevel_Intro::Ready_LightDesc()
{
	/* For. Shadow */
	//XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(XMVectorSet(-20.f, 20.f, -20.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	//XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), g_iWinSizeX / (float)g_iWinSizeY, 0.1f, lightfar 임 ));
	m_pGameInstance->Add_ShadowLight_View(ECast(LEVEL::LEVEL_INTRO), _float4(Engine::g_vLightPos), _float4(0.f, 0.f, 0.f, 1.f), _float4(0.f, 1.f, 0.f, 0.f));
	m_pGameInstance->Add_ShadowLight_Proj(ECast(LEVEL::LEVEL_INTRO), 60.f, (_float)g_iWinSizeX / (_float)g_iWinSizeY, Engine::g_fLightNear, Engine::g_fLightFar);

	LIGHT_DESC			LightDesc{};
	{
		LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
		LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
		LightDesc.vDiffuse = _float4(0.6f, 0.6f, 0.6f, 1.f);
		LightDesc.vAmbient = _float4(0.2f, 0.2f, 0.2f, 1.f);
		LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

		FAILED_CHECK(m_pGameInstance->Add_Light(LightDesc, TempLightNumber));
	}
	{
		ZeroMemory(&LightDesc, sizeof LightDesc);

		LightDesc.eType = LIGHT_DESC::TYPE_POINT;
		LightDesc.vPosition = _float4(30.f, 3.f, 30.f, 1.f);
		LightDesc.fRange = 20.f;
		LightDesc.vDiffuse = _float4(1.f, 0.0f, 0.0f, 1.f);
		LightDesc.vAmbient = _float4(0.4f, 0.1f, 0.1f, 1.f);
		LightDesc.vSpecular = LightDesc.vDiffuse;
		FAILED_CHECK(m_pGameInstance->Add_Light(LightDesc, TempLightNumber));

		LightDesc.eType = LIGHT_DESC::TYPE_POINT;
		LightDesc.vPosition = _float4(50.f, 3.f, 30.f, 1.f);
		LightDesc.fRange = 20.f;
		LightDesc.vDiffuse = _float4(0.0f, 1.f, 0.0f, 1.f);
		LightDesc.vAmbient = _float4(0.1f, 0.4f, 0.1f, 1.f);
		LightDesc.vSpecular = LightDesc.vDiffuse;
		FAILED_CHECK(m_pGameInstance->Add_Light(LightDesc, TempLightNumber));

		LightDesc.eType = LIGHT_DESC::TYPE_POINT;
		LightDesc.vPosition = _float4(70.f, 10.f, 30.f, 1.f);
		LightDesc.fRange = 20.f;
		LightDesc.vDiffuse = _float4(1.f, 0.0f, 1.f, 1.f);
		LightDesc.vAmbient = _float4(0.4f, 0.1f, 0.4f, 1.f);
		LightDesc.vSpecular = LightDesc.vDiffuse;
		FAILED_CHECK(m_pGameInstance->Add_Light(LightDesc, TempLightNumber));
	}
	return S_OK;
}

HRESULT CLevel_Intro::Ready_Layer_Camera(const wstring& strLayerTag)
{
	//CCamera_Dynamic::DYNAMIC_CAMERA_DESC		Desc = {};

	//Desc.fMouseSensor = 0.05f;
	//Desc.vEye = _float4(0.f, 20.f, -15.f, 1.f);
	//Desc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	//Desc.fFovy = XMConvertToRadians(60.0f);
	//Desc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	//Desc.fNear = 0.1f;
	//Desc.fFar = m_pGameInstance->Get_CamFar();
	//Desc.fSpeedPerSec = 15.f;
	//Desc.fRotationPerSec = XMConvertToRadians(180.0f);

	//FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_INTRO, strLayerTag, TEXT("Prototype_GameObject_Camera_Dynamic"), &Desc));


	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_INTRO, strLayerTag, TEXT("Prototype_GameObject_MasterCamera"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Intro::Ready_Layer_Player(const wstring& strLayerTag, void* pArg)
{
	FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_INTRO, strLayerTag, TEXT("Prototype_GameObject_Player"), pArg));
	//FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_INTRO, strLayerTag, TEXT("Prototype_GameObject_Rentier"), pArg));


	//CGameObject* pPlayer = m_pGameInstance->Add_CloneObject_And_Get(LEVEL_INTRO, strLayerTag, TEXT("Prototype_GameObject_Player"), pArg);
	//if (nullptr == pPlayer)
	//	return E_FAIL;

	//m_pGameInstance->Set_Player(dynamic_cast<CCharacter*>(pPlayer));

	return S_OK;
}

HRESULT CLevel_Intro::Ready_Layer_Effect(const wstring& strLayerTag)
{
	//FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_INTRO, strLayerTag, TEXT("Prototype_GameObject_Particle_Blue")));
	//FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_INTRO, strLayerTag, TEXT("Prototype_GameObject_Particle_Red")));
	//FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_INTRO, strLayerTag, TEXT("Prototype_GameObject_Effect_Explosion")));

	return S_OK;
}

HRESULT CLevel_Intro::Ready_Layer_Monster(const wstring& strLayerTag, void* pArg)
{
	//FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_INTRO, strLayerTag, TEXT("Prototype_GameObject_Assassin")));
	//FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_INTRO, strLayerTag, TEXT("Prototype_GameObject_Infected")));
// 	for (int i = 0; i < 100; ++i)
// 	{
	FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_INTRO, strLayerTag, TEXT("Prototype_GameObject_VampireCommander")));
	/*	}*/

	{
		//CGameObject* pMonster = m_pGameInstance->Add_CloneObject_And_Get(LEVEL_INTRO, strLayerTag, TEXT("Prototype_GameObject_Infected_A"));
		//if (nullptr == pMonster)	return E_FAIL;
		//pMonster->Set_Position(_float3(0.f, 0.f, 2.f));
	}

	{
		CGameObject* pMonster = m_pGameInstance->Add_CloneObject_And_Get(LEVEL_INTRO, strLayerTag, TEXT("Prototype_GameObject_Bandit_Sniper"));
		if (nullptr == pMonster)	return E_FAIL;
		pMonster->Set_Position(_float3(1.f, 0.f, 2.f));
	}




	//CGameObject::GAMEOBJECT_DESC GameObjectDesc = *(CGameObject::GAMEOBJECT_DESC*)pArg;

	//CMonster::MONSTER_DESC Desc = {};
	//Desc.fRotationPerSec = GameObjectDesc.fRotationPerSec;
	//Desc.fSpeedPerSec = GameObjectDesc.fSpeedPerSec;
	//Desc.bPreview = false;
	//Desc.WorldMatrix = XMMatrixIdentity();

	//FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_INTRO, strLayerTag, TEXT("Prototype_GameObject_Monster"), &Desc));





	//!
	//!for (_int i = 0; i < iMonsterJsonSize; ++i)
	//!{
	//!	string pushMonsterTag = (string)MonsterJson[i]["PrototypeTag"] + "@" + to_string(i);
	//!
	//!	m_vecCreateMonsterTag.push_back(pushMonsterTag);
	//!
	//!	CMonster::MONSTER_DESC MonsterDesc;
	//!	MonsterDesc.bPreview = false;
	//!
	//!
	//!	const json& TransformJson = MonsterJson[i]["Component"]["Transform"];
	//!	_float4x4 WorldMatrix;
	//!
	//!	for (_int TransformLoopIndex = 0; TransformLoopIndex < 4; ++TransformLoopIndex)
	//!	{
	//!		for (_int TransformSecondLoopIndex = 0; TransformSecondLoopIndex < 4; ++TransformSecondLoopIndex)
	//!		{
	//!			WorldMatrix.m[TransformLoopIndex][TransformSecondLoopIndex] = TransformJson[TransformLoopIndex][TransformSecondLoopIndex];
	//!		}
	//!	}
	//!
	//!	MonsterDesc.WorldMatrix = WorldMatrix;
	//!
	//!	CMonster* pMonster = { nullptr };
	//!
	//!	wstring strProtoTypeTag;
	//!	m_pGameInstance->String_To_WString((string)MonsterJson[i]["PrototypeTag"], strProtoTypeTag);
	//!
	//!	pMonster = dynamic_cast<CMonster*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, L"Layer_Monster", strProtoTypeTag, &MonsterDesc));
	//!
	//!	m_vecCreateMonster.push_back(pMonster);
	//!	m_iCreateMonsterIndex++;
	//!}


	return S_OK;
}

HRESULT CLevel_Intro::Ready_Layer_BackGround(const wstring& strLayerTag)
{
	//FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_INTRO, strLayerTag, TEXT("Prototype_GameObject_Terrain")));
	FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_INTRO, strLayerTag, TEXT("Prototype_GameObject_Sky")));


	json Stage1MapJson = {};

	if (FAILED(CJson_Utility::Load_Json(m_strStage1MapLoadPath.c_str(), Stage1MapJson)))
	{
		MSG_BOX("맵 불러오기 실패");
		return E_FAIL;
	}

	json BasicJson = Stage1MapJson["Basic_Json"];
	_int iBasicJsonSize = (_int)BasicJson.size();

	for (_int i = 0; i < iBasicJsonSize; ++i)
	{
		CEnvironment_Object::ENVIRONMENT_OBJECT_DESC Desc;

		Desc.bAnimModel = BasicJson[i]["AnimType"];

		wstring strLoadModelTag;
		string strJsonModelTag = BasicJson[i]["ModelTag"];

		m_pGameInstance->String_To_WString(strJsonModelTag, strLoadModelTag);
		Desc.strModelTag = strLoadModelTag;

		Desc.iShaderPassIndex = BasicJson[i]["ShaderPassIndex"];
		Desc.iPlayAnimationIndex = BasicJson[i]["PlayAnimationIndex"];
		Desc.bPreview = false;

		const json& TransformJson = BasicJson[i]["Component"]["Transform"];
		_float4x4 WorldMatrix;

		for (_int TransformLoopIndex = 0; TransformLoopIndex < 4; ++TransformLoopIndex)
		{
			for (_int TransformSecondLoopIndex = 0; TransformSecondLoopIndex < 4; ++TransformSecondLoopIndex)
			{
				WorldMatrix.m[TransformLoopIndex][TransformSecondLoopIndex] = TransformJson[TransformLoopIndex][TransformSecondLoopIndex];
			}
		}

		XMStoreFloat4(&Desc.vPos, XMLoadFloat4x4(&WorldMatrix).r[3]);
		Desc.WorldMatrix = WorldMatrix;

		CEnvironment_Object* pObject = { nullptr };

		pObject = dynamic_cast<CEnvironment_Object*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_INTRO, L"Layer_BackGround", L"Prototype_GameObject_Environment_Object", &Desc));
	}


	json InteractJson = Stage1MapJson["Interact_Json"];
	_int InteractJsonSize = (_int)InteractJson.size();

	for (_int i = 0; i < InteractJsonSize; ++i)
	{

		//TODO 추후 상호작용 오브젝트 클래스 작성  후 작업
		//! L"Layer_Event"
	}

	json InstanceJson = Stage1MapJson["Instance_Json"];
	_int InstanceJsonSize = (_int)InstanceJson.size();

	for (_int i = 0; i < InstanceJsonSize; ++i)
	{
		MAPTOOL_INSTANCE_DESC InstanceDesc;

		InstanceDesc.iNumInstance = InstanceJson[i]["InstanceCount"];


		wstring strLoadModelTag;
		string strJsonModelTag = InstanceJson[i]["ModelTag"];

		m_pGameInstance->String_To_WString(strJsonModelTag, strLoadModelTag);
		InstanceDesc.strModelTag = strLoadModelTag;

		InstanceDesc.iShaderPassIndex = InstanceJson[i]["ShaderPassIndex"];

		json InstanceInfoJson = InstanceJson[i]["InstanceInfo_Json"];
		_uint InstanceInfoJsonSize = (_uint)InstanceInfoJson.size();

		for (_uint j = 0; j < InstanceInfoJsonSize; ++j)
		{
			INSTANCE_INFO_DESC InstanceInfoDesc = {};

			CJson_Utility::Load_Float3(InstanceInfoJson[j]["Instance_Scale"], InstanceInfoDesc.vScale);
			CJson_Utility::Load_Float4(InstanceInfoJson[j]["Instance_Rotation"], InstanceInfoDesc.vRotation);
			CJson_Utility::Load_Float3(InstanceInfoJson[j]["Instance_Translation"], InstanceInfoDesc.vTranslation);
			CJson_Utility::Load_Float3(InstanceInfoJson[j]["Instance_Center"], InstanceInfoDesc.vCenter);

			InstanceDesc.vecInstanceInfoDesc.push_back(InstanceInfoDesc);
		}


		CEnvironment_Instance* pInstanceObject = { nullptr };

		pInstanceObject = dynamic_cast<CEnvironment_Instance*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_INTRO, L"Layer_BackGround", L"Prototype_GameObject_Environment_Instance", &InstanceDesc));

	}

	json MonsterJson = Stage1MapJson["Monster_Json"];
	_int iMonsterJsonSize = (_int)MonsterJson.size();

	for (_int i = 0; i < iMonsterJsonSize; ++i)
	{
		CMonster::MONSTER_DESC MonsterDesc = {};

		string LoadMonsterTag = (string(MonsterJson[i]["PrototypeTag"]));

		m_pGameInstance->String_To_WString(LoadMonsterTag, MonsterDesc.strProtoTypeTag);
		MonsterDesc.bPreview = false;


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

		if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_INTRO, L"Layer_Monster", MonsterDesc.strProtoTypeTag, &MonsterDesc)))
			return E_FAIL;

	}


	return S_OK;

}

HRESULT CLevel_Intro::Ready_LandObjects()
{
	CLandObject::LANDOBJECT_DESC	LandObjectDesc{};

	LandObjectDesc.pTerrainBuffer = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Get_Component(LEVEL_INTRO, TEXT("Layer_BackGround"), TEXT("Com_VIBuffer")));
	LandObjectDesc.pTerrainTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_INTRO, TEXT("Layer_BackGround"), TEXT("Com_Transform")));

	FAILED_CHECK(Ready_Layer_Player(TEXT("Layer_Player"), &LandObjectDesc));
	FAILED_CHECK(Ready_Layer_Monster(TEXT("Layer_Monster"), &LandObjectDesc));

	FAILED_CHECK(Ready_Layer_Building(TEXT("Layer_Building"), &LandObjectDesc));

	return S_OK;
}

HRESULT CLevel_Intro::Ready_Layer_Building(const wstring& strLayerTag, void* pArg)
{
	//FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_INTRO, strLayerTag, TEXT("Prototype_GameObject_ForkLift"), pArg));

	return S_OK;
}

HRESULT CLevel_Intro::Ready_Layer_Test(const wstring& strLayerTag)
{
	//FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_INTRO, strLayerTag, TEXT("Prototype_GameObject_Interact_Chain")));
	//! 애님인스턴싱이 할필요없어짐. ㅎㅎㅎㅎㅎㅎFAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_INTRO, strLayerTag, TEXT("Prototype_GameObject_InstanceMonster")));

	//FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_INTRO, strLayerTag, TEXT("Prototype_GameObject_Screamer")));

	///* ui test */
	//m_pGameInstance->Add_CloneObject(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_UI_Player_HPBar"));

	//CUI_Player_HPBar::UI_DESC desc = {};
	//desc.fPositionX = (_float)g_iWinSizeX / 2 + 20.f;
	//desc.fPositionY = (_float)g_iWinSizeY / 2 + 20.f;
	//m_pGameInstance->Add_CloneObject(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_UI_Player_HPBar"), &desc);



	return S_OK;
}

HRESULT CLevel_Intro::Ready_UI()
{
	// FAILED_CHECK(Ready_Layer_UI_Monster(TEXT("Layer_UI_Monster"), nullptr));

	FAILED_CHECK(Ready_Layer_UI_Interface(TEXT("Layer_UI_Interface"), nullptr));

	FAILED_CHECK(Ready_Layer_UI(TEXT("Layer_UI"), nullptr));

	return S_OK;
}

HRESULT CLevel_Intro::Ready_Layer_UI_Monster(const wstring& strLayerTag, void* pArg)
{
	/* 추 후 파싱해서 정보 받아오기 */

	/* MRT로 묶지 않으면 출력이안나옴. */
	json json_in;

	char filePath[MAX_PATH] = "../Bin/DataFiles/Data_UI/UI_Info";

	_int		iPathNum = 0;
	string		strFileName;
	string		strFilePath;

	CJson_Utility::Load_Json(filePath, json_in);

	for (auto& item : json_in.items())
	{
		json object = item.value();

		CUI::UI_DESC tUI_Info;

		tUI_Info.strProtoTag = object["ProtoTag"];
		tUI_Info.strFilePath = object["FilePath"];

		wstring wstrPrototag;
		m_pGameInstance->String_To_WString(tUI_Info.strProtoTag, wstrPrototag);

		wstring wstrFilePath;
		m_pGameInstance->String_To_WString(tUI_Info.strFilePath, wstrFilePath);

		CUI_Anything* pUI_Object = dynamic_cast<CUI_Anything*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_UI_Anything"), &tUI_Info));

		pUI_Object->Get_Transform()->Load_FromJson(object);

		// Pos 잡아주기
	}


	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_UI_Anything"), &json_in)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Intro::Ready_Layer_UI_Interface(const wstring& strLayerTag, void* pArg)
{
	// =>Left_Interface
	Ready_LeftInterface(strLayerTag, pArg);
	// =>Right_Interface
	Ready_RightInterface(strLayerTag, pArg);
	// =>Quest_Box
	Ready_QuestBox(strLayerTag, pArg);
	// =>Tutorial_Box
	Ready_TutorialBox(strLayerTag, pArg);
	// =>LevelUp
	Ready_LevelUp(strLayerTag, pArg);
	// =>Reward_Item
	Ready_Reward_Item(strLayerTag, pArg);
	return S_OK;
}

HRESULT CLevel_Intro::Ready_LeftInterface(const wstring& strLayerTag, void* pArg)
{
	json json_in;

	char filePath[MAX_PATH] = "../Bin/DataFiles/Data_UI/PlayerInterface/Left_Interface.json";

	_int		iPathNum = 0;
	string		strFileName;
	string		strFilePath;

	CJson_Utility::Load_Json(filePath, json_in);

	for (auto& item : json_in.items())
	{
		json object = item.value();

		CUI::UI_DESC tUI_Info;

		/* 저장순서랑 맞는지 확인하기 */
		tUI_Info.bParent = object["Parent"];					// 1. Parent
		tUI_Info.bWorld = object["World"];						// 2. World
		tUI_Info.bGroup = object["Group"];						// 3. Group
		tUI_Info.fAlpha = object["Alpha"];						// 4. Alpha
		tUI_Info.iObjectNum = object["ObjectNum"];				// 5. ObjectNum
		tUI_Info.iShaderNum = object["ShaderNum"];				// 6. ShaderPathNum
		tUI_Info.strObjectName = object["ObjectName"];			// 7. ObjectName
		tUI_Info.strLayerTag = object["LayerTag"];				// 8. LayerTag
		tUI_Info.strCloneTag = object["CloneTag"];				// 9. CloneTag
		tUI_Info.strProtoTag = object["ProtoTag"];				// 10. ProtoTag
		tUI_Info.strFilePath = object["FilePath"];				// 11. FilePath
		tUI_Info.strMapTextureTag = object["MapTextureTag"];	// 12. MapTexture
		tUI_Info.vColor.m128_f32[0] = object["ColorR"];			// 13. R
		tUI_Info.vColor.m128_f32[1] = object["ColorG"];			// 14. G
		tUI_Info.vColor.m128_f32[2] = object["ColorB"];			// 15. B
		tUI_Info.vColor.m128_f32[3] = object["ColorA"];			// 16. A

		wstring wstrLayer;
		m_pGameInstance->String_To_WString(tUI_Info.strLayerTag, wstrLayer); //

		wstring wstrClonetag;
		m_pGameInstance->String_To_WString(tUI_Info.strCloneTag, wstrClonetag);

		wstring wstrPrototag;
		m_pGameInstance->String_To_WString(tUI_Info.strProtoTag, wstrPrototag);

		wstring wstrFilePath;
		m_pGameInstance->String_To_WString(tUI_Info.strFilePath, wstrFilePath);

		CUI* pUI_Object = dynamic_cast<CUI*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_STATIC, strLayerTag, wstrClonetag, &tUI_Info));

		pUI_Object->Get_Transform()->Load_FromJson(object);		// 17. TransformCom
	}

	return S_OK;
}

HRESULT CLevel_Intro::Ready_RightInterface(const wstring& strLayerTag, void* pArg)
{
	json json_in;

	char filePath[MAX_PATH] = "../Bin/DataFiles/Data_UI/PlayerInterface/Right_Interface.json";

	_int		iPathNum = 0;
	string		strFileName;
	string		strFilePath;

	CJson_Utility::Load_Json(filePath, json_in);

	for (auto& item : json_in.items())
	{
		json object = item.value();

		CUI::UI_DESC tUI_Info;

		/* 저장순서랑 맞는지 확인하기 */
		tUI_Info.bParent = object["Parent"];					// 1. Parent
		tUI_Info.bWorld = object["World"];						// 2. World
		tUI_Info.bGroup = object["Group"];						// 3. Group
		tUI_Info.fAlpha = object["Alpha"];						// 4. Alpha
		tUI_Info.iObjectNum = object["ObjectNum"];				// 5. ObjectNum
		tUI_Info.iShaderNum = object["ShaderNum"];				// 6. ShaderPathNum
		tUI_Info.strObjectName = object["ObjectName"];			// 7. ObjectName
		tUI_Info.strLayerTag = object["LayerTag"];				// 8. LayerTag
		tUI_Info.strCloneTag = object["CloneTag"];				// 9. CloneTag
		tUI_Info.strProtoTag = object["ProtoTag"];				// 10. ProtoTag
		tUI_Info.strFilePath = object["FilePath"];				// 11. FilePath
		tUI_Info.strMapTextureTag = object["MapTextureTag"];	// 12. MapTexture
		tUI_Info.vColor.m128_f32[0] = object["ColorR"];			// 13. R
		tUI_Info.vColor.m128_f32[1] = object["ColorG"];			// 14. G
		tUI_Info.vColor.m128_f32[2] = object["ColorB"];			// 15. B
		tUI_Info.vColor.m128_f32[3] = object["ColorA"];			// 16. A

		wstring wstrLayer;
		m_pGameInstance->String_To_WString(tUI_Info.strLayerTag, wstrLayer); //

		wstring wstrClonetag;
		m_pGameInstance->String_To_WString(tUI_Info.strCloneTag, wstrClonetag);

		wstring wstrPrototag;
		m_pGameInstance->String_To_WString(tUI_Info.strProtoTag, wstrPrototag);

		wstring wstrFilePath;
		m_pGameInstance->String_To_WString(tUI_Info.strFilePath, wstrFilePath);

		CUI* pUI_Object = dynamic_cast<CUI*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_STATIC, strLayerTag, wstrClonetag, &tUI_Info));

		pUI_Object->Get_Transform()->Load_FromJson(object);		// 17. TransformCom
	}

	return S_OK;
}

HRESULT CLevel_Intro::Ready_QuestBox(const wstring& strLayerTag, void* pArg)
{
	json json_in;

	char filePath[MAX_PATH] = "../Bin/DataFiles/Data_UI/PlayerInterface/QuestBox.json";

	_int		iPathNum = 0;
	string		strFileName;
	string		strFilePath;

	CJson_Utility::Load_Json(filePath, json_in);

	for (auto& item : json_in.items())
	{
		json object = item.value();

		CUI::UI_DESC tUI_Info;

		/* 저장순서랑 맞는지 확인하기 */
		tUI_Info.bParent = object["Parent"];					// 1. Parent
		tUI_Info.bWorld = object["World"];						// 2. World
		tUI_Info.bGroup = object["Group"];						// 3. Group
		tUI_Info.fAlpha = object["Alpha"];						// 4. Alpha
		tUI_Info.iObjectNum = object["ObjectNum"];				// 5. ObjectNum
		tUI_Info.iShaderNum = object["ShaderNum"];				// 6. ShaderPathNum
		tUI_Info.strObjectName = object["ObjectName"];			// 7. ObjectName
		tUI_Info.strLayerTag = object["LayerTag"];				// 8. LayerTag
		tUI_Info.strCloneTag = object["CloneTag"];				// 9. CloneTag
		tUI_Info.strProtoTag = object["ProtoTag"];				// 10. ProtoTag
		tUI_Info.strFilePath = object["FilePath"];				// 11. FilePath
		tUI_Info.strMapTextureTag = object["MapTextureTag"];	// 12. MapTexture
		tUI_Info.vColor.m128_f32[0] = object["ColorR"];			// 13. R
		tUI_Info.vColor.m128_f32[1] = object["ColorG"];			// 14. G
		tUI_Info.vColor.m128_f32[2] = object["ColorB"];			// 15. B
		tUI_Info.vColor.m128_f32[3] = object["ColorA"];			// 16. A

		wstring wstrLayer;
		m_pGameInstance->String_To_WString(tUI_Info.strLayerTag, wstrLayer); //

		wstring wstrClonetag;
		m_pGameInstance->String_To_WString(tUI_Info.strCloneTag, wstrClonetag);

		wstring wstrPrototag;
		m_pGameInstance->String_To_WString(tUI_Info.strProtoTag, wstrPrototag);

		wstring wstrFilePath;
		m_pGameInstance->String_To_WString(tUI_Info.strFilePath, wstrFilePath);

		CUI* pUI_Object = dynamic_cast<CUI*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_STATIC, strLayerTag, wstrClonetag, &tUI_Info));

		pUI_Object->Get_Transform()->Load_FromJson(object);		// 17. TransformCom
	}

	return S_OK;
}

HRESULT CLevel_Intro::Ready_TutorialBox(const wstring& strLayerTag, void* pArg)
{
	json json_in;

	char filePath[MAX_PATH] = "../Bin/DataFiles/Data_UI/PlayerInterface/TutorialBox.json";

	_int		iPathNum = 0;
	string		strFileName;
	string		strFilePath;

	CJson_Utility::Load_Json(filePath, json_in);

	for (auto& item : json_in.items())
	{
		json object = item.value();

		CUI::UI_DESC tUI_Info;

		/* 저장순서랑 맞는지 확인하기 */
		tUI_Info.bParent = object["Parent"];					// 1. Parent
		tUI_Info.bWorld = object["World"];						// 2. World
		tUI_Info.bGroup = object["Group"];						// 3. Group
		tUI_Info.fAlpha = object["Alpha"];						// 4. Alpha
		tUI_Info.iObjectNum = object["ObjectNum"];				// 5. ObjectNum
		tUI_Info.iShaderNum = object["ShaderNum"];				// 6. ShaderPathNum
		tUI_Info.strObjectName = object["ObjectName"];			// 7. ObjectName
		tUI_Info.strLayerTag = object["LayerTag"];				// 8. LayerTag
		tUI_Info.strCloneTag = object["CloneTag"];				// 9. CloneTag
		tUI_Info.strProtoTag = object["ProtoTag"];				// 10. ProtoTag
		tUI_Info.strFilePath = object["FilePath"];				// 11. FilePath
		tUI_Info.strMapTextureTag = object["MapTextureTag"];	// 12. MapTexture
		tUI_Info.vColor.m128_f32[0] = object["ColorR"];			// 13. R
		tUI_Info.vColor.m128_f32[1] = object["ColorG"];			// 14. G
		tUI_Info.vColor.m128_f32[2] = object["ColorB"];			// 15. B
		tUI_Info.vColor.m128_f32[3] = object["ColorA"];			// 16. A

		wstring wstrLayer;
		m_pGameInstance->String_To_WString(tUI_Info.strLayerTag, wstrLayer); //

		wstring wstrClonetag;
		m_pGameInstance->String_To_WString(tUI_Info.strCloneTag, wstrClonetag);

		wstring wstrPrototag;
		m_pGameInstance->String_To_WString(tUI_Info.strProtoTag, wstrPrototag);

		wstring wstrFilePath;
		m_pGameInstance->String_To_WString(tUI_Info.strFilePath, wstrFilePath);

		CUI* pUI_Object = dynamic_cast<CUI*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_STATIC, strLayerTag, wstrClonetag, &tUI_Info));

		pUI_Object->Get_Transform()->Load_FromJson(object);		// 17. TransformCom
	}

	return S_OK;
}

HRESULT CLevel_Intro::Ready_LevelUp(const wstring& strLayerTag, void* pArg)
{
	json json_in;

	char filePath[MAX_PATH] = "../Bin/DataFiles/Data_UI/PlayerInterface/LevelUp.json";

	_int		iPathNum = 0;
	string		strFileName;
	string		strFilePath;

	CJson_Utility::Load_Json(filePath, json_in);

	for (auto& item : json_in.items())
	{
		json object = item.value();

		CUI::UI_DESC tUI_Info;

		/* 저장순서랑 맞는지 확인하기 */
		tUI_Info.bParent = object["Parent"];					// 1. Parent
		tUI_Info.bWorld = object["World"];						// 2. World
		tUI_Info.bGroup = object["Group"];						// 3. Group
		tUI_Info.fAlpha = object["Alpha"];						// 4. Alpha
		tUI_Info.iObjectNum = object["ObjectNum"];				// 5. ObjectNum
		tUI_Info.iShaderNum = object["ShaderNum"];				// 6. ShaderPathNum
		tUI_Info.strObjectName = object["ObjectName"];			// 7. ObjectName
		tUI_Info.strLayerTag = object["LayerTag"];				// 8. LayerTag
		tUI_Info.strCloneTag = object["CloneTag"];				// 9. CloneTag
		tUI_Info.strProtoTag = object["ProtoTag"];				// 10. ProtoTag
		tUI_Info.strFilePath = object["FilePath"];				// 11. FilePath
		tUI_Info.strMapTextureTag = object["MapTextureTag"];	// 12. MapTexture
		tUI_Info.vColor.m128_f32[0] = object["ColorR"];			// 13. R
		tUI_Info.vColor.m128_f32[1] = object["ColorG"];			// 14. G
		tUI_Info.vColor.m128_f32[2] = object["ColorB"];			// 15. B
		tUI_Info.vColor.m128_f32[3] = object["ColorA"];			// 16. A

		wstring wstrLayer;
		m_pGameInstance->String_To_WString(tUI_Info.strLayerTag, wstrLayer); //

		wstring wstrClonetag;
		m_pGameInstance->String_To_WString(tUI_Info.strCloneTag, wstrClonetag);

		wstring wstrPrototag;
		m_pGameInstance->String_To_WString(tUI_Info.strProtoTag, wstrPrototag);

		wstring wstrFilePath;
		m_pGameInstance->String_To_WString(tUI_Info.strFilePath, wstrFilePath);

		CUI* pUI_Object = dynamic_cast<CUI*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_STATIC, strLayerTag, wstrClonetag, &tUI_Info));

		pUI_Object->Get_Transform()->Load_FromJson(object);		// 17. TransformCom
	}

	return S_OK;
}

HRESULT CLevel_Intro::Ready_Reward_Item(const wstring& strLayerTag, void* pArg)
{
	json json_in;

	char filePath[MAX_PATH] = "../Bin/DataFiles/Data_UI/PlayerInterface/RewardItem.json";

	_int		iPathNum = 0;
	string		strFileName;
	string		strFilePath;

	CJson_Utility::Load_Json(filePath, json_in);

	for (auto& item : json_in.items())
	{
		json object = item.value();

		CUI::UI_DESC tUI_Info;

		/* 저장순서랑 맞는지 확인하기 */
		tUI_Info.bParent = object["Parent"];					// 1. Parent
		tUI_Info.bWorld = object["World"];						// 2. World
		tUI_Info.bGroup = object["Group"];						// 3. Group
		tUI_Info.fAlpha = object["Alpha"];						// 4. Alpha
		tUI_Info.iObjectNum = object["ObjectNum"];				// 5. ObjectNum
		tUI_Info.iShaderNum = object["ShaderNum"];				// 6. ShaderPathNum
		tUI_Info.strObjectName = object["ObjectName"];			// 7. ObjectName
		tUI_Info.strLayerTag = object["LayerTag"];				// 8. LayerTag
		tUI_Info.strCloneTag = object["CloneTag"];				// 9. CloneTag
		tUI_Info.strProtoTag = object["ProtoTag"];				// 10. ProtoTag
		tUI_Info.strFilePath = object["FilePath"];				// 11. FilePath
		tUI_Info.strMapTextureTag = object["MapTextureTag"];	// 12. MapTexture
		tUI_Info.vColor.m128_f32[0] = object["ColorR"];			// 13. R
		tUI_Info.vColor.m128_f32[1] = object["ColorG"];			// 14. G
		tUI_Info.vColor.m128_f32[2] = object["ColorB"];			// 15. B
		tUI_Info.vColor.m128_f32[3] = object["ColorA"];			// 16. A

		wstring wstrLayer;
		m_pGameInstance->String_To_WString(tUI_Info.strLayerTag, wstrLayer); //

		wstring wstrClonetag;
		m_pGameInstance->String_To_WString(tUI_Info.strCloneTag, wstrClonetag);

		wstring wstrPrototag;
		m_pGameInstance->String_To_WString(tUI_Info.strProtoTag, wstrPrototag);

		wstring wstrFilePath;
		m_pGameInstance->String_To_WString(tUI_Info.strFilePath, wstrFilePath);

		CUI* pUI_Object = dynamic_cast<CUI*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_STATIC, strLayerTag, wstrClonetag, &tUI_Info));

		pUI_Object->Get_Transform()->Load_FromJson(object);		// 17. TransformCom
	}

	return S_OK;
}

HRESULT CLevel_Intro::Ready_Layer_UI(const wstring& strLayerTag, void* pArg)
{
	return S_OK;
}

CLevel_Intro* CLevel_Intro::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)

{
	CLevel_Intro* pInstance = new CLevel_Intro(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Intro");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Intro::Free()
{
	__super::Free();

}
