#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Renderer.h"
//#include "UI_Manager.h"

BEGIN(Engine)
class CCharacter;
END

BEGIN(Client)

class CUI abstract : public CGameObject
{
public:
	enum DISTORTIONKIND { MASK, NOISE, DISTORTION_END };
	enum UI_KIND { NORMAL, TEXT, KIND_END };
	enum UI_LEVEL { LEVEL0, LEVEL1, LEVEL2, LEVEL3, LEVEL4, LEVEL5, LEVEL_END };

public:
	void		Set_UILevel(_uint eLevel) { m_eUI_Level = (UI_LEVEL)eLevel; }
	void		UILevelUP() 
	{ 
		if (m_eUI_Level < (UI_LEVEL)(LEVEL_END - LEVEL1) && m_eUI_Level >= LEVEL0)
			m_eUI_Level = (UI_LEVEL)(m_eUI_Level + (UI_LEVEL)LEVEL1);
	}
	UI_LEVEL	Get_UILevel() { return m_eUI_Level; }

protected:
	UI_LEVEL	m_eUI_Level = LEVEL0;
	UI_LEVEL	m_eUI_PreLevel = LEVEL0;

public:
	// 키프레임 구조체
	typedef struct tagUIKeyframe
	{
		_float	fTime = 0.f;						// 키프레임의 시간 (0.0f ~ MaxTime 범위)
		_float	fValue = 0.f;						// 애니메이션 값 (크기, 회전, 이동 등)
		_float	fAnimSpeed = 1.f;					// 애니메이션 재생 속도

		_int	iType = 0;							// 애니메이션 타입 (0: 크기, 1: 회전, 2: 이동)

		_bool	isEaseIn = false;					// Ease In 설정 (True 또는 False)
		_bool	isEaseOut = false;					// Ease Out 설정 (True 또는 False)

		_int	iTexureframe = 0;					// 텍스처 변경 값

		_float2	vScale = { 50.f, 50.f };			// 크기를 담을 그릇
		_float	fRot = 0.f;							// 회전을 담을 그릇
		_float2	vPos = { 10.f, 10.f };				// 위치를 담을 그릇
		
		_float3	vWorld_Scale = { 0.02f, 0.02f, 0.1f };		// 회전을 담을 그릇
		_float3	vWorld_Rot = { 0.f, 0.f, 0.f };		// 회전을 담을 그릇
		_float3	vWorld_Pos = { 0.f, 0.f, 0.1f };	// 위치를 담을 그릇

		_float2	vKeyFramePos = { 0.00000000f, 0.00000000f };	// 툴에서의 해당 키프레임 위치

		_float	fAlpha = 1.f;

		/* 동작 추가 */
		_bool	bActive = false;
		_bool	bAppear = true;
		_bool	bDisappear = false; // Disappear
		_bool	bLoopSection = false; // Disappear
		_bool	bTrigger = false;

		/* Distortion */
		_bool		bDistortionUI = false;
		_bool		bRestore = false;
		_float		fTimeAcc = 0.f;
		_float		fSequenceTerm = 0.f;
		_float3		vScrollSpeeds = { 0.f, 0.f, 0.f };
		_float3		vScales = { 0.f, 0.f, 0.f };
		_float2		vDistortion1 = { 0.f, 0.f };
		_float2		vDistortion2 = { 0.f, 0.f };
		_float2		vDistortion3 = { 0.f, 0.f };
		_float		fDistortionScale = 0.f;
		_float		fDistortionBias = 0.f;

		_bool		bStopPlay = false;		// 정지
		_bool		bReversePlay = false;	// 역재생
		_bool		bMaskChange = false;	// 마스크 텍스처 변경
		_bool		bNoiseChange = false;	// 노이즈 텍스처 변경
		_int		iMaskNum = 0;			// 마스크 텍스처 번호
		_int		iNoiseNum = 0;			// 노이즈 텍스처 번호

	}UIKEYFRAME;


