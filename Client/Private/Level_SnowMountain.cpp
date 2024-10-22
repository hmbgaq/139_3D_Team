#include "stdafx.h"
#include "Player.h"
#include "Effect.h"
#include "LandObject.h"
#include "GameInstance.h"
#include "Effect_Manager.h"
#include "Camera_Dynamic.h"
#include "Effect_Instance.h"
#include "Level_SnowMountain.h"
#include "Environment_Instance.h"

#pragma region UI
#include "UI_Anything.h"
#include "UI_MonsterHp.h"
#include "UI_MonsterHpFrame.h"
#pragma endregion

#pragma region Test
#include "Screamer.h"  
#pragma endregion

#pragma region Monster
#include "Monster_Character.h"
#include "Event_MonsterSpawnTrigger.h"
#pragma endregion

#pragma region MAP
#include "Environment_Object.h"
#include "Environment_Instance.h"
#include "Environment_LightObject.h"
#include "Environment_SpecialObject.h"
#include "Environment_Interact.h"
#pragma endregion

#include "Light.h"
#include "Effect_Manager.h"
#include "Data_Manager.h"
#include "MasterCamera.h"
#include "SpringCamera.h"

#include "Level_Loading.h"


CLevel_SnowMountain::CLevel_SnowMountain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_SnowMountain::Initialize()
{
	m_pGameInstance->Get_Renderer()->Render_UI_MRT(false);
	m_pGameInstance->Set_CurrentLevel(m_pGameInstance->Get_NextLevel());
	Set_ShaderOption("../Bin/DataFiles/Data_Shader/Level/Level_Snowmountain_Shader.json");

	FAILED_CHECK(Ready_LightDesc());
	FAILED_CHECK(Ready_Layer_Player(TEXT("Layer_Player")));
	FAILED_CHECK(Ready_Layer_Monster(TEXT("Layer_Monster")));
	FAILED_CHECK(Ready_Layer_BackGround(TEXT("Layer_BackGround")));
	FAILED_CHECK(Ready_Layer_Camera(TEXT("Layer_Camera")));
	FAILED_CHECK(Ready_Layer_Test(TEXT("Layer_Test")));
	FAILED_CHECK(Ready_Effect());
	FAILED_CHECK(Ready_UI());
	FAILED_CHECK(Ready_Event());

	//m_pGameInstance->Play_BGM(L"BGM", L"SnowMountainBGM.wav", 5.f);

	return S_OK;
}

void CLevel_SnowMountain::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_GRAVE))
	{
		m_pGameInstance->Request_Level_Opening(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_SNOWMOUNTAINBOSS));
	}
}

HRESULT CLevel_SnowMountain::Render()
{
	SetWindowText(g_hWnd, TEXT("SnowMountain 레벨입니다."));

	return S_OK;
}

