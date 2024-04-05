#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_SnowMountainBoss final : public CLevel
{
private:
	CLevel_SnowMountainBoss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_SnowMountainBoss() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_LightDesc();
	HRESULT Ready_Layer_Camera(const wstring& strLayerTag);
	HRESULT Ready_Layer_Effect(const wstring& strLayerTag);
	HRESULT Ready_Layer_BackGround(const wstring& strLayerTag);

	
	HRESULT Ready_Layer_Player(const wstring& strLayerTag);
	HRESULT Ready_Layer_Monster(const wstring& strLayerTag);
	HRESULT Ready_Layer_Test(const wstring& strLayerTag);

private: /* UI */
	HRESULT Ready_UI();
	HRESULT Ready_Layer_UI_Monster(const wstring& strLayerTag, void* pArg);
	HRESULT Ready_Layer_UI_Player(const wstring& strLayerTag, void* pArg);

private: /* Map*/
	HRESULT Ready_Event();

private: /* Shader */
	HRESULT Ready_Shader();

private:
	_int			TempLightNumber = 0;
	string			m_strMapLoadPath = "../Bin/DataFiles/Data_Map/Stage2Boss_MapData_MapData.json";

public:
	static CLevel_SnowMountainBoss* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END