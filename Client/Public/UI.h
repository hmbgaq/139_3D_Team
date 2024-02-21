#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Renderer.h"

class CUI abstract : public CGameObject
{
public:
	typedef struct tagUIDesc
	{
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

		/* 알파 */
		_float		fAlpha = 1.f;

		/* 색상 */
		_vector		vColor;

		string		strCloneTag = "Not Added";
		string		strProtoTag = "Not Added";
		string		strFilePath = "Not Added";

		/* 랜더그룹 */
		CRenderer::RENDERGROUP eRenderGroup = CRenderer::RENDER_UI;

	}UI_DESC;

protected:
	CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CUI(const CGameObject& rhs);
	virtual ~CUI() = default;

public: /* ============================== Get =============================== */
	void			Set_UIDesc(UI_DESC UIDesc) { m_tUIInfo = UIDesc; }
	_bool			Set_Pick(_bool Pick) { m_bPick = Pick; }

public: /* ============================== Set =============================== */
	UI_DESC			Get_UIDesc() { return m_tUIInfo; }
	_bool			Get_Pick() { return m_bPick; }

public: /* =========================== Set_Position =========================== */
	void			Set_Pos(_float2 vPos);
	void			Set_PosZ(_float fZ);

public: /* ========================== Set_Rect_Scale ========================== */
	void			Set_Size(_float fSizeX, _float fSizeY);
	void			Change_SizeBottom(_float MMY);
	void			Change_SizeTop(_float MMY);
	void			Change_SizeLeft(_float MMX);
	void			Change_SizeRight(_float MMX);
	void			Change_SizeY(_float MMY);
	void			Change_SizeX(_float MMX);

public: /* ============================= Basic =============================== */
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

protected: /* =========================== Ready ============================= */
	virtual HRESULT Ready_Components();
	virtual HRESULT Bind_ShaderResources();

protected: /* =========================== SetUp ============================= */
	HRESULT			SetUp_UIRect(_float fPosX, _float fPosY, _float fSizeX = 1.f, _float fSizeY = 1.f);
	HRESULT			SetUp_BillBoarding();
	HRESULT			SetUp_Transform(_float fPosX, _float fPosY, _float fSizeX = 1.f, _float fSizeY = 1.f);

protected: /* =========================== Load ============================= */
	void			Load_UIData(const char* _FilePath);

protected: /* ========================= Component =========================== */
	CShader*			m_pShaderCom = { nullptr };
	CTexture*			m_pTextureCom = { nullptr };
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

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void		 Free() override;

};