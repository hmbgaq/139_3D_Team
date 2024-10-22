#include "stdafx.h"
#include "GameInstance.h"
#include "Level_IntroBoss.h"

#include "MasterCamera.h"
#include "SpringCamera.h"
#include "Camera_Dynamic.h"

#include "Player.h"
#include "LandObject.h"

#include "Light.h"

#include "Data_Manager.h"

#pragma region UI
#include "UI_Player_HPBar.h"
#include "UI_Anything.h"
#include "UI_MonsterHp.h"
#include "UI_MonsterHpFrame.h"
#include "UI_Manager.h"
#pragma endregion

#pragma region MAP
#include "Environment_Object.h"
#include "Environment_Instance.h"
#include "Environment_LightObject.h"
#include "Environment_SpecialObject.h"
#include "Event_MonsterSpawnTrigger.h"
#pragma endregion

#pragma region Monster
#include "Monster.h"
#include "Screamer.h"  
#include "InstanceMonster.h"
#include "VampireCommander.h"
#include "Monster_Character.h"
#pragma endregion

#pragma region Effect
#include "Effect.h"
#include "Effect_Manager.h"
#include "Effect_Instance.h"
#pragma endregion

#include "Data_Manager.h"
#include "MasterCamera.h"
#include "SpringCamera.h"
#include "Light.h"



#include "Level_Loading.h"

CLevel_IntroBoss::CLevel_IntroBoss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel(pDevice, pContext)
{
}

HRESULT CLevel_IntroBoss::Initialize()
{
    m_pGameInstance->Get_Renderer()->Render_UI_MRT(false);
    m_pGameInstance->Set_CurrentLevel(m_pGameInstance->Get_NextLevel());
    Set_ShaderOption("../Bin/DataFiles/Data_Shader/Level/Level_Intro_Boss_Shader.json");

    FAILED_CHECK(Ready_LightDesc());
    FAILED_CHECK(Ready_Layer_BackGround(TEXT("Layer_BackGround")));
    FAILED_CHECK(Ready_LandObjects());
    FAILED_CHECK(Ready_Layer_Camera(TEXT("Layer_Camera")));
    FAILED_CHECK(Ready_Effect());
    FAILED_CHECK(Ready_UI());
    FAILED_CHECK(Ready_Event());
    
    //m_pGameInstance->Play_BGM(L"BGM", L"IntroBossIntroBGM.wav", 4.f);
    return S_OK;
}

void CLevel_IntroBoss::Tick(_float fTimeDelta)
{

	//if (m_pGameInstance->Key_Down(DIK_GRAVE))
	//{
	//	m_pGameInstance->Request_Level_Opening(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_SNOWMOUNTAIN));
	//}

}

HRESULT CLevel_IntroBoss::Render()
{
    SetWindowText(g_hWnd, TEXT("Level IntroBoss 脊艦陥."));

    return S_OK;
}

//!坪球是帖: Level_IntroBoss / Ready_LightDesc敗呪
//!
//!痕井吉 坪球 採歳 : �言厳� 伊事馬檎 蟹神亀系敗.爽汐生稽 痕井走繊 端滴背客

HRESULT CLevel_IntroBoss::Ready_LightDesc()
{
    /* Shadow Light */
    m_pGameInstance->Add_ShadowLight_View(ECast(LEVEL::LEVEL_INTRO_BOSS), _float4(Engine::g_vLightEye), _float4(Engine::g_vLightAt), _float4(Engine::g_vLightUp));
    m_pGameInstance->Add_ShadowLight_Proj(ECast(LEVEL::LEVEL_INTRO_BOSS), 60.f, (_float)g_iWinSizeX / (_float)g_iWinSizeY, Engine::g_fLightNear, Engine::g_fLightFar);

    m_pGameInstance->Remove_AllLight();

    /* Map Light */
    //CLight* pDirectionalLight = m_pGameInstance->Get_DirectionLight();
    //
    //if (pDirectionalLight != nullptr) //TODO 奄糎拭 巨刑偲確 虞戚闘亜 糎仙梅陥檎.
    //{
    //    m_pGameInstance->Remove_Light(pDirectionalLight->Get_LightIndex());
    //}

    json IntroBossMapJson = {};

    if (FAILED(CJson_Utility::Load_Json(m_strStage1MapLoadPath.c_str(), IntroBossMapJson)))
    {
        MSG_BOX("繕誤 災君神奄 叔鳶");
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
        LightDesc.fIntensity = LightObjectJson[i]["Intensity"]; // �� 食奄 蓄亜喫 

        LightDesc.eType = LightObjectJson[i]["LightType"];
        CJson_Utility::Load_Float4(LightObjectJson[i]["Direction"], LightDesc.vDirection);
        LightDesc.fRange = LightObjectJson[i]["Range"];
        CJson_Utility::Load_Float4(LightObjectJson[i]["Position"], LightDesc.vPosition);
        CJson_Utility::Load_Float4(LightObjectJson[i]["Diffuse"], LightDesc.vDiffuse);
        CJson_Utility::Load_Float4(LightObjectJson[i]["Ambient"], LightDesc.vAmbient);
        CJson_Utility::Load_Float4(LightObjectJson[i]["Specular"], LightDesc.vSpecular);


        LightObjectDesc.LightDesc = LightDesc;

        CEnvironment_LightObject* pLightObject = dynamic_cast<CEnvironment_LightObject*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_INTRO_BOSS, L"Layer_BackGround", L"Prototype_GameObject_Environment_LightObject", &LightObjectDesc));

        if (pLightObject == nullptr)
        {
            MSG_BOX("虞戚闘神崎詮闘 持失叔鳶");
            return E_FAIL;
        }
    }

    //TODO LIGHT_DESC LightDesc = {};
    //TODO LightDesc.bEnable = true;
    //TODO LightDesc.eType = LIGHT_DESC::TYPE::TYPE_SPOTLIGHT;
    //TODO 
    //TODO LightDesc.vDirection = { 0.f, -1.f, 0.f, 0.f };
    //TODO LightDesc.vPosition = _float4(60.0f, 5.f, 29.84f, 0.f);
    //TODO LightDesc.fRange = 3.f;
    //TODO LightDesc.fCutOff = 0.5f;
    //TODO LightDesc.fOuterCutOff = 0.7f;
    //TODO 
    //TODO LightDesc.vDiffuse = { 1.f, 0.f, 0.f, 1.f };
    //TODO LightDesc.vAmbient = { 1.f, 0.f, 0.f, 1.f };
    //TODO LightDesc.vSpecular = { 1.f, 0.f, 0.f, 1.f };
    //TODO LightDesc.fIntensity = 0.5f;
    //TODO 
    //TODO CLight* pLight = m_pGameInstance->Add_Light_AndGet(LightDesc, LightDesc.iLightIndex);



    return S_OK;
}