	// 애니메이션 생성시 최소 및 최대 값을 입력 받는 구조체 (키프레임)
	typedef struct tagUI_AnimKeyframe 
	{
		_float		fminTime = 0.0f;
		_float		fmaxTime = 20.f;
		_float		fminValue = 0.0f;
		_float		fmaxValue = 1.0f;
		_float3		vminScale = { 0.01f, 0.01f, 0.f };
		_float3		vmaxScale = { 5.f, 5.f, 5.f };
		_float3		vminRotation = { 0.f, 0.f, 0.f };
		_float3		vmaxRotation = { 360.f, 360.f, 360.f };
		_float3		vminPosition = { 0.f, 0.f, 0.f };
		_float3		vmaxPosition = { 300.f, 300.f, 300.f };

		_int		iKeyframeNum = 0;

	}UI_ANIMKEYFRAME;

	typedef struct tagUIDesc /* 저장/불러오기 할때 순서 잘 맞추기 */
	{
		_bool		bParent = false;
		_bool		bWorld = false;
		_bool		bGroup = false;

		_float		fOrigin_ScaleX = 100.f;
		_float		fOrigin_ScaleY = 100.f;

		_float		fOrigin_fRotationZ = 0.f;

		_float		fOrigin_fPositionX = 0.f;
		_float		fOrigin_fPositionY = 0.f;


		/* 크기 */
		_float		fScaleX = 100.f;
		_float		fScaleY = 100.f;

		/* 회전 */
		_float		fRotationZ = 0.5f;

		/* 이동 */
		_float		fPositionX = (_float)g_iWinSizeX / 2;
		_float		fPositionY = (_float)g_iWinSizeY / 2;
		_float		fPositionZ = 0.5f;

		/* 
			(저장 순서)
			=부모 여부=
			bParent
			=월드 여부=
			bWorld
			=크기 조절=
			fScaleX = 1
			fScaleY = 1
			=회전 조절=
			fRotationX
			fRotationY
			=위치 조절=
			fPositionX
			fPositionY
			fPositionZ
			=투명도 조절=
			fAlpha
			=오브젝트 넘버=
			iObjectNum
			=셰이더 패스 선택=
			iShaderNum
			=테그 값=
			strObjectName
			strLayerTag
			strCloneTag
			strProtoTag
			strFilePath
			strMapTextureTag
			=색상=
			vColor
		*/

		/* 알파 */
		_float		fAlpha = 0.f;
		_float		fAlphaTrue = 0.f;

		_int		iLevel = 0;
		_int		iMaxLevel = 0;
		_int		iObjectNum = 0;			// 몇번째 녀석인지
		_int		iShaderNum = 0;			// 적용할 셰이더 넘버

		_int		iUINum = 0;			// 무슨 UI인지
		string		strUIName = "";			// 무슨 UI인지

		string		strObjectName = "";
		string		strLayerTag = "";
		string		strCloneTag = "";
		string		strProtoTag = "";
		string		strFilePath = "";
		string		strMapTextureTag = "";	// 적용할 맵 텍스처
		string		strButton_Name = "";	// 적용할 맵 텍스처

		/* 색상 */
		_vector		vColor = { 0.f, 0.f, 0.f, 0.f };

		class CTransform* pParentTransformCom = nullptr;

		_int		iKeyframeNum = 0;
		UIKEYFRAME	tKeyframe;

		/* Distortion */
		_float		fTimeAcc = 1.f;
		_float		fSequenceTerm = 0.f;
		_float3		vScrollSpeeds = { 0.f, 0.f, 0.f };
		_float3		vScales = { 0.f, 0.f, 0.f };
		_float2		vDistortion1 = { 0.f, 0.f };
		_float2		vDistortion2 = { 0.f, 0.f };
		_float2		vDistortion3 = { 0.f, 0.f };
		_float		fDistortionScale = 0.f;
		_float		fDistortionBias = 0.f;

		_bool		bDistortionUI = false;
		_int		iMaskNum = 0;
		_int		iNoiseNum = 0;

		MODE_COLOR eColorMode = MODE_COLOR::MODE_COLOR_END;
		//CRenderer::RENDERGROUP eRenderGroup = CRenderer::RENDER_UI;
		_int		iRenderGroup = (_int)CRenderer::RENDER_UI;

	}UI_DESC;