HRESULT CLevel_SnowMountain::Ready_LightDesc()
{
	/* Shadow Light */
	m_pGameInstance->Add_ShadowLight_View(ECast(LEVEL::LEVEL_SNOWMOUNTAIN), _float4(Engine::g_vLightEye), _float4(Engine::g_vLightAt), _float4(Engine::g_vLightUp));
	m_pGameInstance->Add_ShadowLight_Proj(ECast(LEVEL::LEVEL_SNOWMOUNTAIN), 60.f, (_float)g_iWinSizeX / (_float)g_iWinSizeY, Engine::g_fLightNear, Engine::g_fLightFar);

	m_pGameInstance->Remove_AllLight();

	/* Map Light */
//	CLight* pDirectionalLight = m_pGameInstance->Get_DirectionLight();
//
//	if (pDirectionalLight != nullptr) //TODO 기존에 디렉셔널 라이트가 존재했다면.
//	{
//		m_pGameInstance->Remove_Light(pDirectionalLight->Get_LightIndex());
//	}

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
		LightObjectDesc.iSpecialGroupIndex = LightObjectJson[i]["SpecialGroupIndex"];
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

HRESULT CLevel_SnowMountain::Ready_Layer_Camera(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_SNOWMOUNTAIN, strLayerTag, TEXT("Prototype_GameObject_MasterCamera"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_SnowMountain::Ready_Layer_Player(const wstring& strLayerTag)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_SNOWMOUNTAIN, strLayerTag, TEXT("Prototype_GameObject_Player")));
	NULL_CHECK_RETURN(pPlayer, E_FAIL);
	pPlayer->Set_InitPosition(_float3(14.87f, 0.f, -8.06f));

	/* chroma test */
	//pPlayer->Set_InitPosition(_float3(45.077f, 12.5f, -57.028f));
	//CNavigation* pNavi = dynamic_cast<CCharacter*>(pPlayer)->Get_Navigation();
	//NULL_CHECK_RETURN(pNavi, E_FAIL);
	//pNavi->Set_CurrentIndex(499);

	//pNavigation->Set_CurrentIndex(pNavigation->Get_SelectRangeCellIndex(pPlayer));

	//FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_SNOWMOUNTAIN, strLayerTag, TEXT("Prototype_GameObject_Player"), pArg));

	//CGameObject* pPlayer = m_pGameInstance->Add_CloneObject_And_Get(Level_SnowMountain, strLayerTag, TEXT("Prototype_GameObject_Player"), pArg);
	//if (nullptr == pPlayer)
	//   return E_FAIL;

	//m_pGameInstance->Set_Player(pPlayer);

	return S_OK;
}

HRESULT CLevel_SnowMountain::Ready_Layer_Monster(const wstring& strLayerTag)
{
	//FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_SNOWMOUNTAIN, strLayerTag, TEXT("Prototype_GameObject_Monster"), pArg));

	return S_OK;
}