HRESULT CLevel_IntroBoss::Ready_Layer_Camera(const wstring& strLayerTag)
{
    //CCamera_Dynamic::DYNAMIC_CAMERA_DESC      Desc = {};
    //Desc.fMouseSensor = 0.05f;
    //Desc.vEye = _float4(0.f, 20.f, -15.f, 1.f);
    //Desc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
    //Desc.fFovy = XMConvertToRadians(60.0f);
    //Desc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
    //Desc.fNear = 0.1f;
    //Desc.fFar = m_pGameInstance->Get_CamFar();
    //Desc.fSpeedPerSec = 15.f;
    //Desc.fRotationPerSec = XMConvertToRadians(180.0f);

    //FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_INTRO_BOSS, strLayerTag, TEXT("Prototype_GameObject_Camera_Dynamic"), &Desc));

    if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_INTRO_BOSS, strLayerTag, TEXT("Prototype_GameObject_MasterCamera"))))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_IntroBoss::Ready_Layer_Player(const wstring& strLayerTag, void* pArg)
{
    CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_INTRO_BOSS, strLayerTag, TEXT("Prototype_GameObject_Player"), pArg));

    pPlayer->Set_Position(_float3(60.0f, 0.f, 29.84f));
    CNavigation* pNavigation = pPlayer->Get_Navigation();

    pNavigation->Set_CurrentIndex(pNavigation->Get_SelectRangeCellIndex(pPlayer));

    //FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_INTRO_BOSS, strLayerTag, TEXT("Prototype_GameObject_Rentier"), pArg));


    //CGameObject* pPlayer = m_pGameInstance->Add_CloneObject_And_Get(LEVEL_INTRO_BOSS, strLayerTag, TEXT("Prototype_GameObject_Player"), pArg);
    //if (nullptr == pPlayer)
    //   return E_FAIL;

    //m_pGameInstance->Set_Player(dynamic_cast<CCharacter*>(pPlayer));

    return S_OK;
}

HRESULT CLevel_IntroBoss::Ready_Effect()
{

    _float3 vPos = { 60.f, 0.f, 54.f };
    m_pMapEffect = EFFECT_MANAGER->Play_Effect("VampireCommander/Map_Blood/", "Map_Blood_10.json", nullptr, vPos);

    //FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_INTRO_BOSS, strLayerTag, TEXT("Prototype_GameObject_Particle_Blue")));
    //FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_INTRO_BOSS, strLayerTag, TEXT("Prototype_GameObject_Particle_Red")));
    //FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_INTRO_BOSS, strLayerTag, TEXT("Prototype_GameObject_Effect_Explosion")));
    m_pEffect = EFFECT_MANAGER->Play_Effect("Fog/", "SY_IntroBoss_01.json", nullptr, _float3(0.f, 0.f, 0.f));

    return S_OK;
}

