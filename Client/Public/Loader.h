#pragma once

#include "Client_Defines.h"
#include "Base.h"

/* 스레드를 생성한다. */
/* 생성한 스레드로 필요한 레벨의 자원을 로딩한다. */

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CLoader final : public CBase
{

private:
	CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLoader() = default;

public:
	_bool isFinished() const { return m_isFinished; }

public:
	HRESULT Initialize(LEVEL eNextLevelID);
	void	Print_LoadingText();

public:
	HRESULT Loading();
	HRESULT Loading_For_Logo_Level();

	
	HRESULT Loading_For_GamePlay_Level_Origin(LEVEL eLEVEL);
	HRESULT Loading_For_GamePlay_Level();

	HRESULT Loading_For_Intro_Level();
	HRESULT Loading_For_IntroBoss_Level();

	HRESULT Loading_For_SnowMountain_Level();
	HRESULT Loading_For_SnowMountainBoss_Level();
	HRESULT Loading_For_Lava_Level();

	HRESULT	Loading_For_Tool_Level();

	HRESULT	Loading_Player(LEVEL eLEVEL);
	HRESULT Loading_For_Shader(LEVEL eLevel);

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	CGameInstance*			m_pGameInstance = { nullptr };

	/* For. LoadingThread */
	HANDLE					m_hThread;
	CRITICAL_SECTION		m_CriticalSection;

private:
	LEVEL					m_eNextLevelID				= { LEVEL_END };
	_tchar					m_szLoadingText[MAX_PATH]	= TEXT("");
	_bool					m_isFinished				= { false };

private:
	HRESULT	Ready_Origin();
	HRESULT	Ready_UI_Origin();
	HRESULT Ready_Environment_Model(LEVEL eLevel); //TODO 승용
	HRESULT Read_FBXModelPath(const _tchar* StartDirectoryPath, LEVEL eLevel, _int iAnimType); //! 시작 디렉토리 부터 안에 있는 경로를 전부 탐색 , 승용
	HRESULT Read_FBXModelPath_NoScale(const _tchar* StartDirectoryPath, LEVEL eLevel, _int iAnimType); //! 백배 스케일 안함


/* For.Effect */
private:
	HRESULT Ready_Effect_Origin();
	HRESULT Ready_Effect_All(LEVEL eLevel);
	HRESULT Ready_Effect_Texture(LEVEL eLevel);
	HRESULT Ready_Effect_Model(LEVEL eLevel);
	HRESULT Ready_Effect_Buffer(LEVEL eLevel);


public:
	static CLoader * Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, LEVEL eNextLevelID);
	virtual void Free() override;

};

END