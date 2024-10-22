#include "stdafx.h"
#include "Level_GamePlay.h"
#include "GameInstance.h"
#include "Player.h"
#include "Camera_Dynamic.h"
#include "Environment_Instance.h"
#include "Effect_Instance.h"
#include "Data_Manager.h"

#pragma region UI
#include "UI_Player_HPBar.h"
#include "UI_Anything.h"
#include "UI_MonsterHp.h"
#include "UI_MonsterHpFrame.h"
#include "UI_Manager.h"
#pragma endregion

#include "LandObject.h"
#include "Monster_Character.h"

#pragma region MAP
#include "Environment_Object.h"
#include "Environment_Instance.h"
#include "Environment_Interact.h"
#include "Environment_LightObject.h"
#include "Environment_SpecialObject.h"
#include "Event_Trigger.h"
#include "Event_MonsterSpawnTrigger.h"
#include "Event_UITrigger.h"
#include "Light.h"
#include "AnimalObject.h"
#pragma endregion

#pragma region Test
#include "Monster.h"
#include "Screamer.h"  
#include "InstanceMonster.h"
#pragma endregion


#include "Level_Loading.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	m_pGameInstance->Get_Renderer()->Render_UI_MRT(false);
	m_pGameInstance->Set_CurrentLevel(m_pGameInstance->Get_NextLevel());
	Set_ShaderOption("../Bin/DataFiles/Data_Shader/Level/Level_Intro_Shader.json");

	FAILED_CHECK(Ready_LightDesc());
	FAILED_CHECK(Ready_Layer_Player(TEXT("Layer_Player")));
	FAILED_CHECK(Ready_Layer_Monster(TEXT("Layer_Monster")));
	FAILED_CHECK(Ready_Layer_BackGround(TEXT("Layer_BackGround")));
	FAILED_CHECK(Ready_Layer_Camera(TEXT("Layer_Camera")));
	FAILED_CHECK(Ready_Layer_Test(TEXT("Layer_Test")));
	FAILED_CHECK(Ready_UI());
	FAILED_CHECK(Ready_Event());

	return S_OK;
}

void CLevel_GamePlay::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_GRAVE))
	{
		m_pGameInstance->Request_Level_Opening(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_INTRO_BOSS));
	}

}