HRESULT CLevel_IntroBoss::Ready_Layer_Monster(const wstring& strLayerTag, void* pArg)
{
    CGameObject* pMonster = { nullptr };

    //FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_INTRO_BOSS, strLayerTag, TEXT("Prototype_GameObject_Assassin")));
    //FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_INTRO_BOSS, strLayerTag, TEXT("Prototype_GameObject_Infected")));
 //    for (int i = 0; i < 100; ++i)
 //    {


    /*   }*/

    {//Layer_Boss
        //pMonster = m_pGameInstance->Add_CloneObject_And_Get(LEVEL_INTRO_BOSS, L"Layer_Boss", TEXT("Prototype_GameObject_VampireCommander"));
        //
        //if (nullptr == pMonster)   return E_FAIL;
        //pMonster->Set_Position(_float3(60.0f, 0.f, 55.f));
        //pMonster->Get_Transform()->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-180.f));
        //
        //CNavigation* pVampireNavi = dynamic_cast<CVampireCommander*>(pMonster)->Get_Navigation();
        //
        //pVampireNavi->Set_CurrentIndex(pVampireNavi->Get_SelectRangeCellIndex(pMonster));
    }

    return S_OK;
}

HRESULT CLevel_IntroBoss::Ready_Layer_BackGround(const wstring& strLayerTag)
{
    //FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_INTRO_BOSS, strLayerTag, TEXT("Prototype_GameObject_Terrain")));
    FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_INTRO_BOSS, strLayerTag, TEXT("Prototype_GameObject_Sky")));

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

        CEnvironment_Object* pObject = { nullptr };

        pObject = dynamic_cast<CEnvironment_Object*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_INTRO_BOSS, L"Layer_BackGround", L"Prototype_GameObject_Environment_Object", &Desc));
    }


    json InteractJson = Stage1MapJson["Interact_Json"];
    _int InteractJsonSize = (_int)InteractJson.size();

    for (_int i = 0; i < InteractJsonSize; ++i)
    {

        //TODO 蓄板 雌硲拙遂 神崎詮闘 適掘什 拙失  板 拙穣
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

        pInstanceObject = dynamic_cast<CEnvironment_Instance*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_INTRO_BOSS, L"Layer_BackGround", L"Prototype_GameObject_Environment_Instance", &InstanceDesc));

    }

    //json MonsterJson = Stage1MapJson["Monster_Json"];
    //_int iMonsterJsonSize = (_int)MonsterJson.size();
    //
    //for (_int i = 0; i < iMonsterJsonSize; ++i)
    //{
    //    CMonster_Character::MONSTER_DESC MonsterDesc = {};
    //
    //    string LoadMonsterTag = (string(MonsterJson[i]["PrototypeTag"]));
    //
    //    m_pGameInstance->String_To_WString(LoadMonsterTag, MonsterDesc.strProtoTypeTag);
    //    MonsterDesc.bPreview = false;
    //    MonsterDesc.eDescType = CGameObject::MONSTER_DESC;
    //
    //    const json& TransformJson = MonsterJson[i]["Component"]["Transform"];
    //    _float4x4 WorldMatrix;
    //
    //    for (_int TransformLoopIndex = 0; TransformLoopIndex < 4; ++TransformLoopIndex)
    //    {
    //        for (_int TransformSecondLoopIndex = 0; TransformSecondLoopIndex < 4; ++TransformSecondLoopIndex)
    //        {
    //            WorldMatrix.m[TransformLoopIndex][TransformSecondLoopIndex] = TransformJson[TransformLoopIndex][TransformSecondLoopIndex];
    //        }
    //    }
    //
    //    MonsterDesc.WorldMatrix = WorldMatrix;
    //
    //    if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_INTRO_BOSS, L"Layer_Monster", MonsterDesc.strProtoTypeTag, &MonsterDesc)))
    //        return E_FAIL;
    //
    //}

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
    //
    //
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

HRESULT CLevel_IntroBoss::Ready_LandObjects()
{
    CLandObject::LANDOBJECT_DESC   LandObjectDesc{};

    LandObjectDesc.pTerrainBuffer = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Get_Component(LEVEL_INTRO_BOSS, TEXT("Layer_BackGround"), TEXT("Com_VIBuffer")));
    LandObjectDesc.pTerrainTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_INTRO_BOSS, TEXT("Layer_BackGround"), TEXT("Com_Transform")));

    FAILED_CHECK(Ready_Layer_Player(TEXT("Layer_Player"), &LandObjectDesc));
    FAILED_CHECK(Ready_Layer_Monster(TEXT("Layer_Monster"), &LandObjectDesc));


    return S_OK;
}


HRESULT CLevel_IntroBoss::Ready_Layer_Test(const wstring& strLayerTag)
{
    //FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_INTRO_BOSS, strLayerTag, TEXT("Prototype_GameObject_Interact_Chain")));
    //! 蕉還昔什渡縮戚 拝琶推蒸嬢像. ぞぞぞぞぞぞFAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_INTRO_BOSS, strLayerTag, TEXT("Prototype_GameObject_InstanceMonster")));

    //FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_INTRO_BOSS, strLayerTag, TEXT("Prototype_GameObject_Screamer")));

    ///* ui test */
    //m_pGameInstance->Add_CloneObject(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_UI_Player_HPBar"));

    //CUI_Player_HPBar::UI_DESC desc = {};
    //desc.fPositionX = (_float)g_iWinSizeX / 2 + 20.f;
    //desc.fPositionY = (_float)g_iWinSizeY / 2 + 20.f;
    //m_pGameInstance->Add_CloneObject(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_UI_Player_HPBar"), &desc);

    return S_OK;
}


