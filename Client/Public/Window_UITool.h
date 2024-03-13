#pragma once

#include "Imgui_Window.h"
#include "UI_Anything.h"
#include "UI_TutorialBox.h"

BEGIN(Engine)
class CGameObject;
END

//class CUI_TutorialBox;


BEGIN(Client)
class CWindow_UITool final : public CImgui_Window
{
public: /* ==================== Struct ==================== */
	struct WINDOW_UITOOL_DESC : public ImGuiDESC
	{
	};

	enum CHANGETYPE { NONE, SCALE, ROTATION, POSITION, CHANGE_END };

	enum UITYPE { CHILD, PARENT, GROUP, TEXT, TYPE_END };

	enum TOOLSTATE { TOOL_UI, TOOL_ANIMATION, TOOL_END };

	typedef struct tagImageInfo
	{
		_int						iImage_Width = 100;
		_int						iImage_Height = 100;

		_int						iTextureNum = -1;

		ID3D11ShaderResourceView* SRV_Texture = NULL;
	}IMAGEINFO;

public:
	// SH_Add
	typedef struct tagPathInfo
	{
		_int		iPathNum = 0;
		string		strObjectName = "";
		string		strFileName = "";
		string		strFilePath = "";
	}PATHINFO;

public:
	typedef struct tagUIBaseDesc
	{
		//			위치 X,Y / 사이즈 X,Y
		_float		fX, fY, fSizeX, fSizeY;

		_float		fTexSizeX = 0.f, fTexSizeY = 0.f;

		_float		fOffsetX = 0.f, fOffsetY = 0.f;

		_bool		bWorldUI = false;	// 월드상의 UI인지
		_bool		bFrame = false;		// Frame
		_bool		bEnable = true;		// Enable 여부
	}UI_DESC;

protected: /* ============= Create/Extinction ============== */
	CWindow_UITool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CWindow_UITool() = default;


public: /* ===================== Basic =======================*/
	virtual	HRESULT				Initialize() override;
	virtual	void				Tick(_float fTimeDelta) override;
	virtual void				Render() override;

public: /* ====================== UI ========================= */



	void						UI_ToolTip(_float fTimeDelta);
	std::vector<unsigned char>	UI_LoadImage(const std::string& filename, int& width, int& height, int& channels);
	void						ShowImagePreview(const std::vector<unsigned char>& imageData, int width, int height);
	bool						LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height);
	//HRESULT						Update_Pos();

public: /* ==================== Shortcut_Key ================= */
	void						Shortcut_Key(_float fTimeDelta);

public: /* ==================== List List List List List List List List List List List List List List List List List List List List List List List List List List List List List List List ===================== */
	void						Layer_List();
	void						Texture_List();
	void						Parent_List(_float fTimeDelta);
	void						Child_List(_float fTimeDelta);

#pragma region	Parent
public:							/* Setting */
	void						Setting_Parent();
public:						    /* Create/Delete */
	HRESULT						Create_Parent(CUI::UI_DESC pUIDesc);
	void						Delete_Parent();
public:							/* List */
	void						Parent_Class(_float fTimeDelta);
	void						Parent_Object(_float fTimeDelta);
public:							/* Add */
	void						Add_ParentList(CUI::UI_DESC tIn_UI_Desc);
	void						Add_ParentIndexNumber(PATHINFO& UI_Info);
	void						Add_Create_Parts(CUI::UI_DESC tUIDesc);
public:							/* Group */
	void						Add_Parts(CUI* pUI);
	void						Delete_Group();

private:						/* Member */
	_float3						m_fParent_Scale = { 100.f, 100.f, 100.f };
	_float3						m_fParent_Rotation = { 0.f, 0.f, 0.f };
	_float3						m_fParent_Position = { 0.f, 0.f, 0.5f };

	_int						m_iSelected_ParentClassIndex = 0; // 선택된 Parent Class
	_int						m_iSelected_ParentObjectIndex = 0; // 선택된 Parent Object
	_int						m_iSelected_GroupObjectIndex = 0; // 선택된 Parent Object

	CUI::UI_DESC				m_tParent_Desc;
	CUI*						m_pCurrParent = nullptr;
	CUI*						m_pCurrGroup = nullptr;

	vector<CGameObject*>		m_vecParentObject;
	vector<CUI*>*				m_vecParentGroup = nullptr;
	std::vector<PATHINFO*>		m_vecParentObjectName;	// 추가된 Parent오브젝트들의 이름 (리스트 박스 출력용)