HRESULT CLevel_SnowMountain::Ready_Layer_BackGround(const wstring& strLayerTag)
{
	FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_SNOWMOUNTAIN, strLayerTag, TEXT("Prototype_GameObject_Sky")));


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

		pObject = dynamic_cast<CEnvironment_Object*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_SNOWMOUNTAIN, L"Layer_BackGround", L"Prototype_GameObject_Environment_Object", &Desc));
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
		Desc.iSwitchIndex = InteractJson[i]["LeverSwitchIndex"];
		Desc.iArrivalCellIndex = InteractJson[i]["iArrivalCellIndex"];

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

			pObject = dynamic_cast<CEnvironment_Interact*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_SNOWMOUNTAIN, L"Layer_BackGround", L"Prototype_GameObject_Environment_InteractObject", &Desc));

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
		Desc.iSwitchIndex = InteractJson[i]["LeverSwitchIndex"];
		Desc.iArrivalCellIndex = InteractJson[i]["iArrivalCellIndex"];

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

			pObject = dynamic_cast<CEnvironment_Interact*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_SNOWMOUNTAIN, L"Layer_BackGround", L"Prototype_GameObject_Environment_InteractObject", &Desc));
			
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

		pInstanceObject = dynamic_cast<CEnvironment_Instance*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_SNOWMOUNTAIN, L"Layer_BackGround", L"Prototype_GameObject_Environment_Instance", &InstanceDesc));

	}

	json SpecialJson = Stage1MapJson["Special_Json"];
	_int iSpecialJsonSize = (_int)SpecialJson.size();

	
	for (_int i = 0; i < iSpecialJsonSize; ++i)
	{
		CEnvironment_SpecialObject::ENVIRONMENT_SPECIALOBJECT_DESC SpecialDesc = {};

		SpecialDesc.iShaderPassIndex = SpecialJson[i]["iShaderPassIndex"];
		SpecialDesc.bAnimModel = SpecialJson[i]["AnimType"];
		SpecialDesc.iPlayAnimationIndex = SpecialJson[i]["PlayAnimationIndex"];
		SpecialDesc.iSpecialGroupIndex = SpecialJson[i]["SpecialGroupIndex"];
		SpecialDesc.eSpecialType = SpecialJson[i]["SpecialType"];
		//TODOSpecialDesc.iBloomMeshIndex =      SpecialJson[i]["BloomMeshIndex"];
		SpecialDesc.bPreview = false;
		SpecialDesc.eElevatorType = SpecialJson[i]["ElevatorType"];
		SpecialDesc.fElevatorMinHeight = SpecialJson[i]["ElevatorMinHeight"];
		SpecialDesc.fElevatorMaxHeight = SpecialJson[i]["ElevatorMaxHeight"];
		SpecialDesc.fElevatorSpeed = SpecialJson[i]["ElevatorSpeed"];
		SpecialDesc.fElevatorRotationSpeed = SpecialJson[i]["ElevatorRotationSpeed"];
		SpecialDesc.iArrivalCellIndex = SpecialJson[i]["ElevatorArrivalCellIndex"];
		SpecialDesc.bLeverForElevator = SpecialJson[i]["ElevatorLever"];
		SpecialDesc.bOffset = SpecialJson[i]["OffsetFunction"];

		CJson_Utility::Load_Float3(SpecialJson[i]["ColliderSize"], SpecialDesc.vElevatorColliderSize);
		CJson_Utility::Load_Float3(SpecialJson[i]["ColliderCenter"], SpecialDesc.vElevatorColliderCenter);
		CJson_Utility::Load_Float4(SpecialJson[i]["ArrivalPosition"], SpecialDesc.vArrivalPosition);
		CJson_Utility::Load_Float4(SpecialJson[i]["OffsetPosition"], SpecialDesc.vOffset);

		m_pGameInstance->String_To_WString((string)SpecialJson[i]["ModelTag"], SpecialDesc.strModelTag);

		if (SpecialDesc.eSpecialType == CEnvironment_SpecialObject::SPECIAL_SIGNAL)
		{
			const json& TransformJson = SpecialJson[i]["Component"]["Transform"];
			_float4x4 WorldMatrix;

			for (_int TransformLoopIndex = 0; TransformLoopIndex < 4; ++TransformLoopIndex)
			{
				for (_int TransformSecondLoopIndex = 0; TransformSecondLoopIndex < 4; ++TransformSecondLoopIndex)
				{
					WorldMatrix.m[TransformLoopIndex][TransformSecondLoopIndex] = TransformJson[TransformLoopIndex][TransformSecondLoopIndex];
				}
			}

			SpecialDesc.WorldMatrix = WorldMatrix;

			CEnvironment_SpecialObject* pSpecialObject = dynamic_cast<CEnvironment_SpecialObject*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_SNOWMOUNTAIN, L"Layer_BackGround", L"Prototype_GameObject_Environment_SpecialObject", &SpecialDesc));

			if (pSpecialObject == nullptr)
			{
				MSG_BOX("스페셜오브젝트 생성실패");
				return E_FAIL;
			}

		}
		else
			continue;
	}

	for (_int i = 0; i < iSpecialJsonSize; ++i)
	{
		CEnvironment_SpecialObject::ENVIRONMENT_SPECIALOBJECT_DESC SpecialDesc = {};

		SpecialDesc.iShaderPassIndex = SpecialJson[i]["iShaderPassIndex"];
		SpecialDesc.bAnimModel = SpecialJson[i]["AnimType"];
		SpecialDesc.iPlayAnimationIndex = SpecialJson[i]["PlayAnimationIndex"];
		SpecialDesc.iSpecialGroupIndex = SpecialJson[i]["SpecialGroupIndex"];
		SpecialDesc.eSpecialType = SpecialJson[i]["SpecialType"];
		//TODOSpecialDesc.iBloomMeshIndex =      SpecialJson[i]["BloomMeshIndex"];
		SpecialDesc.bPreview = false;
		SpecialDesc.eElevatorType = SpecialJson[i]["ElevatorType"];
		SpecialDesc.fElevatorMinHeight = SpecialJson[i]["ElevatorMinHeight"];
		SpecialDesc.fElevatorMaxHeight = SpecialJson[i]["ElevatorMaxHeight"];
		SpecialDesc.fElevatorSpeed = SpecialJson[i]["ElevatorSpeed"];
		SpecialDesc.fElevatorRotationSpeed = SpecialJson[i]["ElevatorRotationSpeed"];
		SpecialDesc.iArrivalCellIndex = SpecialJson[i]["ElevatorArrivalCellIndex"];
		SpecialDesc.bLeverForElevator = SpecialJson[i]["ElevatorLever"];
		SpecialDesc.bOffset = SpecialJson[i]["OffsetFunction"];



		CJson_Utility::Load_Float3(SpecialJson[i]["ColliderSize"], SpecialDesc.vElevatorColliderSize);
		CJson_Utility::Load_Float3(SpecialJson[i]["ColliderCenter"], SpecialDesc.vElevatorColliderCenter);
		CJson_Utility::Load_Float4(SpecialJson[i]["ArrivalPosition"], SpecialDesc.vArrivalPosition);
		CJson_Utility::Load_Float4(SpecialJson[i]["OffsetPosition"], SpecialDesc.vOffset);


		m_pGameInstance->String_To_WString((string)SpecialJson[i]["ModelTag"], SpecialDesc.strModelTag);

		if (SpecialDesc.eSpecialType == CEnvironment_SpecialObject::SPECIAL_ELEVATOR)
		{
			const json& TransformJson = SpecialJson[i]["Component"]["Transform"];
			_float4x4 WorldMatrix;

			for (_int TransformLoopIndex = 0; TransformLoopIndex < 4; ++TransformLoopIndex)
			{
				for (_int TransformSecondLoopIndex = 0; TransformSecondLoopIndex < 4; ++TransformSecondLoopIndex)
				{
					WorldMatrix.m[TransformLoopIndex][TransformSecondLoopIndex] = TransformJson[TransformLoopIndex][TransformSecondLoopIndex];
				}
			}

			SpecialDesc.WorldMatrix = WorldMatrix;

			CEnvironment_SpecialObject* pSpecialObject = dynamic_cast<CEnvironment_SpecialObject*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_SNOWMOUNTAIN, L"Layer_BackGround", L"Prototype_GameObject_Environment_SpecialObject", &SpecialDesc));

			if (pSpecialObject == nullptr)
			{
				MSG_BOX("스페셜오브젝트 생성실패");
				return E_FAIL;
			}

		}
		else
			continue;
	}

	for (_int i = 0; i < iSpecialJsonSize; ++i)
	{
		CEnvironment_SpecialObject::ENVIRONMENT_SPECIALOBJECT_DESC SpecialDesc = {};

		SpecialDesc.iShaderPassIndex = SpecialJson[i]["iShaderPassIndex"];
		SpecialDesc.bAnimModel = SpecialJson[i]["AnimType"];
		SpecialDesc.iPlayAnimationIndex = SpecialJson[i]["PlayAnimationIndex"];
		SpecialDesc.iSpecialGroupIndex = SpecialJson[i]["SpecialGroupIndex"];
		SpecialDesc.eSpecialType = SpecialJson[i]["SpecialType"];
		//TODOSpecialDesc.iBloomMeshIndex =      SpecialJson[i]["BloomMeshIndex"];
		SpecialDesc.bPreview = false;
		SpecialDesc.eElevatorType = SpecialJson[i]["ElevatorType"];
		SpecialDesc.fElevatorMinHeight = SpecialJson[i]["ElevatorMinHeight"];
		SpecialDesc.fElevatorMaxHeight = SpecialJson[i]["ElevatorMaxHeight"];
		SpecialDesc.fElevatorSpeed = SpecialJson[i]["ElevatorSpeed"];
		SpecialDesc.fElevatorRotationSpeed = SpecialJson[i]["ElevatorRotationSpeed"];
		SpecialDesc.iArrivalCellIndex = SpecialJson[i]["ElevatorArrivalCellIndex"];
		SpecialDesc.bLeverForElevator = SpecialJson[i]["ElevatorLever"];
		SpecialDesc.bOffset = SpecialJson[i]["OffsetFunction"];

		CJson_Utility::Load_Float3(SpecialJson[i]["ColliderSize"], SpecialDesc.vElevatorColliderSize);
		CJson_Utility::Load_Float3(SpecialJson[i]["ColliderCenter"], SpecialDesc.vElevatorColliderCenter);
		CJson_Utility::Load_Float4(SpecialJson[i]["ArrivalPosition"], SpecialDesc.vArrivalPosition);
		CJson_Utility::Load_Float4(SpecialJson[i]["OffsetPosition"], SpecialDesc.vOffset);
		
		m_pGameInstance->String_To_WString((string)SpecialJson[i]["ModelTag"], SpecialDesc.strModelTag);


		if (SpecialDesc.eSpecialType == CEnvironment_SpecialObject::SPECIAL_TRACKLEVER)
		{
			const json& TransformJson = SpecialJson[i]["Component"]["Transform"];
			_float4x4 WorldMatrix;

			for (_int TransformLoopIndex = 0; TransformLoopIndex < 4; ++TransformLoopIndex)
			{
				for (_int TransformSecondLoopIndex = 0; TransformSecondLoopIndex < 4; ++TransformSecondLoopIndex)
				{
					WorldMatrix.m[TransformLoopIndex][TransformSecondLoopIndex] = TransformJson[TransformLoopIndex][TransformSecondLoopIndex];
				}
			}

			SpecialDesc.WorldMatrix = WorldMatrix;

			CEnvironment_SpecialObject* pSpecialObject = dynamic_cast<CEnvironment_SpecialObject*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_SNOWMOUNTAIN, L"Layer_BackGround", L"Prototype_GameObject_Environment_SpecialObject", &SpecialDesc));

			if (pSpecialObject == nullptr)
			{
				MSG_BOX("스페셜오브젝트 생성실패");
				return E_FAIL;
			}

		}
		else
			continue;
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