HRESULT CLevel_IntroBoss::Ready_UI()
{
    // FAILED_CHECK(Ready_Layer_UI_Monster(TEXT("Layer_UI_Monster"), nullptr));

    FAILED_CHECK(Ready_Layer_UI_Interface(TEXT("Layer_UI_Interface"), nullptr));
    //
    //FAILED_CHECK(Ready_Layer_UI(TEXT("Layer_UI"), nullptr));

    return S_OK;
}

HRESULT CLevel_IntroBoss::Ready_Layer_UI_Monster(const wstring& strLayerTag, void* pArg)
{
    /* 蓄 板 督縮背辞 舛左 閤焼神奄 */

    /* MRT稽 広走 省生檎 窒径戚照蟹身. */
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

        tUI_Info.strProtoTag = object["ProtoTag"];
        tUI_Info.strFilePath = object["FilePath"];

        wstring wstrPrototag;
        m_pGameInstance->String_To_WString(tUI_Info.strProtoTag, wstrPrototag);

        wstring wstrFilePath;
        m_pGameInstance->String_To_WString(tUI_Info.strFilePath, wstrFilePath);

        CUI_Anything* pUI_Object = dynamic_cast<CUI_Anything*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_UI_Anything"), &tUI_Info));

        pUI_Object->Get_Transform()->Load_FromJson(object);

        // Pos 説焼爽奄
    }


    //if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_UI_Anything"), &json_in)))
    //   return E_FAIL;

    return S_OK;
}

HRESULT CLevel_IntroBoss::Ready_Layer_UI_Interface(const wstring& strLayerTag, void* pArg)
{

    return S_OK;
}

HRESULT CLevel_IntroBoss::Ready_LeftInterface(const wstring& strLayerTag, void* pArg)
{
    json json_in;

    char filePath[MAX_PATH] = "../Bin/DataFiles/Data_UI/PlayerInterface/Left_Interface.json";

    _int      iPathNum = 0;
    string      strFileName;
    string      strFilePath;

    CJson_Utility::Load_Json(filePath, json_in);

    for (auto& item : json_in.items())
    {
        json object = item.value();

        CUI::UI_DESC tUI_Info;

        /* 煽舌授辞櫛 限澗走 溌昔馬奄 */
        tUI_Info.bParent = object["Parent"];               // 1. Parent
        tUI_Info.bWorld = object["World"];                  // 2. World
        tUI_Info.bGroup = object["Group"];                  // 3. Group
        tUI_Info.fAlpha = object["Alpha"];                  // 4. Alpha
        tUI_Info.iObjectNum = object["ObjectNum"];            // 5. ObjectNum
        tUI_Info.iShaderNum = object["ShaderNum"];            // 6. ShaderPathNum
        tUI_Info.strObjectName = object["ObjectName"];         // 7. ObjectName
        tUI_Info.strLayerTag = object["LayerTag"];            // 8. LayerTag
        tUI_Info.strCloneTag = object["CloneTag"];            // 9. CloneTag
        tUI_Info.strProtoTag = object["ProtoTag"];            // 10. ProtoTag
        tUI_Info.strFilePath = object["FilePath"];            // 11. FilePath
        tUI_Info.strMapTextureTag = object["MapTextureTag"];   // 12. MapTexture
        tUI_Info.vColor.m128_f32[0] = object["ColorR"];         // 13. R
        tUI_Info.vColor.m128_f32[1] = object["ColorG"];         // 14. G
        tUI_Info.vColor.m128_f32[2] = object["ColorB"];         // 15. B
        tUI_Info.vColor.m128_f32[3] = object["ColorA"];         // 16. A

        wstring wstrLayer;
        m_pGameInstance->String_To_WString(tUI_Info.strLayerTag, wstrLayer); //

        wstring wstrClonetag;
        m_pGameInstance->String_To_WString(tUI_Info.strCloneTag, wstrClonetag);

        wstring wstrPrototag;
        m_pGameInstance->String_To_WString(tUI_Info.strProtoTag, wstrPrototag);

        wstring wstrFilePath;
        m_pGameInstance->String_To_WString(tUI_Info.strFilePath, wstrFilePath);

        CUI* pUI_Object = dynamic_cast<CUI*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_STATIC, strLayerTag, wstrClonetag, &tUI_Info));

        pUI_Object->Get_Transform()->Load_FromJson(object);      // 17. TransformCom
    }

    return S_OK;
}

