#pragma once

#include "Imgui_Window.h"

BEGIN(Engine)
class CGameObject;
class CCamera;
class CNavigation;
class CCell;
class CLight;
END

BEGIN(Client)
class CEnvironment_Object;
class CEnvironment_Interact;
class CEnvironment_Instance;
class CEnvironment_LightObject;
class CEnvironment_SpecialObject;
class CPlayer;
class CMonster_Character;
class CCamera_Dynamic;
class CMasterCamera;
class CSky;
class CEvent_MosnterSpawnTrigger;
class CEvent_Trigger;
//TODO 추후 추가 class CNPC;

class CWindow_MapTool final : public CImgui_Window
{
private:
	enum class TAP_TYPE { TAB_SINGLE, TAB_LIGHT, TAB_INSTANCE, TAB_SPECIAL, TAB_INTERACT, TAB_ENVIRONMENT, TAB_NORMALMONSTER, TAB_BOSSMONSTER, TAB_NPC, TAB_END };
	enum class MODE_TYPE { MODE_CREATE, MODE_SELECT, MODE_DELETE, MODE_END };
	enum class PICKING_TYPE { PICKING_FIELD, PICKING_MESH, PICKING_INSTANCE, PICKING_NONE, PICKING_END };
	enum class PICKING_MODE { MOUSE_PRESSING, MOUSE_DOWN, MOUSE_UP};
	enum class OBJECTMODE_TYPE { OBJECTMODE_ENVIRONMENT, OBJECTMODE_CHARACTER, OBJECTMODE_NAVIGATION, OBJECTMODE_TRIGGER };
	enum class LIGHT_CREATEMODE { LIGHT_MODE, LIGHT_OBJECTMODE };
	enum class ANIM_TYPE { TYPE_NONANIM, TYPE_ANIM };
	enum class INSTANCE_ALLMOVETYPE { ALLMOVE_X, ALLMOVE_Y, ALLMOVE_Z };
	enum class MAP_KEY_TYPE //! 맵컨테이너 키
	{
		MODEL_SINGLE, MODEL_INSTANCE, MODEL_INTERACT, MODEL_END
	};

	

public:
	struct WINDOW_MAPTOOL_DESC : public ImGuiDESC
	{
	};

protected:
	CWindow_MapTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CWindow_MapTool() = default;


public:
	virtual	HRESULT Initialize() override;
	virtual	void	Tick(_float fTimeDelta) override;	
	virtual void	Render() override;

private:
	virtual	HRESULT		Save_Function(string strPath, string strFileName) override;
	virtual HRESULT		Load_Function(string strPath, string strFileName) override;
			void		Reset_Function();
			void		ObjectMode_Change_For_Reset();
private:
	//!For. Environment
	HRESULT			Ready_ModelTags();
	//!For. Character
	HRESULT			Ready_PrototypeTags();
	
private:
	void			EnvironmentMode_Function();
	void			CharacterMode_Function();
	void			NavigationMode_Function();
	void			TriggerMode_Function();
	

	//!For. Environment
	void			GroundTab_Function();
		void			Ground_CreateTab();
		void			Ground_SelectTab();
		void			Ground_DeleteTab();

	void			LightTab_Function();
		void			Light_CreateTab();
		void			Light_SelectTab();
		void			Light_DeleteTab();

	void			InteractTab_Function();
		void			Interact_CreateTab();
		void			Interact_SelectTab();
		void			Interact_DeleteTab();
		void			Interact_SplineSave();
		void			Interact_SplineLoad();
		void			Interact_SplineFucntion();
		void			Interact_LevelChangeFunction();
		void			Interact_GroupFunction();
		void			Interact_ColliderFunction();
		void			Interact_RotationFunction();
		void			Interact_ArrivalMissonFunction();
		void			Interact_ShowInfoWindow();

	void			SpecialTab_Function();
		void			Special_CreateTab();
		void			Special_SelectTab();
		void			Special_DeleteTab();
	
	void			InstanceTab_Function();
		void			Instance_CreateTab();
		void			Instance_SelectTab();
		void			Instance_DeleteTab();

	//!For. Character
	void			MonsterTab_Function();
		void			Monster_CreateTab();
		void			Monster_SelectTab();
		void			Monster_DeleteTab();

	void			NPC_Tab_Function();
		void			NPC_CreateTab();
		void			NPC_SelectTab();
		void			NPC_DeleteTab();
	