HRESULT CLevel_SnowMountain::Ready_Layer_Test(const wstring& strLayerTag)
{
	//FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_SNOWMOUNTAIN, strLayerTag, TEXT("Prototype_GameObject_Interact_Chain")));
	//FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_SNOWMOUNTAIN, strLayerTag, TEXT("Prototype_GameObject_Screamer")));

	return S_OK;
}

HRESULT CLevel_SnowMountain::Ready_Effect()
{
	m_pEffect = EFFECT_MANAGER->Play_Effect("Fog/", "SY_SnowMap.json", nullptr, _float3(0.f, 0.f, 0.f));

	return S_OK;
}

HRESULT CLevel_SnowMountain::Ready_UI()
{
	// FAILED_CHECK(Ready_Layer_UI_Monster(TEXT("Layer_UI_Monster"), nullptr));

	FAILED_CHECK(Ready_Layer_UI_Player(TEXT("Layer_UI_Player"), nullptr));

	_float3 vPos = { 14.87f, 0.f, -8.06f };
	m_pMapEffect = EFFECT_MANAGER->Play_Effect("Snow/", "Snow_05.json", nullptr, vPos);

	return S_OK;
}

HRESULT CLevel_SnowMountain::Ready_Layer_UI_Monster(const wstring& strLayerTag, void* pArg)
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