HRESULT CLevel_IntroBoss::Ready_RightInterface(const wstring& strLayerTag, void* pArg)
{
    json json_in;

    char filePath[MAX_PATH] = "../Bin/DataFiles/Data_UI/PlayerInterface/Right_Interface.json";

    _int      iPathNum = 0;
    string      strFileName;
    string      strFilePath;

    CJson_Utility::Load_Json(filePath, json_in);

    for (auto& item : json_in.items())
    {
        json object = item.value();

        CUI::UI_DESC tUI_Info;

        /* 煽舌授辞櫛 限澗走 溌昔馬奄 */
        tUI_Info.bParent = object["Parent"];               // 1. Parent
        tUI_Info.bWorld = object["World"];                  // 2. World
        tUI_Info.bGroup = object["Group"];                  // 3. Group
        tUI_Info.fAlpha = object["Alpha"];                  // 4. Alpha
        tUI_Info.iObjectNum = object["ObjectNum"];            // 5. ObjectNum
        tUI_Info.iShaderNum = object["ShaderNum"];            // 6. ShaderPathNum
        tUI_Info.strObjectName = object["ObjectName"];         // 7. ObjectName
        tUI_Info.strLayerTag = object["LayerTag"];            // 8. LayerTag
        tUI_Info.strCloneTag = object["CloneTag"];            // 9. CloneTag
        tUI_Info.strProtoTag = object["ProtoTag"];            // 10. ProtoTag
        tUI_Info.strFilePath = object["FilePath"];            // 11. FilePath
        tUI_Info.strMapTextureTag = object["MapTextureTag"];   // 12. MapTexture
        tUI_Info.vColor.m128_f32[0] = object["ColorR"];         // 13. R
        tUI_Info.vColor.m128_f32[1] = object["ColorG"];         // 14. G
        tUI_Info.vColor.m128_f32[2] = object["ColorB"];         // 15. B
        tUI_Info.vColor.m128_f32[3] = object["ColorA"];         // 16. A

        wstring wstrLayer;
        m_pGameInstance->String_To_WString(tUI_Info.strLayerTag, wstrLayer); //

        wstring wstrClonetag;
        m_pGameInstance->String_To_WString(tUI_Info.strCloneTag, wstrClonetag);

        wstring wstrPrototag;
        m_pGameInstance->String_To_WString(tUI_Info.strProtoTag, wstrPrototag);

        wstring wstrFilePath;
        m_pGameInstance->String_To_WString(tUI_Info.strFilePath, wstrFilePath);

        CUI* pUI_Object = dynamic_cast<CUI*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_STATIC, strLayerTag, wstrClonetag, &tUI_Info));

        pUI_Object->Get_Transform()->Load_FromJson(object);      // 17. TransformCom
    }

    return S_OK;
}

HRESULT CLevel_IntroBoss::Ready_QuestBox(const wstring& strLayerTag, void* pArg)
{
    json json_in;

    char filePath[MAX_PATH] = "../Bin/DataFiles/Data_UI/PlayerInterface/QuestBox.json";

    _int      iPathNum = 0;
    string      strFileName;
    string      strFilePath;

    CJson_Utility::Load_Json(filePath, json_in);

    for (auto& item : json_in.items())
    {
        json object = item.value();

        CUI::UI_DESC tUI_Info;

        /* 煽舌授辞櫛 限澗走 溌昔馬奄 */
        tUI_Info.bParent = object["Parent"];               // 1. Parent
        tUI_Info.bWorld = object["World"];                  // 2. World
        tUI_Info.bGroup = object["Group"];                  // 3. Group
        tUI_Info.fAlpha = object["Alpha"];                  // 4. Alpha
        tUI_Info.iObjectNum = object["ObjectNum"];            // 5. ObjectNum
        tUI_Info.iShaderNum = object["ShaderNum"];            // 6. ShaderPathNum
        tUI_Info.strObjectName = object["ObjectName"];         // 7. ObjectName
        tUI_Info.strLayerTag = object["LayerTag"];            // 8. LayerTag
        tUI_Info.strCloneTag = object["CloneTag"];            // 9. CloneTag
        tUI_Info.strProtoTag = object["ProtoTag"];            // 10. ProtoTag
        tUI_Info.strFilePath = object["FilePath"];            // 11. FilePath
        tUI_Info.strMapTextureTag = object["MapTextureTag"];   // 12. MapTexture
        tUI_Info.vColor.m128_f32[0] = object["ColorR"];         // 13. R
        tUI_Info.vColor.m128_f32[1] = object["ColorG"];         // 14. G
        tUI_Info.vColor.m128_f32[2] = object["ColorB"];         // 15. B
        tUI_Info.vColor.m128_f32[3] = object["ColorA"];         // 16. A

        wstring wstrLayer;
        m_pGameInstance->String_To_WString(tUI_Info.strLayerTag, wstrLayer); //

        wstring wstrClonetag;
        m_pGameInstance->String_To_WString(tUI_Info.strCloneTag, wstrClonetag);

        wstring wstrPrototag;
        m_pGameInstance->String_To_WString(tUI_Info.strProtoTag, wstrPrototag);

        wstring wstrFilePath;
        m_pGameInstance->String_To_WString(tUI_Info.strFilePath, wstrFilePath);

        CUI* pUI_Object = dynamic_cast<CUI*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_STATIC, strLayerTag, wstrClonetag, &tUI_Info));

        pUI_Object->Get_Transform()->Load_FromJson(object);      // 17. TransformCom
    }

    return S_OK;
}

