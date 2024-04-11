#include "stdafx.h"
#include "Level_SnowMountainBoss.h"
#include "GameInstance.h"
#include "Player.h"
#include "Camera_Dynamic.h"
#include "Environment_Instance.h"
#include "Effect_Instance.h"

#pragma region UI
#include "UI_Anything.h"
#include "UI_MonsterHp.h"
#include "UI_MonsterHpFrame.h"
#pragma endregion

#include "LandObject.h"

#pragma region Test
#include "Screamer.h"  
#pragma endregion

#pragma region Monster
#include "Monster_Character.h"
#pragma endregion


#pragma region MAP
#include "Environment_Object.h"
#include "Environment_Instance.h"
#include "Environment_LightObject.h"
#include "Environment_SpecialObject.h"
#include "Environment_Interact.h"

#pragma endregion



#include "Data_Manager.h"
#include "MasterCamera.h"
#include "SpringCamera.h"
#include "Light.h"
#include "Event_MonsterSpawnTrigger.h"


CLevel_SnowMountainBoss::CLevel_SnowMountainBoss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_SnowMountainBoss::Initialize()
{
	m_pGameInstance->Get_Renderer()->Render_UI_MRT(false);
	m_pGameInstance->Set_CurrentLevel(m_pGameInstance->Get_NextLevel());
	Set_ShaderOption("../Bin/DataFiles/Data_Shader/Level/Level_Snowmountain_Boss_Shader.json");

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

void CLevel_SnowMountainBoss::Tick(_float fTimeDelta)
{

}

HRESULT CLevel_SnowMountainBoss::Render()
{
	SetWindowText(g_hWnd, TEXT("SnowMountain Boss 레벨입니다."));

	return S_OK;
}

HRESULT CLevel_SnowMountainBoss::Ready_LightDesc()
{
	/* Shadow Light */
	m_pGameInstance->Add_ShadowLight_View(ECast(LEVEL::LEVEL_SNOWMOUNTAINBOSS), _float4(Engine::g_vLightEye), _float4(Engine::g_vLightAt), _float4(Engine::g_vLightUp));
	m_pGameInstance->Add_ShadowLight_Proj(ECast(LEVEL::LEVEL_SNOWMOUNTAINBOSS), 60.f, (_float)g_iWinSizeX / (_float)g_iWinSizeY, Engine::g_fLightNear, Engine::g_fLightFar);

	/* Map Light */
	CLight* pDirectionalLight = m_pGameInstance->Get_DirectionLight();

	if (pDirectionalLight != nullptr) //TODO 기존에 디렉셔널 라이트가 존재했다면.
	{
		m_pGameInstance->Remove_Light(pDirectionalLight->Get_LightIndex());
	}

	json IntroBossMapJson = {};

	if (FAILED(CJson_Utility::Load_Json(m_strMapLoadPath.c_str(), IntroBossMapJson)))
	{
		MSG_BOX("조명 불러오기 실패");
		return E_FAIL;
	}

	json LightJson = IntroBossMapJson["Light_Json"];
	_int iLightJsonSize = (_int)LightJson.size();

	for (_int i = 0; i < iLightJsonSize; ++i)
	{
		LIGHT_DESC LightDesc = {};

		LightDesc.iLightIndex = LightJson[i]["LightIndex"];
		LightDesc.bEnable = LightJson[i]["LightEnable"];
		LightDesc.fCutOff = LightJson[i]["CutOff"];
		LightDesc.fOuterCutOff = LightJson[i]["OuterCutOff"];
		LightDesc.fIntensity = LightJson[i]["Intensity"]; // ◀ 여기 추가됨 

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
			MSG_BOX("라이트 불러오기 실패");
			return E_FAIL;
		}

	}

	json LightObjectJson = IntroBossMapJson["LightObject_Json"];
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
		LightDesc.fIntensity = LightObjectJson[i]["Intensity"]; // ◀ 여기 추가됨 

		LightDesc.eType = LightObjectJson[i]["LightType"];
		CJson_Utility::Load_Float4(LightObjectJson[i]["Direction"], LightDesc.vDirection);
		LightDesc.fRange = LightObjectJson[i]["Range"];
		CJson_Utility::Load_Float4(LightObjectJson[i]["Position"], LightDesc.vPosition);
		CJson_Utility::Load_Float4(LightObjectJson[i]["Diffuse"], LightDesc.vDiffuse);
		CJson_Utility::Load_Float4(LightObjectJson[i]["Ambient"], LightDesc.vAmbient);
		CJson_Utility::Load_Float4(LightObjectJson[i]["Specular"], LightDesc.vSpecular);


		LightObjectDesc.LightDesc = LightDesc;

		CEnvironment_LightObject* pLightObject = dynamic_cast<CEnvironment_LightObject*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_SNOWMOUNTAIN, L"Layer_BackGround", L"Prototype_GameObject_Environment_LightObject", &LightObjectDesc));

		if (pLightObject == nullptr)
		{
			MSG_BOX("라이트오브젝트 생성실패");
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CLevel_SnowMountainBoss::Ready_Layer_Camera(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_SNOWMOUNTAINBOSS, strLayerTag, TEXT("Prototype_GameObject_MasterCamera"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_SnowMountainBoss::Ready_Layer_Player(const wstring& strLayerTag)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_SNOWMOUNTAINBOSS, strLayerTag, TEXT("Prototype_GameObject_Player")));

	pPlayer->Set_InitPosition(_float3(99.83f, 0.f, 25.80f));
	//pPlayer->Set_Position(_float3(60.0f, 0.f, 29.84f));
	//CNavigation* pNavigation = pPlayer->Get_Navigation();
	//pNavigation->Set_CurrentIndex(pNavigation->Get_SelectRangeCellIndex(pPlayer));

	//pNavigation->Set_CurrentIndex(pNavigation->Get_SelectRangeCellIndex(pPlayer));

	//   FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_SNOWMOUNTAIN, strLayerTag, TEXT("Prototype_GameObject_Player"), pArg));

	//CGameObject* pPlayer = m_pGameInstance->Add_CloneObject_And_Get(Level_SnowMountain, strLayerTag, TEXT("Prototype_GameObject_Player"), pArg);
	//if (nullptr == pPlayer)
	//   return E_FAIL;

	//m_pGameInstance->Set_Player(pPlayer);

	return S_OK;
}

