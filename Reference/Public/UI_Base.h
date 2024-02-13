#pragma once
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;

/* UI전용 셰이더를 사용하여 그린다. 
	UI랜더 순서는 레이어에 생성하는 순서로 관리한다.
	ex)
	1. HpFrame  Add
	2. HpBar	Add
	ㄴ> 테두리 위에 Hp바
*/

/* UI_Base를 클래스에서 상속받아 UI를 생성시켜준다. */
class ENGINE_DLL CUI_Base abstract: public CGameObject
{
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


	typedef struct tagTextureUVRatio
	{
		_float	fOriginLeft = 0.f;
		_float	fOriginRight = 0.f;
		_float	fOriginTop = 0.f;
		_float	fOriginBottom = 0.f;
		_float	fLeft = 0.f;
		_float	fRight = 0.f;
		_float	fTop = 0.f;
		_float	fBottom = 0.f;
	}TEXUVRATIO;

protected:
	CUI_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Base(const CGameObject& rhs);
	virtual ~CUI_Base() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void	Priority_Tick(_float fTimeDelta);
	virtual void	Tick(_float fTimeDelta);
	virtual void	Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();


protected:
	virtual HRESULT Ready_Components();
	virtual HRESULT Bind_ShaderResources();

protected:
	void			SetUp_UV(_uint iTextureIndex = 0);
	void			SetUp_WorldToScreen(_fvector vWorldPos);
	//void			SetUp_ScreenToWorld(_float fScreenX, _float fScreenY);
	HRESULT			SetUp_Transform(_float fPosX, _float fPosY, _float fSizeX = 1.f, _float fSizeY = 1.f);
	HRESULT			SetUp_BillBoarding();
	HRESULT			SetUp_ScreenPosRect(_float fPosX, _float fPosY, _float fSizeX = 1.f, _float fSizeY = 1.f);

	void			Set_Enable(_bool bEnable) { m_isEnable = bEnable; }

public:
	const RECT& Get_ScreenPosRect() const { return m_ScreenPosRect; }



protected:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

protected:
	UI_DESC				m_tUIInfo = {};
	TEXUVRATIO			m_tTexUVInfo = {};

	_float4x4			m_ViewMatrix, m_ProjMatrix;

protected:
	_float				m_fWorldToScreenX;
	_float				m_fWorldToScreenY;

	RECT				m_ScreenPosRect;
	_bool				m_isEnable = true;



public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void		 Free() override;
};

END

