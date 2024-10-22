#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_SnowMountain final : public CLevel
{
private:
	CLevel_SnowMountain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_SnowMountain() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_LightDesc();
	HRESULT Ready_Layer_Camera(const wstring& strLayerTag);
	HRESULT Ready_Layer_BackGround(const wstring& strLayerTag);

	HRESULT Ready_Layer_Player(const wstring& strLayerTag);
	HRESULT Ready_Layer_Monster(const wstring& strLayerTag);
	HRESULT Ready_Layer_Test(const wstring& strLayerTag);
	HRESULT Ready_Effect();


private: /* UI */
	HRESULT Ready_UI();
	HRESULT Ready_Layer_UI_Monster(const wstring& strLayerTag, void* pArg);
	HRESULT Ready_Layer_UI_Player(const wstring& strLayerTag, void* pArg);

private: /* Map*/
	HRESULT Ready_Event();

private: /* Shader */
	HRESULT Ready_Shader(const string& strShaderFilePath);

private:
	_int			TempLightNumber = 0;
	
	//string		m_strMapLoadPath = "../Bin/DataFiles/Data_Map/SnowMountain_MapData.json";
	string			m_strMapLoadPath = "../Bin/DataFiles/Data_Map/SnowMountain_MapData.json";

	class CEffect*	m_pEffect = { nullptr };
	class CEffect*	m_pMapEffect = { nullptr };

public:
	static CLevel_SnowMountain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END