HRESULT CLevel_SnowMountainBoss::Ready_Layer_Effect(const wstring& strLayerTag)
{
	//FAILED_CHECK(m_pGameInstance->Add_CloneObject(Level_SnowMountain, strLayerTag, TEXT("Prototype_GameObject_Particle_Blue")));
	//FAILED_CHECK(m_pGameInstance->Add_CloneObject(Level_SnowMountain, strLayerTag, TEXT("Prototype_GameObject_Particle_Red")));
	//FAILED_CHECK(m_pGameInstance->Add_CloneObject(Level_SnowMountain, strLayerTag, TEXT("Prototype_GameObject_Effect_Explosion")));

	return S_OK;
}

HRESULT CLevel_SnowMountainBoss::Ready_Layer_Monster(const wstring& strLayerTag)
{
	//CGameObject* pMonster = { nullptr };
	//FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_SNOWMOUNTAIN, strLayerTag, TEXT("Prototype_GameObject_Monster"), pArg));
	////! Boss
	//pMonster = m_pGameInstance->Add_CloneObject_And_Get(LEVEL_SNOWMOUNTAINBOSS, L"Layer_Boss", TEXT("Prototype_GameObject_Mother"));
	//if (nullptr == pMonster)   return E_FAIL;
	//pMonster->Set_Position(_float3(103.f, 0.f, 112.36f));
	//pMonster->Get_Transform()->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f));
	//
	//pMonster = m_pGameInstance->Add_CloneObject_And_Get(LEVEL_SNOWMOUNTAINBOSS, L"Layer_Boss", TEXT("Prototype_GameObject_Son"));
	//if (nullptr == pMonster)   return E_FAIL;
	//pMonster->Set_Position(_float3(86.f, 0.f, 80.36f));
	//pMonster->Get_Transform()->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-180.f));
	//
	//pMonster = m_pGameInstance->Add_CloneObject_And_Get(LEVEL_SNOWMOUNTAINBOSS, L"Layer_Boss", TEXT("Prototype_GameObject_Son"));
	//if (nullptr == pMonster)   return E_FAIL;
	//pMonster->Set_Position(_float3(120.f, 0.f, 80.36f));
	//pMonster->Get_Transform()->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-180.f));
	return S_OK;


}

