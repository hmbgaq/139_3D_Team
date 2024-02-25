#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CDevConsole : public CBase
{
private:
	CDevConsole();
	virtual ~CDevConsole() = default;
	
public:
	virtual HRESULT Initialize();
	virtual void Tick();

private:
	HANDLE	m_hOut;





public:
	static CDevConsole* Create();
	virtual void Free() override;

};

END