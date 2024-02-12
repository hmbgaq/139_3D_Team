#pragma once

#include "Base.h"

/* 생성한 원형 객체를 모아놓는다. (프로토타입) */
/* 원형객체를 복제하여 생성한 사본객체를 레이어로 구분하여 모아놓는다. */
/* 보관하고 있는 사본객체들의 Tick관련 함수를 반복적으로 호출해준다.  */



BEGIN(Engine)

class CLayer;
class CComponent;
class CGameObject;

class CObject_Manager final : public CBase
{
private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	shared_ptr<CComponent> Get_Component(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComponentTag, _uint iIndex, const wstring& strPartTag);

public:
	HRESULT Initialize(_uint iNumLevels);
	HRESULT Add_Prototype(const wstring& strPrototypeTag, shared_ptr<CGameObject> pPrototype);
	HRESULT Add_CloneObject(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg);
	shared_ptr<CGameObject> Clone_Prototype(const wstring& strPrototypeTag, void* pArg);
	void Priority_Tick(_float fTimeDelta);
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);
	void Clear(_uint iLevelIndex);

private:
	_uint			m_iNumLevels = { 0 };

private:
	map<const wstring, shared_ptr<CGameObject>>			m_Prototypes;
	map<const wstring, shared_ptr<CLayer>>*				m_pLayers = { nullptr } ;
	typedef map<const wstring, shared_ptr<CLayer>>		LAYERS;

private:
	shared_ptr<CGameObject> Find_Prototype(const wstring& strPrototypeTag);
	shared_ptr<CLayer> Find_Layer(_uint iLevelIndex, const wstring& strLayerTag);
public:
	static CObject_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};

END