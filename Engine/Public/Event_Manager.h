#pragma once

#include "Base.h"

BEGIN(Engine)

class CEvent_Manager final : public CBase
{
private:
	CEvent_Manager();
	virtual ~CEvent_Manager() = default;

	HRESULT Initialize();


private:
	_uint			m_iCurrentEvnet = { 0 };
	class CGameInstance* m_pGameInstance = { nullptr };

public:
	static CEvent_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};

END