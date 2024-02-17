#pragma once
#include "Client_Defines.h"
#include "Engine_Macro.h"
#include "Base.h"

BEGIN(Engine)
class CGameObject;
END


BEGIN(Client)

class CData_Manager : public CBase
{
	DECLARE_SINGLETON(CData_Manager);


private:
	CData_Manager();
	virtual ~CData_Manager() = default;




private:
	class CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;

public:
	virtual void	Free();
};

END