HRESULT CLevel_SnowMountainBoss::Ready_Layer_BackGround(const wstring& strLayerTag)
{
	FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_SNOWMOUNTAINBOSS, strLayerTag, TEXT("Prototype_GameObject_Sky")));

	json Stage1MapJson = {};

	if (FAILED(CJson_Utility::Load_Json(m_strMapLoadPath.c_str(), Stage1MapJson)))
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

		pObject = dynamic_cast<CEnvironment_Object*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_SNOWMOUNTAINBOSS, L"Layer_BackGround", L"Prototype_GameObject_Environment_Object", &Desc));
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
		Desc.eChangeLevel = (LEVEL)InteractJson[i]["InteractLevel"];

		//Desc.strSplineJsonPath = InteractJson[i]["SplineJsonPath"];
		Desc.bEnable = InteractJson[i]["Enable"];
		Desc.strEnableJsonPath = InteractJson[i]["EnableJsonPath"];
		Desc.iInteractGroupIndex = InteractJson[i]["InteractGroupIndex"];
		Desc.bOffset = InteractJson[i]["Offset"];
		Desc.bOwner = InteractJson[i]["Owner"];
		Desc.bRootTranslate = InteractJson[i]["RootTranslate"];
		Desc.bRotate = InteractJson[i]["Rotate"];
		Desc.fRotationAngle = InteractJson[i]["RotationAngle"];
		Desc.fRotationSpeed = InteractJson[i]["RotationSpeed"];
		Desc.bArrival = InteractJson[i]["Arrival"];


		Desc.bUseGravity = InteractJson[i]["UseGravity"];
		CJson_Utility::Load_Float3(InteractJson[i]["RootMoveRate"], Desc.vPlayerRootMoveRate);

		CJson_Utility::Load_Float3(InteractJson[i]["InteractColliderSize"], Desc.vInteractColliderSize);
		CJson_Utility::Load_Float3(InteractJson[i]["InteractColliderCenter"], Desc.vInteractColliderCenter);

		CJson_Utility::Load_Float3(InteractJson[i]["MoveColliderSize"], Desc.vMoveRangeColliderSize);
		CJson_Utility::Load_Float3(InteractJson[i]["MoveColliderCenter"], Desc.vMoveRangeColliderCenter);

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



		if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_SNOWMOUNTAINBOSS, L"Layer_BackGround", L"Prototype_GameObject_Environment_InteractObject", &Desc)))
			return E_FAIL;
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

		pInstanceObject = dynamic_cast<CEnvironment_Instance*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_SNOWMOUNTAINBOSS, L"Layer_BackGround", L"Prototype_GameObject_Environment_Instance", &InstanceDesc));

	}




	//CEnvironment_SpecialObject::ENVIRONMENT_SPECIALOBJECT_DESC SpecialDesc;
	//
	//SpecialDesc.bAnimModel = false;
	//SpecialDesc.bPreview = false;
	//
	//SpecialDesc.strModelTag = L"Prototype_Component_Model_BloodPoolsRaid";
	////Desc.iShaderPassIndex = 6;
	//XMStoreFloat4x4(&SpecialDesc.WorldMatrix, XMMatrixIdentity());
	//
	//
	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_INTRO_BOSS, L"Layer_BackGround", L"Prototype_GameObject_Environment_SpecialObject", &SpecialDesc)))
	//   return E_FAIL;


	//CEnvironment_LightObject::ENVIRONMENT_LIGHTOBJECT_DESC LightObjectDesc;
	//
	//LightObjectDesc.bAnimModel = false;
	//LightObjectDesc.bPreview = false;
	//LightObjectDesc.strModelTag = L"Prototype_Component_Model_SecretTempleStatue1";
	//XMStoreFloat4x4(&LightObjectDesc.WorldMatrix, XMMatrixIdentity());
	//
	//LightObjectDesc.iLightIndex = 4;
	//
	//LIGHT_DESC LightDesc;
	//
	//
	//LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	//XMStoreFloat4(&LightDesc.vPosition, XMLoadFloat4x4(&LightObjectDesc.WorldMatrix).r[3]);
	//LightDesc.fRange = 100.f;
	//LightDesc.vPosition = _float4(60.0f, 0.f, 55.f, 1.f);
	//LightDesc.vDiffuse = _float4(0.6f, 0.2f, 0.05f, 1.0f);
	//LightDesc.vAmbient = _float4(0.1f, 0.1f, 0.1f, 1.f);
	//LightDesc.vSpecular = LightDesc.vDiffuse;
	//
	//LightObjectDesc.LightDesc = LightDesc;
	//
	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_INTRO_BOSS, L"Layer_BackGround", L"Prototype_GameObject_Environment_LightObject", &LightObjectDesc)))
	//   return E_FAIL;


	return S_OK;

}