	enum UI_BUTTON_STATE
	{
		BUTTON_NORMAL, BUTTON_PICKING, BUTTON_PICKED, BUTTON_END
	};

protected:
	CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CUI(const CUI& rhs);
	virtual ~CUI() = default;

public: /* ============================== Get / Set =============================== */
	// =>UIDesc
	UI_DESC			Get_UIDesc() { return m_tUIInfo; }
	void			Set_UIDesc(UI_DESC UIDesc) { m_tUIInfo = UIDesc; }
	// =>Distortion
	_bool			Get_DistortionUI() { return m_tUIInfo.bDistortionUI; }
	void			Set_ScrollSpeeds(_float3 vScrollSpeeds) { m_tUIInfo.vScrollSpeeds = vScrollSpeeds; }
	void			Set_SequenceTerm(_float fSequenceTerm) { m_tUIInfo.fSequenceTerm = fSequenceTerm; }
	void			Set_DistortionScales(_float3 vScales) { m_tUIInfo.vScales = vScales; }
	void			Set_Distortion1(_float2 vDistortion1) { m_tUIInfo.vDistortion1 = vDistortion1; }
	void			Set_Distortion2(_float2 vDistortion2) { m_tUIInfo.vDistortion2 = vDistortion2; }
	void			Set_Distortion3(_float2 vDistortion3) { m_tUIInfo.vDistortion3 = vDistortion3; }
	void			Set_DistortionScale(_float fDistortionScale) { m_tUIInfo.fDistortionScale = fDistortionScale; }
	void			Set_DistortionBias(_float fDistortionBias) { m_tUIInfo.fDistortionBias = fDistortionBias; }
	// =>Pickup
	_bool			Get_Pick() { return m_bPick; }
	_bool			Set_Pick(_bool Pick) { m_bPick = Pick; }
	// =>Part
	CUI*			Get_UIPart(const wstring& strPartTag);
	// =>Parts
	vector<CUI*>	Get_UIParts();
	// =>Button
	_uint			Get_ButtonState() { return m_iButtonState; }
	_uint			Set_ButtonState(_uint iButtonState) { m_iButtonState = (UI_BUTTON_STATE)iButtonState; }
	// =>Position
	void			Set_Pos(_float fPosX, _float fPosY);
	void			Set_PosZ(_float fZ);
	// =>Kind
	void			Set_Kind(UI_KIND eUI_King) { m_eKind = eUI_King; }
	UI_KIND			Get_Kind() { return m_eKind; }

	// =>Active
	void			Set_Active(_bool bActive) { m_bActive = bActive; }
	_bool			Get_Active() { return m_bActive; }

	_int			Get_AnimCurrentIndex() { return iFrameIndex; }
	_bool			Get_AnimTrigger() { return m_bTrigger; }
	void			Set_AnimTrigger(_bool bTrigger) { m_bTrigger = bTrigger; }

	/* Debug */
	void			Set_Tool(_bool bTool) { m_bTool = bTool; }
	_bool			m_bTool = false;

