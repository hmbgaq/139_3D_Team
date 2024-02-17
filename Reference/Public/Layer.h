#pragma once

#include "Base.h"

BEGIN(Engine)

class CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	class CComponent* Get_Component(const wstring & strComponentTag, _uint iIndex, const wstring& strPartTag);

public:
	HRESULT Add_GameObject(class CGameObject* pGameObject);
	void Priority_Tick(_float fTimeDelta);
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);

public:
	list<class CGameObject*>* Get_GameObjects() { return &m_GameObjects; };

public:
	void Set_Speed(_float _fSpeed) {
		m_fSpeed = _fSpeed;
	};

	void Set_Stop(_bool _bStop) {
		m_bStop = _bStop;
	};

	_bool Get_Stop() {
		return m_bStop;
	}


private:
	list<class CGameObject*>			m_GameObjects;
	_float								m_fSpeed = {1.f};
	_bool								m_bStop = { false };

public:
	static CLayer* Create();
	virtual void Free() override;
};

END