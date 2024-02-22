#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Renderer.h"

class CUI abstract : public CGameObject
{
public:
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

		/* 크기 */
		_float		fScaleX = 100.f;
		_float		fScaleY = 100.f;

		/* 회전 */
		_float		fRotationX = 0.0f;
		_float		fRotationY = 0.0f;

		/* 이동 */
		_float		fPositionX = (_float)g_iWinSizeX / 2;
		_float		fPositionY = (_float)g_iWinSizeY / 2;
		_float		fPositionZ = 0.0f;

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
			=셰이더 패스 선택=
			iShaderNum
			=테그 값=
			strLayerTag
			strCloneTag
			strProtoTag
			strFilePath
			strMapTextureTag
			=색상=
			vColor
		*/

		/* 알파 */
		_float		fAlpha = 1.f;

		_int		iShaderNum = 0;			// 적용할 셰이더 넘버

		string		strLayerTag = "";
		string		strCloneTag = "";
		string		strProtoTag = "";
		string		strFilePath = "";
		string		strMapTextureTag = "";	// 적용할 맵 텍스처

		/* 색상 */
		_vector		vColor = { 1.f, 1.f, 1.f, 1.f };

	}UI_DESC;

	enum UI_BUTTON_STATE
	{
		BUTTON_NORMAL, BUTTON_PICKING, BUTTON_PICKED, BUTTON_END
	};

protected:
	CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CUI(const CGameObject& rhs);
	virtual ~CUI() = default;

public: /* ============================== Get / Set =============================== */
	// =>UIDesc
	UI_DESC			Get_UIDesc() { return m_tUIInfo; }
	void			Set_UIDesc(UI_DESC UIDesc) { m_tUIInfo = UIDesc; }
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

public: /* ============================== Add ============================== */
	void			Create_Add_UIParts(void* pArg);
	void			Add_UIParts(CUI* pArg);

public: /* ========================== Change_Size ========================== */
	void			Set_Size(_float fSizeX, _float fSizeY);
	void			Change_SizeBottom(_float MMY);
	void			Change_SizeTop(_float MMY);
	void			Change_SizeLeft(_float MMX);
	void			Change_SizeRight(_float MMX);
	void			Change_SizeY(_float MMY);
	void			Change_SizeX(_float MMX);

public: /* ============================== Basic =============================== */
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void	Priority_Tick(_float fTimeDelta);
	virtual void	Tick(_float fTimeDelta);
	virtual void	UI_AppearTick(_float fTimeDelta);
	virtual void	UI_DisappearTick(_float fTimeDelta);
	virtual void	Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

public: /* ============================= Function ============================= */
	virtual void	Picking_UI();	// Pick
	virtual void	Check_RectPos();	// Moving
	void			Moving_Picking_Point(POINT pt); // Picking Moving

public: /* ============================== SetUp ============================== */
	HRESULT			SetUp_UIRect(_float fPosX, _float fPosY, _float fSizeX = 1.f, _float fSizeY = 1.f);
	HRESULT			SetUp_BillBoarding();
	HRESULT			SetUp_Transform(_float fPosX, _float fPosY, _float fSizeX = 1.f, _float fSizeY = 1.f);
	HRESULT			Ready_UI(const char* cFilePath);
	HRESULT			Create_UIParts(UI_DESC tUI_Desc);

protected: /* =========================== Ready ============================= */
	virtual HRESULT Ready_Components();
	virtual HRESULT Bind_ShaderResources();

public: /* =========================== Save/Load ============================== */
	void			Load_UIData(const char* _FilePath);
	virtual json	Save_Desc(json& out_json);

protected: /* ========================= Component =========================== */
	CShader*			m_pShaderCom = { nullptr };
	//CTexture*			m_pTextureCom;
	CTexture*			m_pMapTextureCom = { nullptr };	// 적용할 맵 텍스처
	_int				iShaderNum;		// 적용할 셰이더 넘버
	CVIBuffer_Rect*		m_pVIBufferCom = { nullptr };

protected: /* =========================== Space ============================ */
	_float4x4			m_ViewMatrix, m_ProjMatrix;

protected: /* =========================== Screen ============================ */
	_float				m_fWorldToScreenX = 0.f;
	_float				m_fWorldToScreenY = 0.f;

protected: /* ============================= UI =============================== */
	vector<CUI*>		m_vecUIParts;
	UI_DESC				m_tUIInfo;
	RECT				m_rcUI = {};
	UISTATE				m_eState;
	
	// UI_Member
	_float				m_fPositionX = 0.f, m_fPositionY = 0.f;
	_float				m_fScaleX = 0.f, m_fScaleY = 0.f;

protected: /* ============================ bool =============================== */
	_bool				m_bPick = false;
	_uint				m_iButtonState = {};

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void		 Free() override;

};