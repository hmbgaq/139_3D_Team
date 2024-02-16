#pragma once

#include "Imgui_Window.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)
class CEnvironment_Instance;
class CEnvironment_Object;


class CWindow_MapTool final : public CImgui_Window
{
public:
	struct WINDOW_MAPTOOL_DESC : public ImGuiDESC
	{
	};

protected:
	CWindow_MapTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CWindow_MapTool() = default;


public:
	virtual	HRESULT Initialize() override;
	virtual	void	Tick(_float fTimeDelta) override;	
	virtual void	Render() override;


private:
	HRESULT			Ready_ModelTags();

private:
	void			GroundTab_Function();
		//void			Create_EnvironmentFunction();

	void			InteractTab_Function();
		//void			Create_EnvironmentFunction();

	void			EnvironmentTab_Function();
		//void			Create_EnvironmentFunction();

private:
	ImTextureID		m_pSelectedTexture = { nullptr };
	vector<string>  m_vecNonAnimModelTag;
	vector<string>  m_vecAnimModelTag;

private:
	vector<CEnvironment_Object*>	m_vecCreateObject = {}; //! 생성한 오브젝트
	CEnvironment_Object*			m_pPreviewObject = {}; //! 미리보기를 위해 클론시킨 오브젝트.
	
private:
	vector<CEnvironment_Instance*>	m_vecCreateInstance = {};

	vector<CEnvironment_Object*>	m_vecPreViewInstance = {}; //! 인스턴싱 디스크립션 만들기 위해.
	vector<INSTANCE_INFO_DESC>		m_vecInstanceInfo = {}; //! m_vecPreViewInstance를 픽킹해서 인스턴싱 디스크립션을 채워 준후 m_vecCreateInstance를 만들어주자


public:
	static CWindow_MapTool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END