HRESULT CLevel_IntroBoss::Ready_TutorialBox(const wstring& strLayerTag, void* pArg)
{
    json json_in;

    char filePath[MAX_PATH] = "../Bin/DataFiles/Data_UI/PlayerInterface/TutorialBox.json";

    _int      iPathNum = 0;
    string      strFileName;
    string      strFilePath;

    CJson_Utility::Load_Json(filePath, json_in);

    for (auto& item : json_in.items())
    {
        json object = item.value();

        CUI::UI_DESC tUI_Info;

        /* 煽舌授辞櫛 限澗走 溌昔馬奄 */
        tUI_Info.bParent = object["Parent"];               // 1. Parent
        tUI_Info.bWorld = object["World"];                  // 2. World
        tUI_Info.bGroup = object["Group"];                  // 3. Group
        tUI_Info.fAlpha = object["Alpha"];                  // 4. Alpha
        tUI_Info.iObjectNum = object["ObjectNum"];            // 5. ObjectNum
        tUI_Info.iShaderNum = object["ShaderNum"];            // 6. ShaderPathNum
        tUI_Info.strObjectName = object["ObjectName"];         // 7. ObjectName
        tUI_Info.strLayerTag = object["LayerTag"];            // 8. LayerTag
        tUI_Info.strCloneTag = object["CloneTag"];            // 9. CloneTag
        tUI_Info.strProtoTag = object["ProtoTag"];            // 10. ProtoTag
        tUI_Info.strFilePath = object["FilePath"];            // 11. FilePath
        tUI_Info.strMapTextureTag = object["MapTextureTag"];   // 12. MapTexture
        tUI_Info.vColor.m128_f32[0] = object["ColorR"];         // 13. R
        tUI_Info.vColor.m128_f32[1] = object["ColorG"];         // 14. G
        tUI_Info.vColor.m128_f32[2] = object["ColorB"];         // 15. B
        tUI_Info.vColor.m128_f32[3] = object["ColorA"];         // 16. A

        wstring wstrLayer;
        m_pGameInstance->String_To_WString(tUI_Info.strLayerTag, wstrLayer); //

        wstring wstrClonetag;
        m_pGameInstance->String_To_WString(tUI_Info.strCloneTag, wstrClonetag);

        wstring wstrPrototag;
        m_pGameInstance->String_To_WString(tUI_Info.strProtoTag, wstrPrototag);

        wstring wstrFilePath;
        m_pGameInstance->String_To_WString(tUI_Info.strFilePath, wstrFilePath);

        CUI* pUI_Object = dynamic_cast<CUI*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_STATIC, strLayerTag, wstrClonetag, &tUI_Info));

        pUI_Object->Get_Transform()->Load_FromJson(object);      // 17. TransformCom
    }

    return S_OK;
}

HRESULT CLevel_IntroBoss::Ready_LevelUp(const wstring& strLayerTag, void* pArg)
{
    json json_in;

    char filePath[MAX_PATH] = "../Bin/DataFiles/Data_UI/PlayerInterface/LevelUp.json";

    _int      iPathNum = 0;
    string      strFileName;
    string      strFilePath;

    CJson_Utility::Load_Json(filePath, json_in);

    for (auto& item : json_in.items())
    {
        json object = item.value();

        CUI::UI_DESC tUI_Info;

        /* 煽舌授辞櫛 限澗走 溌昔馬奄 */
        tUI_Info.bParent = object["Parent"];               // 1. Parent
        tUI_Info.bWorld = object["World"];                  // 2. World
        tUI_Info.bGroup = object["Group"];                  // 3. Group
        tUI_Info.fAlpha = object["Alpha"];                  // 4. Alpha
        tUI_Info.iObjectNum = object["ObjectNum"];            // 5. ObjectNum
        tUI_Info.iShaderNum = object["ShaderNum"];            // 6. ShaderPathNum
        tUI_Info.strObjectName = object["ObjectName"];         // 7. ObjectName
        tUI_Info.strLayerTag = object["LayerTag"];            // 8. LayerTag
        tUI_Info.strCloneTag = object["CloneTag"];            // 9. CloneTag
        tUI_Info.strProtoTag = object["ProtoTag"];            // 10. ProtoTag
        tUI_Info.strFilePath = object["FilePath"];            // 11. FilePath
        tUI_Info.strMapTextureTag = object["MapTextureTag"];   // 12. MapTexture
        tUI_Info.vColor.m128_f32[0] = object["ColorR"];         // 13. R
        tUI_Info.vColor.m128_f32[1] = object["ColorG"];         // 14. G
        tUI_Info.vColor.m128_f32[2] = object["ColorB"];         // 15. B
        tUI_Info.vColor.m128_f32[3] = object["ColorA"];         // 16. A

        wstring wstrLayer;
        m_pGameInstance->String_To_WString(tUI_Info.strLayerTag, wstrLayer); //

        wstring wstrClonetag;
        m_pGameInstance->String_To_WString(tUI_Info.strCloneTag, wstrClonetag);

        wstring wstrPrototag;
        m_pGameInstance->String_To_WString(tUI_Info.strProtoTag, wstrPrototag);

        wstring wstrFilePath;
        m_pGameInstance->String_To_WString(tUI_Info.strFilePath, wstrFilePath);

        CUI* pUI_Object = dynamic_cast<CUI*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_STATIC, strLayerTag, wstrClonetag, &tUI_Info));

        pUI_Object->Get_Transform()->Load_FromJson(object);      // 17. TransformCom
    }

    return S_OK;
}