	//!For. Navigation
	void			Navigation_CreateTab();
	void			Navigation_SelectTab();
	void			Navigation_DeleteTab();
	void			Set_CCW(_float3* vPoint);
	void			Reset_NaviPicking();
	void			Find_NearPointPos(_float3* fPickedPos);
	CCell*			Find_NearCell(_float3 fPickedPos);
	void			SaveNavi(string strFullPath);
	void			LoadNavi(string strFullPath);
	void			LoadCells();

	//!For. Trigger
	void			Trigger_CreateTab();
	void			Trigger_SelectTab();
	void			Trigger_DeleteTab();

	

private:
	#ifdef _DEBUG
void			MouseInfo_Window(_float fTimeDelta);
#endif // _DEBUG
	void			FieldWindowMenu();
	void			CameraWindow_Function();
	void			IsCreatePlayer_ReadyCamara();
	
	//!For.Character
	void			Select_CharacterModeType();
	
	//!For. Public
	void			Select_ModeType();
	void			Select_PickingType();
	void			Create_Tab(TAP_TYPE eTabType);
	void			Delete_Tab(TAP_TYPE eTabType);
	
private: //! For. Preveiw_Function
	void			Preview_Function();
	void			Change_PreViewObject(TAP_TYPE eTabType);
	 void			 CreateSinglePreview(); //! 인스턴스 프리뷰 포함
	 void			 CreateInstancePreview();
	 void			 CreateLightPreveiw();
	 void			 CreateSpecialPreview();
	 void			 CreateInteractPreview();
	

	void			Preview_DeadForTabType(TAP_TYPE eTabType);
	void			Preview_RayFollowForTabType(TAP_TYPE eTabType);


private: //! ForCreateFunction 
	void			Picking_Function();

	//!For. Environment
	void			Ground_CreateFunction();
	void			Light_CreateFunction();
	void			Interact_CreateFunction();
	void			Special_CreateFunction();
	
	void			Create_Instance();
	void			Preview_Environment_CreateFunction();
	void			Anim_Environment_CreateFunction();


	//!For. Character
	void			Character_CreateFunction();
		void			Monster_CreateFunction();
		void			Boss_CreateFunction();
		void			NPC_CreateFunction();
	
public:
	void			Add_Monster_ForTrigger(CMonster_Character* pMonster);

private: //!For Guizmo
	
	void			Guizmo_Tick(CGameObject* pPickingObject = nullptr);
	void			Instance_GuizmoTick(_int iIndex, INSTANCE_INFO_DESC* pInstance = nullptr);
	void			Trigger_GuizmoTick(class CEvent_Trigger* pEventTrigger = nullptr);



private:
	ImTextureID		m_pSelectedTexture = { nullptr };

	TAP_TYPE		 m_eTabType = TAP_TYPE::TAB_END;
	MODE_TYPE		 m_eModeType = MODE_TYPE::MODE_END;
	PICKING_TYPE	 m_ePickingType = PICKING_TYPE::PICKING_END;
	PICKING_MODE	 m_ePickingMode = PICKING_MODE::MOUSE_PRESSING;
	OBJECTMODE_TYPE  m_eObjectMode = OBJECTMODE_TYPE::OBJECTMODE_ENVIRONMENT;
	ANIM_TYPE		 m_eAnimType = ANIM_TYPE::TYPE_NONANIM;
	LIGHT_CREATEMODE m_eLightCreateMode = LIGHT_CREATEMODE::LIGHT_MODE;
	_bool			m_bChangeObjectMode = false;



	CPlayer*		m_pPlayer = nullptr;
	CMasterCamera*	m_pToolCamera = { nullptr };
	CSky*			m_pSkybox = { nullptr };
	_float			m_fCamaraSpeed = { 60.f };
	_bool			m_bCreateCamera = false;
	_int			m_iSkyTextureIndex = { 0 };

	

	
private: //!For. Environment
	map<string, MAP_KEY_TYPE>	m_mapNonAnimModelTag;
	map<string, MAP_KEY_TYPE>	m_mapAnimModelTag;

	vector<string>				m_vecSingleModelTag;

	vector<string>				m_vecEnviroModelTag;
	vector<string>				m_vecAnimEnviroModelTag;

	vector<string>				m_vecInteractModelTag;
	vector<string>				m_vecAnimInteractModelTag;

	_bool						m_bShowCreateField = false;

private: //!For. Character
	vector<string>				m_vecMonsterTag;
	vector<string>				m_vecBossTag;
	vector<string>				m_vecNpcTag;

	_uint						m_iSelectCharacterTag = {};
	CGameObject*				m_pPreviewCharacter = { nullptr };

private:   //!For Navigation
	_int						m_iNavigationTargetIndex = 0;
	vector<_float3>				m_vecPickedPoints;
	vector<string>				m_vecPickingListBox;

