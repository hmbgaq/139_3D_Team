#pragma once
#include "Client_Defines.h"
#include "UI_Text.h"
#include "Texture.h"

/* 체력 프레임 */
class CUI_Option_SelectBox final : public CUI_Text
{
private:
	CUI_Option_SelectBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CUI_Option_SelectBox(const CUI_Option_SelectBox& rhs);
	virtual ~CUI_Option_SelectBox() = default;

public:
	virtual HRESULT			Initialize_Prototype() override; //! 원형객체의 초기화를 위한 함수.
	virtual HRESULT			Initialize(void* pArg) override; //! 사본객체의 초기화를 위한 함수. ( void*를 매개인자로 받아 특수한 사본객체 처리가 가능하다. )
	virtual void			Priority_Tick(_float fTimeDelta) override;
	virtual void			Tick(_float fTimeDelta) override;
	virtual void			Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	virtual void			Add_Text(string strTextKey, string strFontTag, string strText, _float fPosX, _float fPosY, _vector vColor, _float fScale, _float2 vOrigin, _float fRotation) override;
	//virtual TEXTINFO*		Find_Text(const string& strTextTag) override;
	virtual HRESULT			Find_Change(const string& strTextTag) override;
	//virtual void			Change_Text(string strTextKey, string strFontTag, string strText, _float fPosX, _float fPosY, _vector vColor) override;

private:
	virtual HRESULT			Ready_Components() override;
	virtual HRESULT			Bind_ShaderResources() override;
	HRESULT					Ready_Text();

private:
	void					Compute_OwnerCamDistance();
	_bool					In_Frustum();

public:
	virtual json			Save_Desc(json& out_json);
	void					Load_Desc();

public:
	static CUI_Option_SelectBox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag); //! 원형객체 생성
	virtual CGameObject* Clone(void* pArg) override; //! 사본객체 생성
	virtual CGameObject* Pool() override;
	virtual void			Free() override;
};