#pragma endregion Parent End


#pragma region	Child
public:							/* Setting */
	void						Setting_Child();
public:							/* Create/Delete */
	HRESULT						Create_Child(CUI::UI_DESC pUIDesc);
	void						Delete_Child(_float fTimeDelta);
	/* List */
	void						Child_Class(_float fTimeDelta);
	void						Child_Object(_float fTimeDelta);
public:							/* Add */
	void						Add_ChildIndexNumber(PATHINFO& str);
	void						Add_ChildList(CUI::UI_DESC tIn_UI_Desc);

private:						/* Member */
	_float3						m_fChild_Scale = { 0.f, 0.f, 0.f };
	_float3						m_fChild_Rotation = { 0.f, 0.f, 0.f };
	_float3						m_fChild_Possition = { 0.f, 0.f, 0.f };

	_int						m_iSelected_ChildClassIndex = 0;	// 선택된 Child Class
	_int						m_iSelected_ChildObjectIndex = 0; // 선택된 Child Object

	CUI::UI_DESC				m_tChild_Desc;
	CUI*						m_pCurrChild = nullptr;

	vector<CGameObject*>		m_vecChildObject;
	//std::vector<PATHINFO*>		m_vecChildObjectName;	// 추가된 오브젝트들의 이름 (리스트 박스 출력용)
#pragma endregion Child End


#pragma region	Info
public:
	void						UI_Info();
	HRESULT						Menu_Info();
	void						Curr_Info();
	CUI* m_pCurrSelectUI = nullptr;
#pragma endregion Info End

#pragma region	Common
public:
	void						IndexCheck();
	void						Current_Info();
	void						Create_TargetTexture();
	void						Load_Paths();
#pragma endregion Common End

#pragma region	Text
public:
	void						Text_List();
	void						Text_Add();
	void						Find_Change(string strTextTag);
	void						Add_TextList(CUI::UI_DESC tUIDesc);
	void						Change_Text(string strTextKey, string strFontTag, string strText, _float fPosX, _float fPosY, _vector vColor, _float fScale, _float2 vOrigin, _float fRotation);
	_int						m_iSelected_TextClassIndex = 0; // 선택된 Text Class
	_int						m_iSelected_TextObjectIndex = 0; // 선택된 Text Object
	_int						m_iSelected_TextListIndex = 0; // 선택된 Text List

	char						m_cTextKey[MAX_PATH] = "";
	char						m_cTextFont[MAX_PATH] = "";
	char						m_cText[MAX_PATH] = "";
	char						m_cChangeTextTag[MAX_PATH] = "";
	_bool						m_bText = false;
	_bool						m_bFreeMove = false;
	_float						m_fPosX = 0.f;
	_float						m_fPosY = 0.f;
	_float						m_fScale = 5.f;
	_float						m_fRotation = 0.f;
	_float2						m_vOrigin = { 0.1f, 0.1f };
	_vector						m_vColor = { 1.f, 1.f, 1.f, 1.f };
	CUI::UI_DESC				m_tText_Desc;
	CUI*						m_pCurrText = nullptr;
	struct CUI_Text::TEXTINFO	m_tTextInfo;
	vector<CUI*>				m_vecTextObject;
	vector<string>				m_vecTextList =
	{
		"Defaul"
	};
#pragma endregion											Text End

#pragma region												Animation
	// 키프레임 목록
	void						KeyframeList();
	// 키프레임 수정 창
	void						KeyframeChangeWindow();
	// 키프레임 랜더링 및 편집 (순서 UI중 가장 마지막)
	void						KeyframeRender_ValueChange();
	// 키프레임 자동 생성 함수(선형 보간)
	void CreateKeyframesWithLinearInterpolation(
												_float minTime, _float maxTime,
												_float minValue, _float maxValue,
												_float2 minScaleValue, _float2 maxScaleValue,
												_float minRotationValue, _float maxRotationValue,
												_float2 minTranslationValue, _float2 maxTranslationValue,
												_float _minTexture, _float _maxTexture,
												_int numKeyframes);
	// 키프레임 자동생성 세팅 함수
	void	KeyframeAutomaticGeneration();