HRESULT CLevel_SnowMountainBoss::Ready_Layer_Test(const wstring& strLayerTag)
{
	//FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_SNOWMOUNTAIN, strLayerTag, TEXT("Prototype_GameObject_Interact_Chain")));
	//FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_SNOWMOUNTAIN, strLayerTag, TEXT("Prototype_GameObject_Screamer")));

	return S_OK;
}

HRESULT CLevel_SnowMountainBoss::Ready_UI()
{
	// FAILED_CHECK(Ready_Layer_UI_Monster(TEXT("Layer_UI_Monster"), nullptr));

	FAILED_CHECK(Ready_Layer_UI_Player(TEXT("Layer_UI_Player"), nullptr));

	return S_OK;
}

HRESULT CLevel_SnowMountainBoss::Ready_Layer_UI_Monster(const wstring& strLayerTag, void* pArg)
{
	/* 추 후 파싱해서 정보 받아오기 */

	json json_in;

	char filePath[MAX_PATH] = "../Bin/DataFiles/Data_UI/UI_Info";

	_int      iPathNum = 0;
	string      strFileName;
	string      strFilePath;


	CJson_Utility::Load_Json(filePath, json_in);

	for (auto& item : json_in.items())
	{
		json object = item.value();

		CUI::UI_DESC tUI_Info;

		tUI_Info.fPositionX = object["PostionX"];
		tUI_Info.fPositionY = object["PostionY"];
		tUI_Info.fScaleX = object["SizeX"];
		tUI_Info.fScaleY = object["SizeY"];
		tUI_Info.strProtoTag = object["ProtoTag"];
		tUI_Info.strFilePath = object["FilePath"];

		wstring wstrPrototag;
		m_pGameInstance->String_To_WString(tUI_Info.strProtoTag, wstrPrototag);

		wstring wstrFilePath;
		m_pGameInstance->String_To_WString(tUI_Info.strFilePath, wstrFilePath);

		FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_UI_Anything"), &tUI_Info));
	}
	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_UI_Anything"), &json_in)))
	//   return E_FAIL;

	return S_OK;
}

HRESULT CLevel_SnowMountainBoss::Ready_Layer_UI_Player(const wstring& strLayerTag, void* pArg)
{
	return S_OK;
}

HRESULT CLevel_SnowMountainBoss::Ready_Event()
{

	json LoadJson;

	if (FAILED(CJson_Utility::Load_Json(m_strMapLoadPath.c_str(), LoadJson)))
	{
		MSG_BOX("이벤트 불러오기 실패");
		return E_FAIL;
	}

	json TriggerJson = LoadJson["Trigger_Json"];

	json MonsterTriggerJson = TriggerJson["MonsterTriggerJson"];
	_int iMonsterTriggerSize = (_int)MonsterTriggerJson.size();

	for (_int i = 0; i < iMonsterTriggerSize; ++i)
	{
		CEvent_MosnterSpawnTrigger::MONSTERSPAWN_TRIGGERDESC MonsterTriggerDesc = {};
		MonsterTriggerDesc.bOnTrigger = MonsterTriggerJson[i]["OnTrigger"];
		MonsterTriggerDesc.strSpawnMonsterJsonPath = m_strMapLoadPath;
		MonsterTriggerDesc.strTriggerNameTag = MonsterTriggerJson[i]["NameTag"];
		MonsterTriggerDesc.iSpawnGroupIndex = MonsterTriggerJson[i]["SpawnGroupIndex"];
		CJson_Utility::Load_Float3(MonsterTriggerJson[i]["ColliderSize"], MonsterTriggerDesc.vColliderSize);
		CJson_Utility::Load_Float3(MonsterTriggerJson[i]["ColliderCenter"], MonsterTriggerDesc.vColliderCenter);

		CEvent_MosnterSpawnTrigger* pMonsterTrigger = CEvent_MosnterSpawnTrigger::Create(m_pDevice, m_pContext, &MonsterTriggerDesc);

		pMonsterTrigger->Load_FromJson(MonsterTriggerJson[i]);

		if (pMonsterTrigger == nullptr)
		{
			MSG_BOX("몬스터 트리거 불러오기 실패");
			return E_FAIL;
		}
		else
		{
			m_pGameInstance->Add_Event(pMonsterTrigger);
		}

	}


	return S_OK;

}

CLevel_SnowMountainBoss* CLevel_SnowMountainBoss::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)

{
	CLevel_SnowMountainBoss* pInstance = new CLevel_SnowMountainBoss(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_SnowMountainBoss");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_SnowMountainBoss::Free()
{
	__super::Free();


}