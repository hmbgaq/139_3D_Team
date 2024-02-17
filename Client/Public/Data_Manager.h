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

public:
	HRESULT		Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	
public: //Get Set 함수 작성
	_int Get_Player_Hp() {
		return m_iPlayerHp;
	};
	void Set_Player_Hp(_int _iPlayerHp) {
		m_iPlayerHp = _iPlayerHp;
	}
	

private:
	// Player
	_int m_iPlayerHp = { 0 };

	
private:
	class CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };



public:
	virtual void	Free();
};

END

