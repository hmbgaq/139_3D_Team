#pragma once

#include "Base.h"

BEGIN(Engine)

class CEvent_Manager final : public CBase
{
private:
	CEvent_Manager();
	virtual ~CEvent_Manager() = default;


public:
	static CEvent_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};

END