#pragma region												PlayAnim
	void	PlayAnimation(_float fTimeDelta);
	_bool	m_isPlayAnim = false;
	_float  m_fPlayTime = 0.f;
#pragma region												TimeLineBar
	void	AnimationTimeLineBar(_float fTimeDelta);// 애니메이션 타임라임 바
	_float	EvaluateAnimationAtTime(float time);	// 애니메이션에 따른 현재 시간 계산
	void	KeyframeValueChange(_float fTimeDelta); // 애니메이션 키프레임 조절
	void	ImGuiKeyInput();
	void	CurKeyframe_ValueChange();
	// 선택한 키프레임 값을 변경하는 함수
	void	DrawSelectedKeyframeEditor(CUI::UIKEYFRAME& selectedKeyframe);
	void	SelectKeyframeMouse();
#pragma region Rect_Option
	// UV 변경
	void	Setting_Distortion(_float fTimeDelta);
	_int	m_vUV_MaxTileCount[2] = { 7, 7 };
	_float	m_fSequenceTerm_RectSprite = { 0.05f };

	/* Distortion */
	_float m_fSequenceTerm_Distortion = { 1.f };

	_float	m_vScrollSpeeds[3] = { 1.f, 1.f, 0.f };
	_float	m_vScales_Distortion[3] = { 1.f, 1.f, 1.f };

	_float	m_vDistortion1[2] = { 0.1f, 0.1f };
	_float	m_vDistortion2[2] = { 0.0f, 0.0f };
	_float	m_vDistortion3[2] = { 0.0f, 0.1f };

	_float	m_fDistortionScale = { 1.f };
	_float	m_fDistortionBias = { 1.f };

#pragma endregion

	// 변경할 속성 값 모드
	enum EDITMODE
	{
		EDITMODE_NONE,
		EDITMODE_SCALE,
		EDITMODE_ROTATION,
		EDITMODE_TRANSLATION,
		EDITMODE_TIME_VALUE,
		EDITMODE_TEXTURE
	};
	EDITMODE eEditMode = EDITMODE_NONE; // 변경할 모드 변수
	float vValueSize = 1.f;	// 변경을 줄 값의 크기
	_float originalIndex;
	// 키프레임 드래그
	_bool	isDraggingKeyframe = false;
	_int	draggingKeyframeIndex = 0;
	_float  initialKeyframeX = 0.f;					// 마우스 드래그 시작 위치
	_float	m_fFrame[100];							// 시간
	_float	m_fPlaybackSpeed = 1.0f;				// 재생 속도
#pragma endregion
	// _vec3 선형 보간 함수
	_float2						Lerp_float2(const _float2& a, const _float2& b, float t);

	// _vec2 선형 보간 함수
	_float						Lerp_float(const _float& a, const _float& b, float t);

	// 쿼드라틱 이징(InQuad) 함수 (시작)
	_float						ImEaseInQuad(float start, float end, float t);

	// 쿼드라틱 이징(OutQuad) 함수 (끝)
	_float						ImEaseOutQuad(float start, float end, float t);

private:
	// 애니메이션 타임 라인
	std::vector<CUI::UIKEYFRAME>*	m_vecTimeline = nullptr;
	_float currentTime = 0.0f; // 현재 시간 값
	_float MaxTime = 20.f;
	_float fDisplayTime = 1.f;	// 눈금마다의 시간 값 표시
	_bool isDraggingTimeline = false;
	_bool m_bRendomValue = false; // 벨류 값 랜덤으로 주기
	_int  closestKeyframeIndex = -1; // # 현재 마우스로 선택한 키프레임 인덱스
	// 애니메이션의 타입
	ImVec2					timelinePos = { 0.f, 0.f };
	ImVec2					timelineSize = { 800.f, 85.f }; // 애니메이션 타임 라인 크기

	_int m_iOldIndex = -1;