	/* RenderGroup */
	void			Set_RenderGroup(CRenderer::RENDERGROUP eGroup) { m_tUIInfo.iRenderGroup = eGroup; }
	//CRenderer::RENDERGROUP*		Get_RenderGroup() {	return &m_tUIInfo.eRenderGroup; }
	_int*			Get_RenderGroup() {	return &m_tUIInfo.iRenderGroup; }
	/* State */
	void			Set_UIState(UISTATE eState) { m_eState = eState; 
	int i = 10;
	}
	UISTATE			Get_UIState() { return m_eState; }

//protected:
public:
	virtual HRESULT	Set_ParentTransform(CTransform* pParentTransformCom);
	void			Check_Change_WorldUI(_float fTimeDelta);
	void			Check_MouseInput(_float fTimeDelta);

protected:
	//_bool			m_bMouseDown_LB = false;
	//_bool			m_bMouseDown_RB = false;
	//_bool			m_bMouseDown_MB = false;
	//
	//_bool			m_bMousePressing_LB = false;
	//_bool			m_bMousePressing_RB = false;
	//_bool			m_bMousePressing_MB = false;
	//
	//_bool			m_bMouseUp_LB = false;
	//_bool			m_bMouseUp_RB = false;
	//_bool			m_bMouseUp_MB = false;

public: /* ============================== Add ============================== */
	void			Add_Create_Parts(void* pArg);
	void			Add_Parts(CUI* pArg);

public: /* ========================== Change_Size ========================== */
	void			Set_Size(_float fSizeX, _float fSizeY);
	void			Change_SizeX(_float MMX);
	void			Change_SizeY(_float MMY);
	void			Change_SizeRight(_float MMX);
	void			Change_SizeBottom(_float MMY);
	void			Change_SizeTop(_float MMY);
	void			Change_SizeLeft(_float MMX);


public: /* ============================== Basic =============================== */
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void	Priority_Tick(_float fTimeDelta);
	virtual void	Tick(_float fTimeDelta);

	/* State */
	virtual void	UI_Ready(_float fTimeDelta);
	virtual void	UI_Enter(_float fTimeDelta);
	virtual void	UI_Loop(_float fTimeDelta);
	virtual void	UI_Exit(_float fTimeDelta);

	virtual void	UI_AppearTick(_float fTimeDelta);
	virtual void	UI_DisappearTick(_float fTimeDelta);
	virtual void	Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

public: /* ============================= Function ============================= */
	virtual void	Picking_UI();	// Pick
	virtual void	Check_RectPos();	// Moving
	void			Moving_Picking_Point(POINT pt); // Picking Moving
	virtual void	Parts_Delete();
	_float			Check_CamToTarget_Distance(_vector vTargetPos);

public: /* ============================= LifeTime ============================= */
	void			LifeOff(_float fTimeDelta);
	void			LifeOn(_float fTimeDelta);

public:
	void			Set_LifeTimeUI(_bool bLifeTimeUI) { m_bLifeTimeUI = bLifeTimeUI; }
	void			Set_LifeTime(_float fLifeTime) { m_fLifeTime = fLifeTime; }
	void			ResetTime();
	_bool			Get_LifeTimeUI() { return m_bLifeTimeUI; }

protected:
	_bool			m_bLifeTimeUI = false;
	_float			m_fActive_Distance = 35.f;

public: /* ============================== SetUp ============================== */
	HRESULT			SetUp_UIRect(_float fPosX, _float fPosY, _float fSizeX = 1.f, _float fSizeY = 1.f);
	HRESULT			SetUp_Transform(_float fPosX, _float fPosY, _float fScaleX, _float fScaleY);
	HRESULT			Ready_UI(const char* cFilePath);
	HRESULT			Create_UIParts(UI_DESC tUI_Desc);
	HRESULT			Update_Child_Transform();

	//				TargetPosition => Screen
	void			SetUp_PositionToScreen(_fvector vWorldPos);

	//				TargetWorld => Screen
	void			SetUp_WorldToScreen(_matrix matWorldPos, _float3 vOffsetPos = { 0.f, 0.f, 0.f });
	HRESULT			SetUp_BillBoarding();
	_matrix			matTargetWorld = XMMatrixIdentity();
	_bool			Calculation_Direcion(_vector vTargetPos, _float4 vCurrentDir);
	_float			Target_Contained_Angle(_vector vStandard, _float4 vTargetPos);

