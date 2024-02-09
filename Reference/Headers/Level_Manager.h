#pragma once

#include "Base.h"

/* 현재 레벨의 틱, 렌더를 무한히 호출한다. */

/* Open_Level() */
/* 현재 화면에 보여줘야할 레벨의 주소를 갖는다. */
/* 이전 레벨을 삭제한다. */
/* 이전 레벨용 자원들을 정리한다. */

BEGIN(Engine)

class CLevel_Manager final : public CBase
{
private:
	CLevel_Manager();
	virtual ~CLevel_Manager() = default;

public:
	HRESULT Initialize();
	void Tick(_float fTimeDelta);
	HRESULT Render();

public:
	HRESULT Open_Level(_uint iCurrentLevelIndex, class CLevel* pNewLevel);

private:
	_uint						m_iCurrentLevelIndex = { 0 };
	class CLevel*				m_pCurrentLevel = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };

public:
	static CLevel_Manager* Create();
	virtual void Free() override;
};

END