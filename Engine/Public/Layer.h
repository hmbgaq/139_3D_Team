#pragma once

#include "Base.h"

BEGIN(Engine)

class CComponent;
class CGameObject;

class CLayer final : public CBase
{
public:
	CLayer();
	virtual ~CLayer() = default;

public:
	shared_ptr<CComponent> Get_Component(const wstring & strComponentTag, _uint iIndex, const wstring& strPartTag);

public:
	HRESULT Add_GameObject(shared_ptr<CGameObject> pGameObject);
	void Priority_Tick(_float fTimeDelta);
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);

private:
	list<shared_ptr<CGameObject>>			m_GameObjects;

public:
	static shared_ptr<CLayer> Create();
	virtual void Free() override;
};

END

