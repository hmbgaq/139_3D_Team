#pragma once
#include "Client_Defines.h"
#include "Engine_Macro.h"

#include "Base.h"
#include "GameInstance.h"


BEGIN(Client)
class CEffect;


//! EFFECT_MANAGER 매크로 : GetInstance()안해도 됨. (EFFECT_MANAGER->Create_Effect(~~);)

class CEffect_Manager : public CBase
{
	DECLARE_SINGLETON(CEffect_Manager);

private:
	CEffect_Manager();
	virtual ~CEffect_Manager() = default;

public:
	HRESULT		Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:	
	//! 유정 : strFileName은 json파일 이름. ("../Bin/DataFiles/Data_Effect/" 경로에 저장해 놓은 json파일 중 원하는거 골라쓰기!)
	CEffect* Create_Effect(_uint iLevelIndex, const wstring& strLayerTag, string strFileName, CGameObject* pOwner = nullptr);


private:
	class CGameInstance*	m_pGameInstance = { nullptr };
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };


public:
	virtual void	Free();
};

END