	_int						m_iNaviListBoxIndex = 0;
	_int						m_iCurrentPickingIndex = 0;
	_int						m_iNaviIndex = 0;
	_int						m_iCellIndex = 0;
	_int						m_iPointIndex = 0;
	_int						m_iNaviPickingIndex = 0;

	_float3						m_fNaviPickingPos = {};
	_float						m_fCombinationRange = 0.25f; //! 결합 범위
	_bool						m_bPickingNaviMode = false;
	vector<CCell*>				m_vecCells;
	vector<string>				m_vecCellIndexs;
	_float3						m_vPickingPoint = {};

	string						m_strNaviFinalSavePath = "";
	_bool						m_bHaveNaviSave = false;
	_uint						m_iSaveNaviIndex = 50;

private:  //!For Trigger
	_int						m_iMonsterSpawnGroupIndex = 0;
	class CEvent_Trigger*		m_pPickingTrigger = {nullptr};
	_char						m_strSelectTriggerNameTag[32] = "";

private:  //! For Public
	_uint							m_iSelectModelTag = 0;
	_bool							m_bAnimType = false;
	_int							m_iShaderPassIndex = { 0 };
	_int							m_iAnimIndex = 0;

private:
		
		CNavigation* m_pNavigation = { nullptr };

private:
	PrimitiveBatch<DirectX::VertexPositionColor>* m_pBatch = { nullptr };
	BasicEffect* m_pEffect = { nullptr };
	ID3D11InputLayout* m_pInputLayOut = { nullptr };
	CBounding_Sphere* m_pBoundingSphere = { nullptr };

private: //! For.Picking
	class	CField* m_pField = { nullptr };
	_float			m_fFieldSizeX = { 20.f };
	_float			m_fFieldSizeZ = { 20.f };

	RAY				m_tWorldRay = {};
	_float3			m_fRayPos = {};
	_float3			m_fMeshPos = {};
	_float3			m_fInstanceMeshPos = {};

	_float			m_fRayUpdateTime = { 0.1f };
	_float			m_fRayUpdateTimeAcc = { 0.f };
	_uint			m_iSelectMeshObjectIndex = 0;
	CGameObject*	m_pPickingObject = { nullptr };
	INSTANCE_INFO_DESC* m_pPickingInstanceInfo = { nullptr };

private: //! For PriviewObject //미리보기용 오브젝트
	CEnvironment_Object*			m_pPreviewObject = {}; //! 미리보기를 위해 클론시킨 오브젝트.
	CEnvironment_Interact*			m_pPreviewInteract = {}; //! 상호작용용 미리보기 오브젝트
	CEnvironment_LightObject*		m_pPreviewLightObject = {};
	CEnvironment_SpecialObject*		m_pPreviewSpecialObject = { nullptr };
	_bool							m_bChange = false;
	
private: //!For.Interact //! 상호작용
	_int							m_eInteractState = 0; //! 전부 이넘 캐스팅해야함
	_int							m_eInteractType = 0;  //! 전부 이넘 캐스팅해야함
	_int							m_eInteractLevel = 0;
	
	_bool							m_bInteractLevelChange = false;

	_float							m_fColliderSizeArray[3] = { 1.f, 1.f, 1.f};
	_float							m_fColliderCenterArray[3] = { 0.f, 1.f, 0.f};

	_float							m_fSelectColliderSizeArray[3] = { 1.f, 1.f, 1.f}; //! 콜라이더 사이즈변경용
	_float							m_fSelectColliderCenterArray[3] = { 0.f, 1.f, 0.f }; //! 콜라이더 센터변경용
	_int							m_iInteractPlayAnimIndex = 0;
	_float3							m_vInteractRootMoveRate = { 1.f, 1.f, 1.f };
	_bool							m_bInteractUseGravity = false;
	_bool							m_bInteractUseSpline = false;
	_bool							m_bInteractUseGroup = false;
	_bool							m_bShowAddInteract = false;
	_int							m_iAddInteractSelectIndex = 0;

	_bool							m_bInteractColliderSetting = false;
	CEnvironment_Interact::ENVIRONMENT_INTERACTOBJECT_DESC m_tSelectInteractDesc = {};

	_float4							m_vArrivalPosition = {};
	_float4							m_vEnablePosition = {};

	_int							m_iInteractGroupIndex = -1;

	map<string, vector<_float4>>	m_mapSplinePoints;
	map<string, _float>				m_mapSplineSpeeds;
	map<string, vector<string>>		m_mapSplineListBox;
	

	_char							m_strSplinePointKeyTag[32] = "";