	// error : 새 코드 요소를 반환하지 못했습니다. 구문 오류일 수 있습니다. -> 해결방법 : 프로젝트 폴더 내에 vs폴더 삭제 후 실행
	void			Tick_LevelUp(_float fTimeDelta);

public:	// Tick
	void			Player_HUD(_float fTimeDelta);
	void			TutorialBox(_float fTimeDelta);

public:
	void			Check_Disappear(_float fTimeDelta);

public:
#ifdef _DEBUG
	/* (컨테이너의 주소를 받아오는건 릴리즈 모드에서 터지는 버그가있음. 툴용) */
	vector<CUI*>*	Get_vecUIParts() { return &m_vecUIParts; }
	vector<UIKEYFRAME>*	Get_vecAnimation() { return &m_vecAnimation; }
#endif // DEBUG
	string			Get_FilePathTag() { return m_tUIInfo.strFilePath; }
	string			Get_ObjectNameTag() { return m_tUIInfo.strObjectName; }
	_int			Get_ObjectNum() { return m_tUIInfo.iObjectNum; }
	void			Set_UIName(string strName) { m_tUIInfo.strUIName = strName; }
	void			Set_ProtoTag(string strProto) { m_tUIInfo.strProtoTag = strProto; }

protected: /* =========================== Ready ============================= */
	virtual HRESULT Ready_Components();
	virtual HRESULT Bind_ShaderResources();

public:
	virtual HRESULT	Setting_Owner();

public: /* =========================== Save/Load ============================== */
	virtual void	Load_FromJson(const json& In_Json);
	virtual json	Save_Desc(json& out_json);
	virtual json	Save_Animation(json& out_json);
	virtual void	Change_Animation(const string& strAnimPath);

public: /* =========================== Animation ============================== */
	void			Play_Animation(_float fTimeDelta);
	void			Add_Keyframe(UIKEYFRAME tKeyframe) { m_vecAnimation.push_back(tKeyframe); }
	void			Clear_Keyframe() { m_vecAnimation.clear(); }
	void			Emplaceback_Keyframe(UIKEYFRAME tKeyframe) 
	{ 
		if(!m_vecAnimation.empty())
			m_vecAnimation.emplace_back(tKeyframe);
	}
	void			Set_AnimationKeyframe(UIKEYFRAME tKeyframe);
	// 애니메이션 값
	std::vector<UIKEYFRAME> m_vecAnimation = {};
	std::vector<UIKEYFRAME> m_vecChangAnimation = {};

	_int			m_iTextureNum = 0;
	_int			m_iLoopAnimIndex = 0;

	void			Set_AnimPlay(_bool bPlay) { m_bPlayAnim = bPlay; }
	_bool			Get_AnimPlay() { return m_bPlayAnim; }
	_bool			m_bPlayAnim = false;

	void			Set_Disappear(_bool bDisappear) { m_bDisappear = bDisappear; }
	_bool			Get_Disappear() { return m_bDisappear; }
	_bool			m_bDisappear = false;

	void			Set_CurrTime(_float fCurrTime) { m_fCurrTime = fCurrTime; }
	_float			Get_CurrTime() { return m_fCurrTime; }
	_float			m_fCurrTime = 0.f;

	void			Set_Repetition(_bool bRepetition) { m_bRepetition = bRepetition; }
	_bool			Get_Repetition() { return m_bRepetition; }
	_bool			m_bRepetition = false;

	void			Set_Alpha(_float fAlpha) { m_fAlpha = fAlpha; }
	_float			Get_Alpha() { return m_fAlpha; }
	_float*			Get_PointAlpha() { return &m_tUIInfo.fAlpha; }

	void			Set_SkillPoint(_int iSkillPoint) { m_iSkillPoint = iSkillPoint; }
	_int			Get_SkillPoint() { return m_iSkillPoint; }
	void			Set_Price(_int iPrice) { m_iPrice = iPrice; }
	_int			Get_Price() { return m_iPrice; }

protected:
	_int			m_iPrice = 0;
	_int			m_iSkillPoint = 0;

private:
	// dt 값
	_float			fFrameTimeDelta, fCurFrameTimeDelta;

