#pragma once
#include "Client_Defines.h"
#include "Engine_Macro.h"
#include "Base.h"

BEGIN(Engine)
class CGameObject;
class CCamera;
END


BEGIN(Client)

class CPlayer;
class CCamera_Dynamic;

class CData_Manager : public CBase
{
	DECLARE_SINGLETON(CData_Manager);

private:
	CData_Manager();
	virtual ~CData_Manager() = default;

public:
	HRESULT		Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	
public: 
	void Set_Player(CPlayer* _pPlayer);
	CPlayer* Get_Player();
	void Reset_Player(LEVEL eLEVEL);

public:
	void Set_Camera_Dynamic(CCamera_Dynamic* m_pCamera_Dynamic);
	CCamera_Dynamic* Get_Camera_Dynamic();
	void Reset_Camera_Dynamic(LEVEL eLEVEL);





private:
	CPlayer* m_pPlayer = { nullptr };
	CCamera_Dynamic* m_pCamera_Dynamic = { nullptr };

private:
	CCamera* m_pCamera = { nullptr };

	
private:
	class CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };



public:
	virtual void	Free();
};

END