HRESULT CLevel_IntroBoss::Ready_Reward_Item(const wstring& strLayerTag, void* pArg)
{
    json json_in;

    char filePath[MAX_PATH] = "../Bin/DataFiles/Data_UI/PlayerInterface/RewardItem.json";

    _int      iPathNum = 0;
    string      strFileName;
    string      strFilePath;

    CJson_Utility::Load_Json(filePath, json_in);

    for (auto& item : json_in.items())
    {
        json object = item.value();

        CUI::UI_DESC tUI_Info;

        /* 煽舌授辞櫛 限澗走 溌昔馬奄 */
        tUI_Info.bParent = object["Parent"];               // 1. Parent
        tUI_Info.bWorld = object["World"];                  // 2. World
        tUI_Info.bGroup = object["Group"];                  // 3. Group
        tUI_Info.fAlpha = object["Alpha"];                  // 4. Alpha
        tUI_Info.iObjectNum = object["ObjectNum"];            // 5. ObjectNum
        tUI_Info.iShaderNum = object["ShaderNum"];            // 6. ShaderPathNum
        tUI_Info.strObjectName = object["ObjectName"];         // 7. ObjectName
        tUI_Info.strLayerTag = object["LayerTag"];            // 8. LayerTag
        tUI_Info.strCloneTag = object["CloneTag"];            // 9. CloneTag
        tUI_Info.strProtoTag = object["ProtoTag"];            // 10. ProtoTag
        tUI_Info.strFilePath = object["FilePath"];            // 11. FilePath
        tUI_Info.strMapTextureTag = object["MapTextureTag"];   // 12. MapTexture
        tUI_Info.vColor.m128_f32[0] = object["ColorR"];         // 13. R
        tUI_Info.vColor.m128_f32[1] = object["ColorG"];         // 14. G
        tUI_Info.vColor.m128_f32[2] = object["ColorB"];         // 15. B
        tUI_Info.vColor.m128_f32[3] = object["ColorA"];         // 16. A

        wstring wstrLayer;
        m_pGameInstance->String_To_WString(tUI_Info.strLayerTag, wstrLayer); //

        wstring wstrClonetag;
        m_pGameInstance->String_To_WString(tUI_Info.strCloneTag, wstrClonetag);

        wstring wstrPrototag;
        m_pGameInstance->String_To_WString(tUI_Info.strProtoTag, wstrPrototag);

        wstring wstrFilePath;
        m_pGameInstance->String_To_WString(tUI_Info.strFilePath, wstrFilePath);

        CUI* pUI_Object = dynamic_cast<CUI*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_STATIC, strLayerTag, wstrClonetag, &tUI_Info));

        pUI_Object->Get_Transform()->Load_FromJson(object);      // 17. TransformCom
    }

    return S_OK;
}

HRESULT CLevel_IntroBoss::Ready_Layer_UI(const wstring& strLayerTag, void* pArg)
{
    return S_OK;
}