	// 크기
	_float			fSizeX_Delta, fSizeY_Delta, fSizeZ_Delta;

	// 회전
	_float			fRotX_Delta, fRotY_Delta, fRotZ_Delta;

	// 이동
	_float			fPosX_Delta, fPosY_Delta, fPosZ_Delta;

	// 알파
	_float			fAlpha_Delta;

	// 시간
	_float			fTimeAcc_Delta = 0.f;

	// 시퀀스
	_float			fSequenceTerm_Delta = 0.f;

	// 스크롤 스피드
	_float3			vScrollSpeeds_Delta = { 0.f, 0.f, 0.f };

	// 크기
	_float3			vScales_Delta = { 0.f, 0.f, 0.f };

	// 디스토션1
	_float2			vDistortion1_Delta = { 0.f, 0.f };

	// 디스토션2
	_float2			vDistortion2_Delta = { 0.f, 0.f };

	// 디스토션3
	_float2			vDistortion3_Delta = { 0.f, 0.f };

	// 디스토션 Scale
	_float			fDistortionScale_Delta = 0.f;

	// 디스토션 Bias
	_float			fDistortionBias_Delta = 0.f;

protected:
	_bool			m_bAnimChange = false;

public:
	void	Set_TimeAcc(_float fTimeAcc) { m_tUIInfo.fTimeAcc = fTimeAcc; }

protected: /* Data */
	class CData_Manager* m_pData_Manager = { nullptr };

protected:
	class CUI_Manager* m_pUIManager = { nullptr };
	//	class CCharacter*  m_pCharacter = { nullptr };
	//
	//public:
	//	void	Set_Character(CCharacter* pCharacter) { m_pCharacter = pCharacter; }

protected:
	_bool				Alpha_Minus(_float fTimeDelta);
	_bool				Alpha_Minus_Control(_float fTimeDelta, _float fAlpha);
	_bool				Alpha_Plus(_float fTimeDelta);
	_bool				Alpha_Plus_Control(_float fTimeDelta, _float fAlpha);
	_float				m_fAlphaSpeed = 1.f;

protected:
	void				Compute_CamDistance();
	_float				m_fCamDistance = { 0.0f };

protected: /* LifeTime */
	void				LifeTime_LevelUp(_float fTimeDelta);
	_bool				m_bEventOn = false;
	_float				m_fLifeTime = 5000.f;
	_float				m_fActiveTime = 1000.f;
	_float				m_fTime = (_float)GetTickCount64();
	_float				m_fLeftSize = 0.f;
	_float				m_fBottomSize = 0.f;
	_float				m_fChangeValue = 0.1f;
	_float				m_fOriginPoint = 0.0f;
	_bool				m_bStart = false;

public:
	void				Set_LeftSize(_float fLeftSize) { m_fLeftSize = fLeftSize; }
	void				Set_ChangeValue(_float fChangeValue) { m_fChangeValue = fChangeValue; }

	_float*				Get_LeftSize() { return &m_fLeftSize; }
	_float*				Get_BottomSize() { return &m_fBottomSize; }
	_float*				Get_ChangeValue() { return &m_fChangeValue; }
	_float*				Get_OriginPoint() { return &m_fOriginPoint; }

protected: /* ========================= Component =========================== */
	CShader*			m_pShaderCom = { nullptr };
	//CTexture*			m_pTextureCom;
	CTexture*			m_pMapTextureCom = { nullptr };	// 적용할 맵 텍스처
	_int				m_iShaderNum;		// 적용할 셰이더 넘버
	CVIBuffer_Rect*		m_pVIBufferCom = { nullptr };
	_vector				m_vAxis = { 0.f, 0.f, 0.f, 0.f };

protected: /* =========================== Space ============================ */
	_float4x4			m_ViewMatrix, m_ProjMatrix;

protected: /* =========================== Screen ============================ */
	_float				m_fWorldToScreenX = 0.f;
	_float				m_fWorldToScreenY = 0.f;
	_float				m_fScreenOffsetX = 300.f;
	_float				m_fScreenOffsetY = 150.f;