HRESULT CLevel_SnowMountain::Ready_Layer_UI_Player(const wstring& strLayerTag, void* pArg)
{
	return S_OK;
}

HRESULT CLevel_SnowMountain::Ready_Event()
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

HRESULT CLevel_SnowMountain::Ready_Shader(const string& strShaderFilePath)
{
	json BasicJson = {};

	if (FAILED(CJson_Utility::Load_Json(strShaderFilePath.c_str(), BasicJson)))
	{
		MSG_BOX("Fail to Load Shader");
		return E_FAIL;
	}

	/* 1. 셰이더 초기화 */
	m_pGameInstance->Off_Shader();

	/* 2. 셰이더 셋팅 */
	ANTI_DESC Desc_Anti = {};
	Desc_Anti.bFXAA_Active = BasicJson["Anti"]["bFXAA_Active"];

	CHROMA_DESC	Desc_Chroma = {};
	Desc_Chroma.bChroma_Active = BasicJson["Chroma"]["bFinal_Active"];
	Desc_Chroma.fChromaticIntensity = BasicJson["Chroma"]["fFinal_Intensity"];

	DOF_DESC Desc_Dof = {};
	Desc_Dof.bDOF_Active = BasicJson["DOF"]["bDOF_Active"];
	Desc_Dof.DOF_Distance = BasicJson["DOF"]["fDOF_Distance"];

	DEFERRED_DESC Desc_Deferred = {};
	Desc_Deferred.bRimBloom_Blur_Active = BasicJson["Deferred"]["bRimBloom_Blur_Active"];
	Desc_Deferred.bShadow_Active = BasicJson["Deferred"]["bShadow_Active"];

	FOG_DESC Desc_Fog = {};
	Desc_Fog.bFog_Active = BasicJson["Fog"]["bFog_Active"];
	Desc_Fog.fFogDistanceDensity = BasicJson["Fog"]["fFogDistanceDensity"];
	Desc_Fog.fFogDistanceValue = BasicJson["Fog"]["fFogDistanceValue"];
	Desc_Fog.fFogHeightDensity = BasicJson["Fog"]["fFogHeightDensity"];
	Desc_Fog.fFogHeightValue = BasicJson["Fog"]["fFogHeightValue"];
	Desc_Fog.fFogStartDepth = BasicJson["Fog"]["fFogStartDepth"];
	Desc_Fog.fFogStartDistance = BasicJson["Fog"]["fFogStartDistance"];
	Desc_Fog.vFogColor.x = BasicJson["Fog"]["vFogColor_x"];
	Desc_Fog.vFogColor.y = BasicJson["Fog"]["vFogColor_y"];
	Desc_Fog.vFogColor.z = BasicJson["Fog"]["vFogColor_z"];
	Desc_Fog.vFogColor.w = BasicJson["Fog"]["vFogColor_w"];

	HBAO_PLUS_DESC Desc_Hbao = {};
	Desc_Hbao.bHBAO_Active = BasicJson["HBAO"]["bHBAO_Active"];
	Desc_Hbao.fBias = BasicJson["HBAO"]["fBias"];
	Desc_Hbao.fBlur_Sharpness = BasicJson["HBAO"]["fBlur_Sharpness"];
	Desc_Hbao.fPowerExponent = BasicJson["HBAO"]["fPowerExponent"];
	Desc_Hbao.fRadius = BasicJson["HBAO"]["fRadius"];

	HDR_DESC Desc_HDR = {};
	Desc_HDR.bHDR_Active = BasicJson["HDR"]["bHDR_Active"];
	Desc_HDR.fmax_white = BasicJson["HDR"]["fmax_white"];

	HSV_DESC Desc_HSV = {};
	Desc_HSV.bScreen_Active = BasicJson["HSV"]["bScreen_Active"];
	Desc_HSV.fFinal_Saturation = BasicJson["HSV"]["fFinal_Saturation"];
	Desc_HSV.fFinal_Brightness = BasicJson["HSV"]["fFinal_Brightness"];

	LUMASHARPEN_DESC Desc_Luma = {};
	Desc_Luma.bLumaSharpen_Active = BasicJson["Luma"]["bLuma_Active"];
	Desc_Luma.foffset_bias = BasicJson["Luma"]["fLuma_bias"];
	Desc_Luma.fsharp_clamp = BasicJson["Luma"]["fLuma_clamp"];
	Desc_Luma.fsharp_strength = BasicJson["Luma"]["fLuma_strength"];

	PBR_DESC Desc_PBR = {};
	Desc_PBR.bPBR_ACTIVE = BasicJson["PBR"]["bPBR_Active"];
	Desc_PBR.fBrightnessOffset = BasicJson["PBR"]["fPBR_Brightness"];
	Desc_PBR.fSaturationOffset = BasicJson["PBR"]["fPBR_Saturation"];

	RADIAL_DESC Desc_Radial = {};
	Desc_Radial.bRadial_Active = BasicJson["Radial"]["bRadial_Active"];
	Desc_Radial.fRadial_Quality = BasicJson["Radial"]["fRadial_Quality"];
	Desc_Radial.fRadial_Power = BasicJson["Radial"]["fRadial_Power"];

	SSR_DESC Desc_SSR = {};
	Desc_SSR.bSSR_Active = BasicJson["SSR"]["bFinal_Active"];
	Desc_SSR.fRayStep = BasicJson["SSR"]["fFinal_RayStep"];
	Desc_SSR.fStepCnt = BasicJson["SSR"]["fFinal_StepCnt"];

	SCREENEFFECT_DESC Desc_ScreenEffect = {};
	Desc_ScreenEffect.bGrayScale_Active = BasicJson["Screen"]["GrayActive"];
	Desc_ScreenEffect.bSephia_Active = BasicJson["Screen"]["SephiaActive"];
	Desc_ScreenEffect.GreyPower = BasicJson["Screen"]["GreyPower"];
	Desc_ScreenEffect.SepiaPower = BasicJson["Screen"]["SepiaPower"];

	VIGNETTE_DESC Desc_Vignette = {};
	Desc_Vignette.bVignette_Active = BasicJson["Vignette"]["bFinal_Active"];
	Desc_Vignette.fVignetteRatio = BasicJson["Vignette"]["fFinal_Ratio"];
	Desc_Vignette.fVignetteRadius = BasicJson["Vignette"]["fFinal_Radius"];
	Desc_Vignette.fVignetteAmount =  BasicJson["Vignette"]["fFinal_Amount"];
	Desc_Vignette.fVignetteSlope = BasicJson["Vignette"]["fFinal_Slope"];
	Desc_Vignette.fVignetteCenter_X = BasicJson["Vignette"]["fFinal_CenterX"];
	Desc_Vignette.fVignetteCenter_Y = BasicJson["Vignette"]["fFinal_CenterY"];

	// 셰이더 14개 

	m_pGameInstance->Get_Renderer()->Set_FXAA_Option(Desc_Anti);
	m_pGameInstance->Get_Renderer()->Set_Chroma_Option(Desc_Chroma);
	m_pGameInstance->Get_Renderer()->Set_Deferred_Option(Desc_Deferred);
	m_pGameInstance->Get_Renderer()->Set_DOF_Option(Desc_Dof);
	m_pGameInstance->Get_Renderer()->Set_Fog_Option(Desc_Fog);
	m_pGameInstance->Get_Renderer()->Set_HBAO_Option(Desc_Hbao);
	m_pGameInstance->Get_Renderer()->Set_HDR_Option(Desc_HDR);
	m_pGameInstance->Get_Renderer()->Set_HSV_Option(Desc_HSV);
	m_pGameInstance->Get_Renderer()->Set_LumaSharpen_Option(Desc_Luma);
	m_pGameInstance->Get_Renderer()->Set_PBR_Option(Desc_PBR);
	m_pGameInstance->Get_Renderer()->Set_RadialBlur_Option(Desc_Radial);
	m_pGameInstance->Get_Renderer()->Set_SSR_Option(Desc_SSR);
	m_pGameInstance->Get_Renderer()->Set_ScreenEffect_Option(Desc_ScreenEffect);
	m_pGameInstance->Get_Renderer()->Set_Vignette_Option(Desc_Vignette);

	return S_OK;
}

CLevel_SnowMountain* CLevel_SnowMountain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)

{
	CLevel_SnowMountain* pInstance = new CLevel_SnowMountain(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_SnowMountain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_SnowMountain::Free()
{
	__super::Free();

	if (nullptr != m_pMapEffect)
	{
		m_pMapEffect->End_Effect_ForPool();
		Safe_Release(m_pMapEffect);
		//m_pMapEffect = nullptr;
	}

}