#pragma once

#include "Base.h"

BEGIN(Engine)

class CCollider;

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

	/* Ãß°¡ RenderGroup¿ë*/
	virtual HRESULT Render_Shadow() { return S_OK; }
	virtual HRESULT Render_OutLine() { return S_OK; }

	virtual _bool	Picking(_Out_ _float3* vPickedPos);
	


public:
	virtual class CComponent* Find_Component(const wstring& strComTag, const wstring& strPartTag = TEXT(""));
public:
	void	Set_Position(const _float3& vState);
	void	Set_WorldMatrix(_float4x4 matrix);
public:
	_bool	Is_Dead() { return m_bDead; }
	void	Set_Dead(_bool _bDead) { m_bDead = _bDead; }

public:
	virtual _bool Write_Json(json& Out_Json) override;
	virtual void Load_FromJson(const json& In_Json) override;

public:
	class CTransform* Get_Transform();


public:
	virtual void	OnCollisionEnter(CCollider* other) {};
	virtual void	OnCollisionStay(CCollider* other) {};
	virtual void	OnCollisionExit(CCollider* other) {};


public:


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
	HRESULT Remove_Component(const wstring& strComTag, _Inout_ CComponent** ppOut = nullptr);


public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END