HRESULT CLevel_GamePlay::Render()
{
	SetWindowText(g_hWnd, TEXT("惟績巴傾戚傾婚脊艦陥."));
	
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_LightDesc()
{
	/* Shadow Light */
	m_pGameInstance->Add_ShadowLight_View(ECast(LEVEL::LEVEL_GAMEPLAY), _float4(Engine::g_vLightEye), _float4(Engine::g_vLightAt), _float4(Engine::g_vLightUp));
	m_pGameInstance->Add_ShadowLight_Proj(ECast(LEVEL::LEVEL_GAMEPLAY), 60.f, (_float)g_iWinSizeX / (_float)g_iWinSizeY, Engine::g_fLightNear, Engine::g_fLightFar);

	/* Map Light */
	CLight* pDirectionalLight = m_pGameInstance->Get_DirectionLight();

	if (pDirectionalLight != nullptr) //TODO 奄糎拭 巨刑偲確 虞戚闘亜 糎仙梅陥檎.
	{
		m_pGameInstance->Remove_Light(pDirectionalLight->Get_LightIndex());
	}

	json Stage1MapJson = {};     

	if (FAILED(CJson_Utility::Load_Json(m_strStage1MapLoadPath.c_str(), Stage1MapJson)))
	{
		MSG_BOX("繕誤 災君神奄 叔鳶");
		return E_FAIL;
	}

	json LightJson = Stage1MapJson["Light_Json"];
	_int iLightJsonSize = (_int)LightJson.size();

	for (_int i = 0; i < iLightJsonSize; ++i)
	{
		LIGHT_DESC LightDesc = {};

		LightDesc.iLightIndex = LightJson[i]["LightIndex"];
		LightDesc.bEnable = LightJson[i]["LightEnable"];
		LightDesc.fCutOff = LightJson[i]["CutOff"];
		LightDesc.fOuterCutOff = LightJson[i]["OuterCutOff"];
		LightDesc.fIntensity = LightJson[i]["Intensity"]; // �� 食奄 蓄亜喫 

		LightDesc.eType = LightJson[i]["Type"];
		CJson_Utility::Load_Float4(LightJson[i]["Direction"], LightDesc.vDirection);
		LightDesc.fRange = LightJson[i]["Range"];
		CJson_Utility::Load_Float4(LightJson[i]["Position"], LightDesc.vPosition);
		CJson_Utility::Load_Float4(LightJson[i]["Diffuse"], LightDesc.vDiffuse);
		CJson_Utility::Load_Float4(LightJson[i]["Specular"], LightDesc.vSpecular);
		CJson_Utility::Load_Float4(LightJson[i]["Ambient"], LightDesc.vAmbient);


		if (LightDesc.eType == tagLightDesc::TYPE_DIRECTIONAL)
		{
			CLight* pDirectionLight = m_pGameInstance->Get_DirectionLight();

			if (pDirectionLight != nullptr)
			{
				m_pGameInstance->Remove_Light(pDirectionLight->Get_LightIndex());

			}
		}

		CLight* pLight = m_pGameInstance->Add_Light_AndGet(LightDesc, LightDesc.iLightIndex);

		if (pLight == nullptr)
		{
			MSG_BOX("虞戚闘 災君神奄 叔鳶");
			return E_FAIL;
		}

	}

	json LightObjectJson = Stage1MapJson["LightObject_Json"];
	_int iLightObjectJsonSize = (_int)LightObjectJson.size();

	for (_int i = 0; i < iLightObjectJsonSize; ++i)
	{
		CEnvironment_LightObject::ENVIRONMENT_LIGHTOBJECT_DESC LightObjectDesc = {};

		LightObjectDesc.bAnimModel = LightObjectJson[i]["AnimType"];
		LightObjectDesc.bEffect = LightObjectJson[i]["Effect"];
		LightObjectDesc.eLightEffect = LightObjectJson[i]["EffectType"];
		LightObjectDesc.iPlayAnimationIndex = LightObjectJson[i]["PlayAnimationIndex"];
		LightObjectDesc.iShaderPassIndex = LightObjectJson[i]["ShaderPassIndex"];
		LightObjectDesc.bPreview = false;

		m_pGameInstance->String_To_WString((string)LightObjectJson[i]["ModelTag"], LightObjectDesc.strModelTag);

		const json& TransformJson = LightObjectJson[i]["Component"]["Transform"];
		_float4x4 WorldMatrix;

		for (_int TransformLoopIndex = 0; TransformLoopIndex < 4; ++TransformLoopIndex)
		{
			for (_int TransformSecondLoopIndex = 0; TransformSecondLoopIndex < 4; ++TransformSecondLoopIndex)
			{
				WorldMatrix.m[TransformLoopIndex][TransformSecondLoopIndex] = TransformJson[TransformLoopIndex][TransformSecondLoopIndex];
			}
		}

		LightObjectDesc.WorldMatrix = WorldMatrix;

		LIGHT_DESC LightDesc = {};

		LightDesc.iLightIndex = LightObjectJson[i]["LightIndex"];
		LightDesc.bEnable = LightObjectJson[i]["LightEnable"];
		LightDesc.fCutOff = LightObjectJson[i]["CutOff"];
		LightDesc.fOuterCutOff = LightObjectJson[i]["OuterCutOff"];
		LightDesc.fIntensity = LightObjectJson[i]["Intensity"]; // �� 食奄 蓄亜喫 

		LightDesc.eType = LightObjectJson[i]["LightType"];
		CJson_Utility::Load_Float4(LightObjectJson[i]["Direction"], LightDesc.vDirection);
		LightDesc.fRange = LightObjectJson[i]["Range"];
		CJson_Utility::Load_Float4(LightObjectJson[i]["Position"], LightDesc.vPosition);
		CJson_Utility::Load_Float4(LightObjectJson[i]["Diffuse"], LightDesc.vDiffuse);
		CJson_Utility::Load_Float4(LightObjectJson[i]["Ambient"], LightDesc.vAmbient);
		CJson_Utility::Load_Float4(LightObjectJson[i]["Specular"], LightDesc.vSpecular);


		LightObjectDesc.LightDesc = LightDesc;

		CEnvironment_LightObject* pLightObject = dynamic_cast<CEnvironment_LightObject*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, L"Layer_BackGround", L"Prototype_GameObject_Environment_LightObject", &LightObjectDesc));

		if (pLightObject == nullptr)
		{
			MSG_BOX("虞戚闘神崎詮闘 持失叔鳶");
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const wstring & strLayerTag)
{
	//CCamera_Dynamic::DYNAMIC_CAMERA_DESC		Desc = {};

	//Desc.fMouseSensor = 0.05f;
	//Desc.vEye = _float4(0.f, 20.f, -15.f, 1.f);
	//Desc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	//Desc.fFovy = XMConvertToRadians(60.0f);wwwwwDdssaw
	//Desc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	//Desc.fNear = 0.1f;
	//Desc.fFar = m_pGameInstance->Get_CamFar();
	//Desc.fSpeedPerSec = 15.f;
	//Desc.fRotationPerSec = XMConvertToRadians(180.0f);

	//FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Camera_Dynamic"), &Desc));

	FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_MasterCamera")));

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const wstring & strLayerTag)
{
	FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Player")));

	CPlayer* pPlayer = CData_Manager::GetInstance()->Get_Player();

	pPlayer->Set_InitPosition(_float3(250.66f, 0.f, 2.38f));
	//pPlayer->Set_Position(_float3(250.66f, 0.f, 2.38f));
	////pPlayer->Set_Position(_float3(153.6f, 0.f, 150.55f)); /* Sniper 蒋 */
	//
	//
	//CNavigation* pNavigation = pPlayer->Get_Navigation();
	//
	//
	//pNavigation->Set_CurrentIndex(pNavigation->Get_SelectRangeCellIndex(pPlayer));

	//FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Rentier"));

	//CGameObject* pPlayer = m_pGameInstance->Add_CloneObject_And_Get(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Player"));
	//NULL_CHECK_RETURN(pPlayer, E_FAIL);
	//m_pGameInstance->Set_Player(dynamic_cast<CCharacter*>(pPlayer));

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Effect(const wstring & strLayerTag)
{
	//FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Particle_Blue")));
	//FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Particle_Red")));
	//FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Effect_Explosion")));

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const wstring & strLayerTag)
{

	//TODO 闘軒暗拭辞 災君臣暗績 戚薦
// 	json Stage1MonsterJson = {};
// 
// 	if (FAILED(CJson_Utility::Load_Json(m_strStage1MapLoadPath.c_str(), Stage1MonsterJson)))
// 	{
// 		MSG_BOX("佼什斗 災君神奄 叔鳶");
// 		return E_FAIL;
// 	}
// 
// 	json MonsterJson = Stage1MonsterJson["Monster_Json"];
// 	_int iMonsterJsonSize = (_int)MonsterJson.size();
// 
// 	for (_int i = 0; i < iMonsterJsonSize; ++i)
// 	{
// 		CMonster_Character::MONSTER_DESC MonsterDesc = {};
// 
// 		string LoadMonsterTag = (string(MonsterJson[i]["PrototypeTag"]));
// 
// 		m_pGameInstance->String_To_WString(LoadMonsterTag, MonsterDesc.strProtoTypeTag);
// 		MonsterDesc.bPreview = false;
// 		MonsterDesc.eDescType = CGameObject::MONSTER_DESC;
// 
// 		const json& TransformJson = MonsterJson[i]["Component"]["Transform"];
// 		_float4x4 WorldMatrix;
// 
// 		for (_int TransformLoopIndex = 0; TransformLoopIndex < 4; ++TransformLoopIndex)
// 		{
// 			for (_int TransformSecondLoopIndex = 0; TransformSecondLoopIndex < 4; ++TransformSecondLoopIndex)
// 			{
// 				WorldMatrix.m[TransformLoopIndex][TransformSecondLoopIndex] = TransformJson[TransformLoopIndex][TransformSecondLoopIndex];
// 			}
// 		}
// 
// 		MonsterDesc.WorldMatrix = WorldMatrix;
// 
// 		if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, L"Layer_Monster", MonsterDesc.strProtoTypeTag, &MonsterDesc)))
// 			return E_FAIL;
// 
// 	}

	////FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_VampireCommander")));
	//CGameObject* pMonster = nullptr;

	///* -- Monster -----------------------------*/
	////CGameObject* pMonster = m_pGameInstance->Add_CloneObject_And_Get(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Screamer"));
	////NULL_CHECK_RETURN(pMonster, E_FAIL);
	////pMonster->Set_Position(_float3(250.5, 0.f, 20.f));
	//
	//pMonster = m_pGameInstance->Add_CloneObject_And_Get(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Infected_A"));
	//NULL_CHECK_RETURN(pMonster, E_FAIL);
	//pMonster->Set_InitPosition(_float3(250.5, 0.f, 5.f));
	//
	//pMonster = m_pGameInstance->Add_CloneObject_And_Get(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Infected_B"));
	//NULL_CHECK_RETURN(pMonster, E_FAIL);
	//pMonster->Set_InitPosition(_float3(251.f, 0.f, 7.f));
	//
	//pMonster = m_pGameInstance->Add_CloneObject_And_Get(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Infected_C"));
	//NULL_CHECK_RETURN(pMonster, E_FAIL);
	//pMonster->Set_InitPosition(_float3(252.5f, 0.f, 9.f));	
	//
	//pMonster = m_pGameInstance->Add_CloneObject_And_Get(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Bandit_Sniper"));
	//NULL_CHECK_RETURN(pMonster, E_FAIL);
	////pMonster->Set_InitPosition(_float3(253.5f, 0.f, 11.f));
	//pMonster->Set_InitPosition(_float3(161.5f, 14.65f, 215.5f));

	//pMonster = m_pGameInstance->Add_CloneObject_And_Get(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Bandit_Sniper"));
	//NULL_CHECK_RETURN(pMonster, E_FAIL);
	//pMonster->Set_InitPosition(_float3(153.6f, 14.65f, 217.55f));

	return S_OK;
}
HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const wstring& strLayerTag)
{
	FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Sky")));

	json Stage1MapJson = {};

	if (FAILED(CJson_Utility::Load_Json(m_strStage1MapLoadPath.c_str(), Stage1MapJson)))
	{
		MSG_BOX("己 災君神奄 叔鳶");
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


		string strJsonObjectTag = BasicJson[i]["ObjectTag"];

		if ("Prototype_Component_Model_TNTCrate" == strJsonModelTag || "TNTCrate" == strJsonObjectTag)
		{
			CGameObject* pObject = m_pGameInstance->Add_CloneObject_And_Get(LEVEL_GAMEPLAY, L"Layer_BackGround", L"Prototype_GameObject_DestructableProps_TNTCrate");
			pObject->Set_WorldMatrix(WorldMatrix);
		}
		else if ("Prototype_Component_Model_Crane" == strJsonModelTag || "Crane" == strJsonObjectTag)
		{
			CGameObject* pObject = m_pGameInstance->Add_CloneObject_And_Get(LEVEL_GAMEPLAY, L"Layer_BackGround", L"Prototype_GameObject_Crane");
			pObject->Set_WorldMatrix(WorldMatrix);
		}
		else
		{
			CEnvironment_Object* pObject = { nullptr };

			pObject = dynamic_cast<CEnvironment_Object*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_GAMEPLAY, L"Layer_BackGround", L"Prototype_GameObject_Environment_Object", &Desc));

		}

	}


	json InteractJson = Stage1MapJson["Interact_Json"];
	_int InteractJsonSize = (_int)InteractJson.size();


	for (_int i = 0; i < InteractJsonSize; ++i)
	{

		CEnvironment_Interact::ENVIRONMENT_INTERACTOBJECT_DESC Desc = {};

		Desc.bAnimModel = InteractJson[i]["AnimType"];

		wstring strLoadModelTag;
		string strJsonModelTag = InteractJson[i]["ModelTag"];
		m_pGameInstance->String_To_WString(strJsonModelTag, strLoadModelTag);
		Desc.strModelTag = strLoadModelTag;
		Desc.bPreview = false;
		Desc.iPlayAnimationIndex = InteractJson[i]["PlayAnimationIndex"];
		Desc.iShaderPassIndex = InteractJson[i]["ShaderPassIndex"];
		Desc.eInteractState = InteractJson[i]["InteractState"];
		Desc.eInteractType = InteractJson[i]["InteractType"];
		Desc.bLevelChange = InteractJson[i]["LevelChange"];
		//Desc.bLevelChange = false;
		Desc.eChangeLevel = (LEVEL)InteractJson[i]["InteractLevel"];

		Desc.strSplineJsonPath = InteractJson[i]["SplineJsonPath"];
		Desc.bEnable = InteractJson[i]["Enable"];
		Desc.strEnableJsonPath = InteractJson[i]["EnableJsonPath"];
		Desc.iInteractGroupIndex = InteractJson[i]["InteractGroupIndex"];
		Desc.bOffset = InteractJson[i]["Offset"];
		Desc.bOwner = InteractJson[i]["Owner"];
		Desc.bRootTranslate = InteractJson[i]["RootTranslate"];
		Desc.bRotate = InteractJson[i]["Rotate"];
		Desc.fRotationAngle = InteractJson[i]["RotationAngle"];
		Desc.fRotationSpeed = InteractJson[i]["RotationSpeed"];
		Desc.eRotationState = InteractJson[i]["RotationType"];
		Desc.bArrival = InteractJson[i]["Arrival"];
		Desc.bInteractMoveMode = InteractJson[i]["InteractMove"];
		Desc.iLadderCount = InteractJson[i]["InteractLadderCount"];
		Desc.iReverseLadderCount = InteractJson[i]["InteractReverseLadderCount"];
		//Desc.iSwitchIndex = InteractJson[i]["LeverSwitchIndex"];
		//Desc.iArrivalCellIndex = InteractJson[i]["iArrivalCellIndex"];

		Desc.bUseGravity = InteractJson[i]["UseGravity"];
		CJson_Utility::Load_Float3(InteractJson[i]["RootMoveRate"], Desc.vPlayerRootMoveRate);
		CJson_Utility::Load_Float3(InteractJson[i]["ReverseRootMoveRate"], Desc.vPlayerReverseRootMoveRate);

		CJson_Utility::Load_Float3(InteractJson[i]["ColliderSize"], Desc.vBodyColliderSize);
		CJson_Utility::Load_Float3(InteractJson[i]["ColliderCenter"], Desc.vBodyColliderCenter);
		CJson_Utility::Load_Float3(InteractJson[i]["InteractColliderSize"], Desc.vInteractColliderSize);
		CJson_Utility::Load_Float3(InteractJson[i]["InteractColliderCenter"], Desc.vInteractColliderCenter);

		CJson_Utility::Load_Float3(InteractJson[i]["MoveColliderSize"], Desc.vMoveRangeColliderSize);
		CJson_Utility::Load_Float3(InteractJson[i]["MoveColliderCenter"], Desc.vMoveRangeColliderCenter);
		CJson_Utility::Load_Float3(InteractJson[i]["InteractMoveColliderSize"], Desc.vInteractMoveColliderSize);
		CJson_Utility::Load_Float3(InteractJson[i]["InteractMoveColliderCenter"], Desc.vInteractMoveColliderCenter);

		CJson_Utility::Load_Float4(InteractJson[i]["OffsetPosition"], Desc.vOffset);
		CJson_Utility::Load_Float4(InteractJson[i]["EnablePosition"], Desc.vEnablePosition);
		CJson_Utility::Load_Float4(InteractJson[i]["ArrivalPosition"], Desc.vArrivalPosition);

		const json& TransformJson = InteractJson[i]["Component"]["Transform"];
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

		json UpdateCellJson = InteractJson[i]["UpdateCellJson"];
		_int iUpdateCellJsonSize = (_int)UpdateCellJson.size();

		for (_int i = 0; i < iUpdateCellJsonSize; ++i)
		{
			Desc.vecUpdateCellIndex.push_back(UpdateCellJson[i]["UpdateCellIndex"]);
		}

		if (Desc.bOwner == false)
		{
			CEnvironment_Interact* pObject = { nullptr };

			pObject = dynamic_cast<CEnvironment_Interact*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_GAMEPLAY, L"Layer_BackGround", L"Prototype_GameObject_Environment_InteractObject", &Desc));

			if (Desc.eInteractType == CEnvironment_Interact::INTERACT_WAGONEVENT)
			{
				CData_Manager::GetInstance()->Set_SnowMountainWagon(pObject);
			}

		}
		else
			continue;
	}

	for (_int i = 0; i < InteractJsonSize; ++i)
	{

		CEnvironment_Interact::ENVIRONMENT_INTERACTOBJECT_DESC Desc = {};

		Desc.bAnimModel = InteractJson[i]["AnimType"];

		wstring strLoadModelTag;
		string strJsonModelTag = InteractJson[i]["ModelTag"];
		m_pGameInstance->String_To_WString(strJsonModelTag, strLoadModelTag);
		Desc.strModelTag = strLoadModelTag;
		Desc.bPreview = false;
		Desc.iPlayAnimationIndex = InteractJson[i]["PlayAnimationIndex"];
		Desc.iShaderPassIndex = InteractJson[i]["ShaderPassIndex"];
		Desc.eInteractState = InteractJson[i]["InteractState"];
		Desc.eInteractType = InteractJson[i]["InteractType"];
		Desc.bLevelChange = InteractJson[i]["LevelChange"];
		//Desc.bLevelChange = false;
		Desc.eChangeLevel = (LEVEL)InteractJson[i]["InteractLevel"];

		Desc.strSplineJsonPath = InteractJson[i]["SplineJsonPath"];
		Desc.bEnable = InteractJson[i]["Enable"];
		Desc.strEnableJsonPath = InteractJson[i]["EnableJsonPath"];
		Desc.iInteractGroupIndex = InteractJson[i]["InteractGroupIndex"];
		Desc.bOffset = InteractJson[i]["Offset"];
		Desc.bOwner = InteractJson[i]["Owner"];
		Desc.bRootTranslate = InteractJson[i]["RootTranslate"];
		Desc.bRotate = InteractJson[i]["Rotate"];
		Desc.fRotationAngle = InteractJson[i]["RotationAngle"];
		Desc.fRotationSpeed = InteractJson[i]["RotationSpeed"];
		Desc.eRotationState = InteractJson[i]["RotationType"];
		Desc.bArrival = InteractJson[i]["Arrival"];
		Desc.bInteractMoveMode = InteractJson[i]["InteractMove"];
		Desc.iLadderCount = InteractJson[i]["InteractLadderCount"];
		Desc.iReverseLadderCount = InteractJson[i]["InteractReverseLadderCount"];
		//Desc.iSwitchIndex = InteractJson[i]["LeverSwitchIndex"];
		//Desc.iArrivalCellIndex = InteractJson[i]["iArrivalCellIndex"];

		Desc.bUseGravity = InteractJson[i]["UseGravity"];
		CJson_Utility::Load_Float3(InteractJson[i]["RootMoveRate"], Desc.vPlayerRootMoveRate);
		CJson_Utility::Load_Float3(InteractJson[i]["ReverseRootMoveRate"], Desc.vPlayerReverseRootMoveRate);

		CJson_Utility::Load_Float3(InteractJson[i]["ColliderSize"], Desc.vBodyColliderSize);
		CJson_Utility::Load_Float3(InteractJson[i]["ColliderCenter"], Desc.vBodyColliderCenter);
		CJson_Utility::Load_Float3(InteractJson[i]["InteractColliderSize"], Desc.vInteractColliderSize);
		CJson_Utility::Load_Float3(InteractJson[i]["InteractColliderCenter"], Desc.vInteractColliderCenter);

		CJson_Utility::Load_Float3(InteractJson[i]["MoveColliderSize"], Desc.vMoveRangeColliderSize);
		CJson_Utility::Load_Float3(InteractJson[i]["MoveColliderCenter"], Desc.vMoveRangeColliderCenter);
		CJson_Utility::Load_Float3(InteractJson[i]["InteractMoveColliderSize"], Desc.vInteractMoveColliderSize);
		CJson_Utility::Load_Float3(InteractJson[i]["InteractMoveColliderCenter"], Desc.vInteractMoveColliderCenter);

		CJson_Utility::Load_Float4(InteractJson[i]["OffsetPosition"], Desc.vOffset);
		CJson_Utility::Load_Float4(InteractJson[i]["EnablePosition"], Desc.vEnablePosition);
		CJson_Utility::Load_Float4(InteractJson[i]["ArrivalPosition"], Desc.vArrivalPosition);

		const json& TransformJson = InteractJson[i]["Component"]["Transform"];
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

		json UpdateCellJson = InteractJson[i]["UpdateCellJson"];
		_int iUpdateCellJsonSize = (_int)UpdateCellJson.size();

		for (_int i = 0; i < iUpdateCellJsonSize; ++i)
		{
			Desc.vecUpdateCellIndex.push_back(UpdateCellJson[i]["UpdateCellIndex"]);
		}

		if (Desc.bOwner == true)
		{
			CEnvironment_Interact* pObject = { nullptr };

			pObject = dynamic_cast<CEnvironment_Interact*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_GAMEPLAY, L"Layer_BackGround", L"Prototype_GameObject_Environment_InteractObject", &Desc));

		}
		else
			continue;
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
 
 		pInstanceObject = dynamic_cast<CEnvironment_Instance*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_GAMEPLAY, L"Layer_BackGround", L"Prototype_GameObject_Environment_Instance", &InstanceDesc));
 
 	}
 
	json AnimalJson = Stage1MapJson["NPC_Json"];
	_int iAnimalJsonSize = (_int)AnimalJson.size();

	for (_int i = 0; i < iAnimalJsonSize; ++i)
	{
		CAnimalObject::Animal_OBJECT_DESC Desc = {};

		wstring strLoadModelTag;
		string strJsonModelTag = AnimalJson[i]["ModelTag"];

		m_pGameInstance->String_To_WString(strJsonModelTag, strLoadModelTag);
		Desc.strModelTag = strLoadModelTag;

		Desc.iPlayAnimationIndex = AnimalJson[i]["AnimationIndex"];
		Desc.bPreview = false;

		const json& TransformJson = AnimalJson[i]["Component"]["Transform"];
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


		
		CAnimalObject* pObject = { nullptr };

		pObject = dynamic_cast<CAnimalObject*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_GAMEPLAY, L"Layer_NPC", L"Prototype_GameObject_AnimalObject", &Desc));

		

	}

 
	return S_OK;

}
HRESULT CLevel_GamePlay::Ready_Layer_Test(const wstring& strLayerTag)
{
	//FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Interact_Chain")));
	//! 蕉還昔什渡縮戚 拝琶推蒸嬢像. ぞぞぞぞぞぞFAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_InstanceMonster")));


	///* ui test */
	//m_pGameInstance->Add_CloneObject(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_UI_Player_HPBar"));
	//
	//CUI_Player_HPBar::UI_DESC desc = {};
	//desc.fPositionX = (_float)g_iWinSizeX / 2 + 20.f;
	//desc.fPositionY = (_float)g_iWinSizeY / 2 + 20.f;
	//m_pGameInstance->Add_CloneObject(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_UI_Player_HPBar"), &desc);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Shader()
{
	/* For. Shadow */
	m_pGameInstance->Add_ShadowLight_View(ECast(LEVEL::LEVEL_GAMEPLAY), _float4(Engine::g_vLightEye), _float4(Engine::g_vLightAt), _float4(Engine::g_vLightUp));
	m_pGameInstance->Add_ShadowLight_Proj(ECast(LEVEL::LEVEL_GAMEPLAY), 60.f, (_float)g_iWinSizeX / (_float)g_iWinSizeY, Engine::g_fLightNear, Engine::g_fLightFar);

	/* 1. 捨戚希 段奄鉢 */
	m_pGameInstance->Off_Shader(); 

	/* 2. 捨戚希 辛芝 繕箭 */
	m_pGameInstance->Get_Renderer()->Set_HBAO_Active(true);
	m_pGameInstance->Get_Renderer()->Set_BloomBlur_Active(true);
	m_pGameInstance->Get_Renderer()->Set_Fog_Active(true);
	m_pGameInstance->Get_Renderer()->Set_Radial_Blur_Active(false);
	m_pGameInstance->Get_Renderer()->Set_DOF_Active(false);
	m_pGameInstance->Get_Renderer()->Set_HDR_Active(true);
	m_pGameInstance->Get_Renderer()->Set_FXAA_Active(true);
	m_pGameInstance->Get_Renderer()->Set_HSV_Active(true);
	
	HBAO_PLUS_DESC Desc_Hbao = {};
	Desc_Hbao.bHBAO_Active = true;
	Desc_Hbao.fRadius = 1.482f;
	Desc_Hbao.fBias = 0.1f;
	Desc_Hbao.fPowerExponent = 1.828f;
	Desc_Hbao.fBlur_Sharpness = 16.f;

	DEFERRED_DESC Desc_Deferred = {};
	Desc_Deferred.bRimBloom_Blur_Active = true;
	Desc_Deferred.bShadow_Active = true;

	FOG_DESC Desc_Fog = {};
	Desc_Fog.bFog_Active = true;
	Desc_Fog.fFogStartDepth = 97.746f;
	Desc_Fog.fFogStartDistance = 1.810f;
	Desc_Fog.fFogDistanceValue = 30.0f;
	Desc_Fog.fFogHeightValue = 47.151f;
	Desc_Fog.fFogDistanceDensity = 0.275f;
	Desc_Fog.fFogHeightDensity = 0.242f;

	HDR_DESC Desc_HDR = {};
	Desc_HDR.bHDR_Active = true;
	Desc_HDR.fmax_white = 0.478f;

	ANTI_DESC Desc_Anti = {};
	Desc_Anti.bFXAA_Active = true;

	HSV_DESC Desc_HSV = {};
	Desc_HSV.bScreen_Active = true;
	Desc_HSV.fFinal_Brightness = 0.818f;
	Desc_HSV.fFinal_Saturation = 0.979f;

	m_pGameInstance->Get_Renderer()->Set_HBAO_Option(Desc_Hbao);
	m_pGameInstance->Get_Renderer()->Set_Deferred_Option(Desc_Deferred);
	m_pGameInstance->Get_Renderer()->Set_Fog_Option(Desc_Fog);
	m_pGameInstance->Get_Renderer()->Set_HDR_Option(Desc_HDR);
	m_pGameInstance->Get_Renderer()->Set_FXAA_Option(Desc_Anti);
	m_pGameInstance->Get_Renderer()->Set_HSV_Option(Desc_HSV);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_UI()
{
	/* Test遂亀 */
	//FAILED_CHECK(Ready_Layer_UI_Monster(TEXT("Layer_UI_Monster"), nullptr));
	 
	//FAILED_CHECK(Ready_Layer_UI_Interface(TEXT("Layer_UI_Interface"), nullptr)); /* 兄希展ぁ*/
	FAILED_CHECK(Ready_Layer_UI(TEXT("Layer_UI"), nullptr));

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI_Monster(const wstring& strLayerTag, void* pArg)
{
	/* 蓄 板 督縮背辞 舛左 閤焼神奄 */
	
	/* MRT稽 広走 省生檎 窒径戚照蟹身. */
	//json json_in;

	//char filePath[MAX_PATH] = "../Bin/DataFiles/TestData/Test_5.json";

	//_int		iPathNum = 0;
	//string		strFileName;
	//string		strFilePath;
	//
	//CJson_Utility::Load_Json(filePath, json_in);

	//for (auto& item : json_in.items())
	//{
	//	json object = item.value();

	//	CUI::UI_DESC tUI_Info;

	//	tUI_Info.strProtoTag = object["ProtoTag"];
	//	tUI_Info.strFilePath = object["FilePath"];

	//	wstring wstrPrototag;
	//	m_pGameInstance->String_To_WString(tUI_Info.strProtoTag, wstrPrototag);

	//	wstring wstrFilePath;
	//	m_pGameInstance->String_To_WString(tUI_Info.strFilePath, wstrFilePath);

	//	CUI_Anything* pUI_Object = dynamic_cast<CUI_Anything*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_UI_Anything"), &tUI_Info));

	//	pUI_Object->Get_Transform()->Load_FromJson(object);
	//	
	//	// Pos 説焼爽奄
	//}


	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_UI_Anything"), &json_in)))
	//	return E_FAIL;
	//FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_UI_Anything")));

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI_Interface(const wstring& strLayerTag, void* pArg)
{

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_LeftInterface(const wstring& strLayerTag, void* pArg)
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

		/* 煽舌授辞櫛 限澗走 溌昔馬奄 */
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

HRESULT CLevel_GamePlay::Ready_RightInterface(const wstring& strLayerTag, void* pArg)
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

		/* 煽舌授辞櫛 限澗走 溌昔馬奄 */
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

HRESULT CLevel_GamePlay::Ready_QuestBox(const wstring& strLayerTag, void* pArg)
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

		/* 煽舌授辞櫛 限澗走 溌昔馬奄 */
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

HRESULT CLevel_GamePlay::Ready_TutorialBox(const wstring& strLayerTag, void* pArg)
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

		/* 煽舌授辞櫛 限澗走 溌昔馬奄 */
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

HRESULT CLevel_GamePlay::Ready_LevelUp(const wstring& strLayerTag, void* pArg)
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

		/* 煽舌授辞櫛 限澗走 溌昔馬奄 */
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

HRESULT CLevel_GamePlay::Ready_Reward_Item(const wstring& strLayerTag, void* pArg)
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

		/* 煽舌授辞櫛 限澗走 溌昔馬奄 */
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

HRESULT CLevel_GamePlay::Ready_Layer_UI(const wstring& strLayerTag, void* pArg)
{
	//// Ready Interface
	//FAILED_CHECK(CUI_Manager::GetInstance()->Ready_Interface(LEVEL_STATIC));
	//// Ready Crosshair
	//FAILED_CHECK(CUI_Manager::GetInstance()->Ready_Crosshair(LEVEL_STATIC));

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Cursor(const wstring& strLayerTag, void* pArg)
{
	//json json_in;

	//char filePath[MAX_PATH] = "../Bin/DataFiles/Data_UI/PlayerInterface/RewardItem.json";

	//_int		iPathNum = 0;
	//string		strFileName;
	//string		strFilePath;

	//CJson_Utility::Load_Json(filePath, json_in);

	//for (auto& item : json_in.items())
	//{
	//	json object = item.value();

	//	CUI::UI_DESC tUI_Info;

	//	/* 煽舌授辞櫛 限澗走 溌昔馬奄 */
	//	tUI_Info.bParent = object["Parent"];					// 1. Parent
	//	tUI_Info.bWorld = object["World"];						// 2. World
	//	tUI_Info.bGroup = object["Group"];						// 3. Group
	//	tUI_Info.fAlpha = object["Alpha"];						// 4. Alpha
	//	tUI_Info.iObjectNum = object["ObjectNum"];				// 5. ObjectNum
	//	tUI_Info.iShaderNum = object["ShaderNum"];				// 6. ShaderPathNum
	//	tUI_Info.strObjectName = object["ObjectName"];			// 7. ObjectName
	//	tUI_Info.strLayerTag = object["LayerTag"];				// 8. LayerTag
	//	tUI_Info.strCloneTag = object["CloneTag"];				// 9. CloneTag
	//	tUI_Info.strProtoTag = object["ProtoTag"];				// 10. ProtoTag
	//	tUI_Info.strFilePath = object["FilePath"];				// 11. FilePath
	//	tUI_Info.strMapTextureTag = object["MapTextureTag"];	// 12. MapTexture
	//	tUI_Info.vColor.m128_f32[0] = object["ColorR"];			// 13. R
	//	tUI_Info.vColor.m128_f32[1] = object["ColorG"];			// 14. G
	//	tUI_Info.vColor.m128_f32[2] = object["ColorB"];			// 15. B
	//	tUI_Info.vColor.m128_f32[3] = object["ColorA"];			// 16. A

	//	wstring wstrLayer;
	//	m_pGameInstance->String_To_WString(tUI_Info.strLayerTag, wstrLayer); //

	//	wstring wstrClonetag;
	//	m_pGameInstance->String_To_WString(tUI_Info.strCloneTag, wstrClonetag);

	//	wstring wstrPrototag;
	//	m_pGameInstance->String_To_WString(tUI_Info.strProtoTag, wstrPrototag);

	//	wstring wstrFilePath;
	//	m_pGameInstance->String_To_WString(tUI_Info.strFilePath, wstrFilePath);

	//	CUI* pUI_Object = dynamic_cast<CUI*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_STATIC, strLayerTag, wstrClonetag, &tUI_Info));

	//	pUI_Object->Get_Transform()->Load_FromJson(object);		// 17. TransformCom
	//}

		CUI* pUI_Object = dynamic_cast<CUI*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_UI_MouseCursor")));

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Event()
{
	json LoadJson;

	if (FAILED(CJson_Utility::Load_Json(m_strStage1MapLoadPath.c_str(), LoadJson)))
	{
		MSG_BOX("戚坤闘 災君神奄 叔鳶");
		return E_FAIL;
	}

	json TriggerJson = LoadJson["Trigger_Json"];

	json MonsterTriggerJson = TriggerJson["MonsterTriggerJson"];
	_int iMonsterTriggerSize = (_int)MonsterTriggerJson.size();
	
	for (_int i = 0; i < iMonsterTriggerSize; ++i)
	{
		CEvent_MosnterSpawnTrigger::MONSTERSPAWN_TRIGGERDESC MonsterTriggerDesc = {};
		MonsterTriggerDesc.bOnTrigger = MonsterTriggerJson[i]["OnTrigger"];
		MonsterTriggerDesc.strSpawnMonsterJsonPath = m_strStage1MapLoadPath;
		MonsterTriggerDesc.strTriggerNameTag = MonsterTriggerJson[i]["NameTag"];
		MonsterTriggerDesc.iSpawnGroupIndex = MonsterTriggerJson[i]["SpawnGroupIndex"];
		CJson_Utility::Load_Float3(MonsterTriggerJson[i]["ColliderSize"], MonsterTriggerDesc.vColliderSize);
		CJson_Utility::Load_Float3(MonsterTriggerJson[i]["ColliderCenter"], MonsterTriggerDesc.vColliderCenter);
	
		CEvent_MosnterSpawnTrigger* pMonsterTrigger = CEvent_MosnterSpawnTrigger::Create(m_pDevice, m_pContext, &MonsterTriggerDesc);
	
		pMonsterTrigger->Load_FromJson(MonsterTriggerJson[i]);
	
		if (pMonsterTrigger == nullptr)
		{
			MSG_BOX("佼什斗 闘軒暗 災君神奄 叔鳶");
			return E_FAIL;
		}
		else
		{
			m_pGameInstance->Add_Event(pMonsterTrigger);
		}
	
	
	}

	json UITriggerJson = TriggerJson["UITriggerJson"];
	_int iUITriggerJsonSize = (_int)UITriggerJson.size();
	
	for (_int i = 0; i < iUITriggerJsonSize; ++i)
	{
		CEvent_UITrigger::UI_TRIGGERDESC UITriggerDesc = {};
		UITriggerDesc.bOnTrigger = UITriggerJson[i]["OnTrigger"];
		UITriggerDesc.strSpawnMonsterJsonPath = UITriggerJson[i]["JsonPath"];
		UITriggerDesc.strTriggerNameTag = UITriggerJson[i]["NameTag"];
		UITriggerDesc.iSpawnGroupIndex = UITriggerJson[i]["SpawnGroupIndex"];
		UITriggerDesc.eTriggerType = CEvent_Trigger::TRIGGER_UI;
		CJson_Utility::Load_Float3(UITriggerJson[i]["ColliderSize"], UITriggerDesc.vColliderSize);
		CJson_Utility::Load_Float3(UITriggerJson[i]["ColliderCenter"], UITriggerDesc.vColliderCenter);
	
		CEvent_UITrigger* pUITrigger = CEvent_UITrigger::Create(m_pDevice, m_pContext, &UITriggerDesc);
	
		const json& TransformJson = UITriggerJson[i]["Component"]["Transform"];
		_float4x4 WorldMatrix;
	
		for (_int TransformLoopIndex = 0; TransformLoopIndex < 4; ++TransformLoopIndex)
		{
			for (_int TransformSecondLoopIndex = 0; TransformSecondLoopIndex < 4; ++TransformSecondLoopIndex)
			{
				WorldMatrix.m[TransformLoopIndex][TransformSecondLoopIndex] = TransformJson[TransformLoopIndex][TransformSecondLoopIndex];
			}
		}
	
		pUITrigger->Load_FromJson(UITriggerJson[i]);
	
		if (pUITrigger == nullptr)
		{
			MSG_BOX("UI 闘軒暗 災君神奄 叔鳶");
			return E_FAIL;
		}
		else
		{
			m_pGameInstance->Add_Event(pUITrigger);
		}
	}


	return S_OK;
}

CLevel_GamePlay * CLevel_GamePlay::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)

{
	CLevel_GamePlay*		pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_GamePlay");
		Safe_Release(pInstance);
	}
	return pInstance; 
}

void CLevel_GamePlay::Free()
{
	__super::Free();

	m_pGameInstance->Clear_Event();

}