HRESULT CLevel_IntroBoss::Ready_Shader()
{
    /* For. Shadow */
   
    /* 1. 捨戚希 段奄鉢 */
    m_pGameInstance->Off_Shader();

    /* 2. 捨戚希 実特 */
    PBR_DESC Desc_PBR = {};
    Desc_PBR.fBrightnessOffset = 0.f;
    Desc_PBR.fSaturationOffset = 1.0f;

    DEFERRED_DESC Desc_Deferred = {};

    HBAO_PLUS_DESC Desc_Hbao = {};
    Desc_Hbao.fRadius = 2.769f;
    Desc_Hbao.fBias = 0.249f;
    Desc_Hbao.fPowerExponent = 1.689f;
    Desc_Hbao.fBlur_Sharpness = 13.0f;

    FOG_DESC Desc_Fog = {};
    Desc_Fog.fFogStartDepth = 92.805f;
    Desc_Fog.fFogStartDistance = 0.822f;
    Desc_Fog.fFogDistanceValue = 29.025f;
    Desc_Fog.fFogHeightValue = 50.f;
    Desc_Fog.fFogDistanceDensity = 0.025f;
    Desc_Fog.fFogHeightDensity = 0.011f;
    Desc_Fog.vFogColor.x = 0.239f;
    Desc_Fog.vFogColor.y = 0.251f;
    Desc_Fog.vFogColor.z = 0.266f;
    Desc_Fog.vFogColor.w = 1.f;

    RADIAL_DESC Desc_Radial = {};
    Desc_Radial.fRadial_Quality = 8.f;
    Desc_Radial.fRadial_Power = 0.1f;

    DOF_DESC Desc_Dof = {};
    Desc_Dof.DOF_Distance = 4.f;

    HDR_DESC Desc_HDR = {};
    Desc_HDR.fmax_white = 0.4f;

    ANTI_DESC Desc_Anti = {};

    HSV_DESC Desc_HSV = {};
    Desc_HSV.fFinal_Brightness = 1.0f;
    Desc_HSV.fFinal_Saturation = 1.0f;

    VIGNETTE_DESC Desc_Vignette = {};
    Desc_Vignette.fVignetteRatio = 1.902f;
    Desc_Vignette.fVignetteRadius = 1.215f;
    Desc_Vignette.fVignetteAmount = -1.0f;
    Desc_Vignette.fVignetteSlope = 8.f;
    Desc_Vignette.fVignetteCenter_X = 0.5f;
    Desc_Vignette.fVignetteCenter_Y = 0.5f;

    SSR_DESC Desc_SSR = {};
    Desc_SSR.fRayStep = 0.005f;
    Desc_SSR.fStepCnt = 75.f;

    CHROMA_DESC	Desc_Chroma = {};
    Desc_Chroma.fChromaticIntensity = 11.f;

    SCREENEFFECT_DESC Desc_ScreenEffect = {};
    Desc_ScreenEffect.GreyPower = 0.11f;
    Desc_ScreenEffect.SepiaPower = 0.58f;

    LUMASHARPEN_DESC Desc_Luma = {};
    Desc_Luma.foffset_bias = 1.873f;
    Desc_Luma.fsharp_clamp = 0.05f;
    Desc_Luma.fsharp_strength = 1.193f;

    Desc_PBR.bPBR_ACTIVE = true;
    Desc_Deferred.bRimBloom_Blur_Active = true;
    Desc_Deferred.bShadow_Active = true;
    Desc_Hbao.bHBAO_Active = true;
    Desc_Fog.bFog_Active = false;

    Desc_SSR.bSSR_Active = false;
    Desc_Radial.bRadial_Active = false;
    Desc_HDR.bHDR_Active = true;
    Desc_Dof.bDOF_Active = false;
    Desc_Anti.bFXAA_Active = true;
    Desc_HSV.bScreen_Active = true;
    Desc_Vignette.bVignette_Active = false;
    Desc_Chroma.bChroma_Active = false;
    Desc_Luma.bLumaSharpen_Active = true;
    Desc_ScreenEffect.bGrayScale_Active = false;
    Desc_ScreenEffect.bSephia_Active = false;

    m_pGameInstance->Get_Renderer()->Set_PBR_Option(Desc_PBR);
    m_pGameInstance->Get_Renderer()->Set_Deferred_Option(Desc_Deferred);
    m_pGameInstance->Get_Renderer()->Set_HBAO_Option(Desc_Hbao);
    m_pGameInstance->Get_Renderer()->Set_Fog_Option(Desc_Fog);
    m_pGameInstance->Get_Renderer()->Set_RadialBlur_Option(Desc_Radial);
    m_pGameInstance->Get_Renderer()->Set_DOF_Option(Desc_Dof);
    m_pGameInstance->Get_Renderer()->Set_HDR_Option(Desc_HDR);
    m_pGameInstance->Get_Renderer()->Set_FXAA_Option(Desc_Anti);
    m_pGameInstance->Get_Renderer()->Set_HSV_Option(Desc_HSV);
    m_pGameInstance->Get_Renderer()->Set_Vignette_Option(Desc_Vignette);
    m_pGameInstance->Get_Renderer()->Set_SSR_Option(Desc_SSR);
    m_pGameInstance->Get_Renderer()->Set_Chroma_Option(Desc_Chroma);
    m_pGameInstance->Get_Renderer()->Set_ScreenEffect_Option(Desc_ScreenEffect);
    m_pGameInstance->Get_Renderer()->Set_LumaSharpen_Option(Desc_Luma);


    return S_OK;
}
HRESULT CLevel_IntroBoss::Ready_Event()
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


    return S_OK;
}
CLevel_IntroBoss* CLevel_IntroBoss::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)

{
    CLevel_IntroBoss* pInstance = new CLevel_IntroBoss(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CLEVEL_INTRO_BOSSBoss");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CLevel_IntroBoss::Free()
{
    __super::Free();


    if (nullptr != m_pMapEffect)
    {
        m_pMapEffect->End_Effect_ForPool();
        Safe_Release(m_pMapEffect);
        //m_pMapEffect = nullptr;
    }	

    if (nullptr != m_pEffect)
    {
        m_pEffect->End_Effect_ForPool();
        Safe_Release(m_pEffect);
        //m_pEffect = nullptr;
    }

}