#pragma region											최소, 최대 값
// 크기
	_float	fMin_Scale = -2000.0001f;	// 최소
	_float	fMax_Scale = 2000.f;	// 최대

	// 회전
	_float	fMin_Rot = -360.0f;		// 최소
	_float	fMax_Rot = 360.f;	// 최대

	// 이동
	_float	fMin_Pos = -5000.f;		// 최소
	_float	fMax_Pos = 5000.f;	// 최대

	// 시간
	_float	fMin_Time = 0.f;	// 최소
	_float	fMax_Time = MaxTime;// 최대

	// 벨류
	_float	fMin_Value = 0.f;	// 최소
	_float	fMax_Value = 1.f;	// 최대
#pragma endregion

#pragma region											최소, 최대 자동 생성 값
	_bool  m_bIndividualTexture = false;

	_float _v2Time[2] = { 0.f, 1.f };
	_float _v2Value[2] = { 0.f, 0.7f };
	_float minScale[2] = { 50.f, 50.f };
	_float maxScale[2] = { 50.f, 50.f };
	_float minRot = { -360.f };
	_float maxRot = { 360.f };
	_float minPos[2] = { 0.f, 0.f };
	_float maxPos[2] = { 0.f, 0.f };

	// 텍스처 프레임
	_float	fMin_Texture = 0.f;	// 최소
	_float	fMax_Texture = 0.f;	// 최대

	_float numKeyframes = 1.f;
#pragma endregion
	// Test
	_bool	m_bCreateOK = false;
	_bool	m_bRepetition = false;
#pragma endregion

public: /* Save/Load */
	virtual	HRESULT				Save_Function(string strPath, string strFileName) override;
	virtual HRESULT				Load_Function(string strPath, string strFileName) override;

public: /* ================= Function ================= */
	// string타입을 받는 벡터 컨테이너를 char*타입을 받는 벡터 컨테이너로 변환 해주는 함수
	std::vector<const char*> ConvertStringVectorToCharArray(const std::vector<std::string>& stringVector);
	// string을 wstring으로 변환 해주는 함수
	std::wstring ConvertToWideString(const std::string& ansiString);
	// string을 wchar로 변환 해주는 함수
	WCHAR*		StringTowchar(const std::string& str);
	// wstring을 string으로 변환 해주는 함수
	std::string WStringToString(const std::wstring& wstr);
	//	wstring을 char로 변환 해주는 함수
	char*		ConverWStringtoC(const wstring& wstr);
	//	char를 wchar_t로 변환 해주는 함수
	wchar_t*	ConverCtoWC(char* str);
	// WCHAR*를 string으로 변환 해주는 함수
	std::string WideStringToString(const wchar_t* wideStr);
	// 경로에서 파일이름과 확장자만 추출해주는 함수
	std::string GetFileName(const std::string& filePath);
	// _기준으로 가장 마지막 이름을 추출해주는 함수
	std::string GetUnderbarName(const std::string& filePath);
	// 확장자를 제거해주는 함수
	std::string RemoveExtension(const std::string& filePath);



public: /* Image */
	void						LoadImgPath(const _tchar* folderPath);
	void						ImagePreview(_float fTimeDelta);

private: /* ==================== Mouse ==================== */
	POINT						m_pt;

private: /* Image_Member */
	_int						m_My_Image_Width = 100;		// 미리보기 이미지 가로사이즈
	_int						m_My_Image_Height = 100;	// 미리보기 이미지 세로사이즈
	_int						m_iTextureNum = 0;			// 텍스처 개수
	vector<IMAGEINFO*>			m_vecTexture;				// 이미지 미리보기를 위한 텍스처 정보들

	vector<PATHINFO*>			m_vecInitialPaths;			// 1. 폴더를 순회하여 경로, 파일이름을 처음으로 담아오는 녀석 (프로토타입 파싱가능)
	std::vector<PATHINFO*>		m_vecImagePaths;			// 2. 담아온 녀석에게 받은 경로와 이름으로 이미지 로드를 위한 녀석
	_int						m_iSelectedPathIndex = 0;	// 선택된 이미지 경로 인덱스

