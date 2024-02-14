#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	typedef struct tagGameObjectDesc
	{
		tagGameObjectDesc() {}
		tagGameObjectDesc(_float fSpeed, _float fRotation)
			: fSpeedPerSec(fSpeed), fRotationPerSec(fRotation) {}

		_float	fSpeedPerSec = 0.f;
		_float	fRotationPerSec = 0.f;

	}GAMEOBJECT_DESC;
protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Tick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_Shadow() { return S_OK; }

public:
	virtual class CComponent* Find_Component(const wstring& strComTag, const wstring& strPartTag = TEXT(""));

public:
	_bool Is_Dead() { return m_bDead; }
	void Set_Dead(_bool _bDead) { m_bDead = _bDead; }

public:
	class CTransform* Get_Transform();


protected:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

protected:
	class CGameInstance*		m_pGameInstance = { nullptr };

protected:
	class CTransform*							m_pTransformCom = { nullptr };
	map<const wstring, class CComponent*>		m_Components;

protected:
	_bool						m_isCloned = { false };

protected:
	_bool						m_bDead = { false };

protected:
	HRESULT Add_Component(_uint iLevelIndex, const wstring& strPrototypeTag,
		const wstring& strComTag, _Inout_ CComponent** ppOut, void* pArg = nullptr);




public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END