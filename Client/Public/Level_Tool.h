#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Tool final : public CLevel
{
private:
	CLevel_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Tool() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Imgui();

private:
	HRESULT Ready_Layer_Camera(const wstring& strLayerTag);

public:
	HRESULT Ready_EffectTool();
	HRESULT Ready_Layer_Particle(const wstring& strLayerTag);

public:
	static CLevel_Tool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;

private:
	class CImgui_Manager*	m_pImguiManager = { nullptr };

};

END