private: /* 2D */
	string						m_strLayer[3] = { "Layer_UI_Player", "Layer_UI_Monster", "Layer_UI_Inventory" };
	_int						m_iCurrLayerNum = 0;


private: /* enum */
	UITYPE						m_eUIType;
	TOOLSTATE					m_eToolType = TOOL_UI;

private: /* Value */
	_float						m_fChangeValue = 0.1f;
	_int						m_iChangeType = (_int)CHANGETYPE::NONE;
	_int						m_iRenderGroup = (_int)CRenderer::RENDER_UI;

private: /* bool */
	_bool						m_bOpenTexture = true;
	_bool						m_bOpenSetting = false;
	_bool						m_bOpenUI = false;
	_bool						m_bScaleChange = false;
	_bool						m_bRotChange = false;
	_bool						m_bPosChange = false;
	_bool						m_bParent = false;
	_bool						m_bGroupObject = false;

private:
	_bool						m_bCheckImguiRect_Child = false;
	_bool						m_bCheckImguiRect_Parent = false;
	_bool						m_bCheckImguiRect_UI_Animation = false;
	_bool						m_bCheckImguiRect_ValueChange = false;
	_bool						m_bCheckImguiRect_UI_TimeLine = false;
	_bool						m_bCheckImguiRect_Info = false;
	_bool						m_bCheckImguiRect_UI_Info = false;
	_bool						Check_ImGui_Rect();

private:
	//// ==============폴더 경로==============
	//// 이미지 경로 목록을 저장하는 벡터
	//std::vector<std::string> m_vecImgPs =
	//{
	//	"../Bin/Resources/Textures/UI/Textures/Blood",
	//	"../Bin/Resources/Textures/UI/Textures/Buttons",
	//	"../Bin/Resources/Textures/UI/Textures/Crosshairs",
	//	"../Bin/Resources/Textures/UI/Textures/DeathScreen",
	//	"../Bin/Resources/Textures/UI/Textures/EnemyHUD",
	//	"../Bin/Resources/Textures/UI/Textures/EnemyIndicators",
	//	"../Bin/Resources/Textures/UI/Textures/Icons",
	//	"../Bin/Resources/Textures/UI/Textures/Interaction",
	//	"../Bin/Resources/Textures/UI/Textures/InText",
	//	"../Bin/Resources/Textures/UI/Textures/Inventory",
	//	"../Bin/Resources/Textures/UI/Textures/Journal",
	//};

	// ============== 클래스 목록 ==============
	// 	// 클래스 목록을 저장하는 벡터
	std::vector<std::string> m_vecParent =
	{
		"Player_Left_Interface",
		"Player_Right_Interface",
		"QuestBox"
	};

	// 클래스 목록을 저장하는 벡터
	std::vector<std::string> m_vecClass =
	{
		"Anything",
		"BackGround",
		"Loading_Icon",
		"Player_HPBar",
		"Player_HPFrame",
		"Player_Skill_Frame",
		"Player_Skill_Guige",
		"Player_Skill_Icon",
		"Player_ExpBar",
		"Blood_Lights",
		"Blood_Veins",
		"Blood_Vignette",
		"TutorialBox",
		"QuestIcon",
		"ItemShowcase",
		"ItemIcon",
		"LevelUp_MagicFrame",
		"LevelUp_MagicTrack",
		"LevelUp_Shards",
		"LevelUp_MagicSide",
		"LevelUp_SunBeacon",
		"LevelUp_TextBox",
		"LevelUp_ShieldFrame",
		"MouseCursor",
		"Option_Window",
		"AimCrosshair",
		"Weakness",
		"Distortion",
		"EnemyHP_FrameBar",
		"EnemyHP_Bar"
	};

	// 클래스 목록을 저장하는 벡터
	std::vector<std::string> m_vecTextBoxClass =
	{
		"TutorialBox",
		"QuestBox"
	};

private:
	void	UI_Preset();
	class CUI_Manager*	m_pUI_Manager = nullptr;
	_bool				m_bDisappear = false;

private:
	ImGuiTabBarFlags m_Tab_bar_flags = ImGuiTabBarFlags_None;

public:
	static CWindow_UITool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END

