#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)
class CData_Manager;
class CUI_Manager;

class CLevel_Logo final : public CLevel
{
private:
	CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Logo() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Layer_BackGround(const wstring& strLayerTag);
	HRESULT Ready_Static_UI();

private:
	void Set_Filter();
	class CData_Manager* m_pDataManager = nullptr;
	class CUI_Manager* m_pUIManager = nullptr;
	_bool				m_bUI_ReadyOK = false;

public:
	static CLevel_Logo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END