	vector<_float4>					m_vecSplinePoints;
	vector<string>					m_vecSplineListBox;

	_int							m_iSplinePickingIndex = 0;
	_int							m_iSplineListIndex = 0;
	_int							m_iSplineDivergingCount = 0;

private: //!For.Light//! 라이트
	_int							m_eLightEffectType = 0; //! 전부 이넘 캐스팅
	LIGHT_DESC::TYPE				m_eLightType = LIGHT_DESC::TYPE::TYPE_END;
	_bool							m_bLightEffect = true;
	_float3							m_vLightEffectPos = {};
	
	
private:  //!For.Special 스페셜오브젝트
	_int							m_eSpecialType = 0; //! 이넘 캐스팅용;
	_int							m_iSpecialGroupIndex = 0;
	_int							m_iSpecialBloonMeshIndex = 0;

private: //!For. CreateSpecialObject
	vector<CEnvironment_SpecialObject*> m_vecCreateSpecialObject;
	vector<string>						m_vecCreateSpecialObjectTag;
	_int								m_iCreateSpecialObjectIndex = 0;
	_int								m_iSelectSpecialObjectIndex = 0;
	
private: //!For. CreateLightObject
	vector<CEnvironment_LightObject*>	m_vecCreateLightObject;
	vector<string>						m_vecCreateLightObjectTag = {};
	_int								m_iCreateLightObjectIndex = 0;
	_int								m_iSelectLightObjectIndex = 0;
	LIGHT_DESC							m_tEditLightDesc = {};

	vector<CLight*>						m_vecCreateLight;
	vector<string>						m_vecCreateLightTag;
	_int								m_iSelectLightIndex = 0;

private: //!For. CreateInteractObject
	vector<CEnvironment_Interact*>		m_vecCreateInteractObject = {};
	vector<string>						m_vecCreateInteractObjectTag = {};
	_int								m_vecCreateInteractIndex = 0;

private: //!For. CreateNormalObject
	vector<CEnvironment_Object*>		m_vecCreateObject = {}; //! 생성한 오브젝트
	vector<string>						m_vecCreateObjectTag = {};	
	_int								m_vecCreateObjectIndex = 0;
	_int								m_iCreateObjectIndex = 0;
	_int								m_iSelectObjectIndex = 0;


private: //! For. CreateInstance
	_uint							m_iSelectInstanceIndex = 0;
	_uint							m_iSelectEnvironmentIndex = 0;

	_int							m_iCreatePreviewIndex = 0;
	_int							m_iSelectPreviewIndex = 0;
	vector<CEnvironment_Instance*>	m_vecCreateInstance = {};
	vector<CEnvironment_Object*>	m_vecPreViewInstance = {}; //! 인스턴싱 디스크립션 만들기 위해.
	vector<string>					m_vecPreViewInstanceTag = {};

	map<string, vector<CEnvironment_Object*>> m_mapPreviewInstance; //! 선택한 모델태그마다 각기 다른 벡터에 저장해서 생성하게하자.


	vector<string>					m_vecCreateInstanceTag = {};
	_int							m_iCreateInstanceIndex = 0;

	vector<string>					m_vecInstanceInfoTag = {}; //! m_vecPreViewInstance를 픽킹해서 인스턴싱 디스크립션을 채워 준후 m_vecCreateInstance를 만들어주자
	_int							m_iInstanceInfoTagIndex = 0;

	INSTANCE_ALLMOVETYPE			m_eInstanceAllMoveMode = INSTANCE_ALLMOVETYPE::ALLMOVE_X;
	_float4x4						m_matInstanceMatrix = {};

private: //!For.CreateTrigger
	vector<CEvent_MosnterSpawnTrigger*>	 m_vecCreateMonsterTrigger;
	vector<string>						 m_vecCreateMonsterTriggerTag;
	_int								 m_iSelectMonsterTriggerIndex = 0;
	_int								 m_iSelectMonsterSpawnGroupIndex = 0;


private:  //!For. CreateCharacter
	
	vector<CMonster_Character*>			m_vecCreateMonster;
	vector<string>						m_vecCreateMonsterTag;
	_int								m_iSelectMonsterGroupIndex = 0;
	_int								m_iSelectMonsterNaviIndex = -1;
	_int								m_iCreateMonsterIndex = {};
	_int								m_iSelectCharacterIndex = {};

private:  /*!For.CreateNPC*/	/*TODO 추후 NPC추가되면 작성*/
	//!vector<CNPC*>					m_vecCreateNPC;
	//!vector<string>					m_vecCreateNPCTag ;
	//!_int							m_iCreateNPCIndex = {};

public:
	static CWindow_MapTool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END