	_float				m_fPreScreenX = 0.f;
	_float				m_fPreScreenY = 0.f;

protected: /* ============================= UI =============================== */
	vector<CUI*>		m_vecUIParts;
	UI_DESC				m_tUIInfo;
	RECT				m_rcUI = {};
	UISTATE				m_eState = UISTATE::STATE_END;
	UITYPE				m_eType = UITYPE::NONE;
	_float4x4			m_Origin_WorldMatrix = {};
	_bool				m_bActive = false;
	_bool				m_bReset = false;
	_bool				m_bRestore = false;
	_bool				m_bRenderOut = false;
	// UI_Member
	_float				m_fPositionX = 0.f, m_fPositionY = 0.f;
	_float				m_fScaleX = 0.f, m_fScaleY = 0.f, m_fScaleZ = 0.1f;
	UI_KIND				m_eKind = NORMAL;
	_float				m_fOffsetX = 0.f, m_fOffsetY = 0.f;
	_float				m_fTarget_Distance = 0.f;

	CTexture*			m_pDistortionCom[DISTORTION_END] = { nullptr };
	_int				m_iMaskNum = 0;
	_int				m_iNoiseNum = 0;

	_bool				m_bReversePlay = false;
	_bool				m_bStopPlay = false;
	_bool				m_bButtonUI = false;
	UI_BUTTON			m_eUI_ButtonState = UI_BUTTON::NONE;

	/* Anim Index */
	_uint iFrameIndex = 0;

public:
	void				Set_MaskNum(_int iMaskNum) { m_tUIInfo.iMaskNum = iMaskNum; }
	void				Set_NoiseNum(_int iNoiseNum) { m_tUIInfo.iNoiseNum = iNoiseNum; }
	void				Set_WorldUI(_bool bWorld) { m_tUIInfo.bWorld = bWorld; }
	void				Set_DiffuseColor(_float fColorR, _float fColorG, _float fColorB, _float fColorA)
	{
		m_tUIInfo.vColor.m128_f32[0] = fColorR;
		m_tUIInfo.vColor.m128_f32[1] = fColorG;
		m_tUIInfo.vColor.m128_f32[2] = fColorB;
		m_tUIInfo.vColor.m128_f32[3] = fColorA;
	}
	void				Set_ColorMode(MODE_COLOR eColorMode) { m_tUIInfo.eColorMode = eColorMode; }
	_bool*				Get_WorldUI() { return &m_tUIInfo.bWorld; }

public:
	void	Set_OffsetX(_float fOffsetX) { m_fOffsetX = fOffsetX; }
	void	Set_OffsetY(_float fOffsetY) { m_fOffsetY = fOffsetY; }
	void	Set_OffsetXY(_float fOffsetX, _float fOffsetY) 
	{ 
		m_fOffsetX = fOffsetX;
		m_fOffsetY = fOffsetY;
	}

	// 투명도
	_float				m_fAlpha = 0.f;
	_float2				m_vScale = { 1.f, 1.f };
	_float				m_fWithProgress = 1.f;
	_bool				m_bTrigger = false;

protected: /* ============================ bool =============================== */
	_bool				m_bPick = false;
	//_bool				m_bSelect = false;
	//_bool				m_bSelectPressing = false;
	_uint				m_iButtonState = {};
	_bool				m_bChange_Proj = false;

public:
	//void				Set_Select(_bool bSelect) { m_bSelect = bSelect; }
	//_bool				Get_Select() { return m_bSelect; }
	//void				Set_SelectPressing(_bool bSelectPressing) { m_bSelectPressing = bSelectPressing; }
	//_bool				Get_SelectPressing() { return m_bSelectPressing; };

public:
	void	ChangeProj() { m_bChange_Proj = true; }

public:
	virtual void		 Free() override;

};

END