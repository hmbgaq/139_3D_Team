#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CComponent abstract : public CBase
{
protected:
	CComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CComponent(const CComponent& rhs);
	virtual ~CComponent() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta) {};
	virtual void Late_Tick(_float fTimeDelta) {};

#ifdef _DEBUG
	virtual HRESULT Render() {
		return S_OK;
	}
#endif

public:
	HRESULT	Set_Owner(class CGameObject* _pOwner);
	class CGameObject* Get_Owner();

protected:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	class CGameInstance*	m_pGameInstance = { nullptr };

protected:
	_bool					m_isCloned = { false };
	class CGameObject*		m_pOwner = { nullptr };


public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END