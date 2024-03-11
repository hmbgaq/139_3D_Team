#pragma once

#include "Base.h"

BEGIN(Engine)

class CCollider;
class CPhysXCollider;

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
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Tick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

	/* 추가 RenderGroup용*/
	virtual HRESULT Render_Shadow() { return S_OK; }
	virtual HRESULT Render_OutLine() { return S_OK; }
	virtual HRESULT Render_Cascade_Shadow(_uint iIndex) { return S_OK; } /* 대기 */

	virtual _bool	Picking(_Out_ _float3 * vPickedPos);



public:
	virtual class CComponent* Find_Component(const wstring & strComTag, const wstring & strPartTag = TEXT(""));
public:
	void	Set_Position(const _float3 & vState);
	void	Set_WorldMatrix(_float4x4 matrix);

public:
	_bool	Is_Dead() { return m_bDead; }
	void	Set_Dead(_bool _bDead) { m_bDead = _bDead; }

public:
	virtual void Set_Enable(_bool _Enable) override;

public:
	void	Set_ModelWidth(_float fWidth) { m_fModelWidth = fWidth;}
	void	Set_ModelHeight(_float fHeight) { m_fModelHeight = fHeight; }
	_float  Get_ModelWidth() { return m_fModelWidth;}
	_float 	Get_ModelHeight() { return m_fModelHeight; }

public:
	virtual _bool Write_Json(json & Out_Json) override;
	virtual void Load_FromJson(const json & In_Json) override;

public:
	class CTransform* Get_Transform();
	_vector Get_Position_Vector();
	_float3 Get_Position();
	_vector Calc_Look_Dir(_vector vTargetPos);


public:
	virtual void	OnCollisionEnter(CCollider * other) {};
	virtual void	OnCollisionStay(CCollider * other) {};
	virtual void	OnCollisionExit(CCollider * other) {};

public:
	virtual void OnPhysXCollisionEnter(CPhysXCollider* pOtherCollider) {};
	virtual void OnPhysXCollisionStay(CPhysXCollider* pOtherCollider) {};
	virtual void OnPhysXCollisionExit(CPhysXCollider* pOtherCollider) {};



public:
	const wstring& Get_ProtoTypeTag() { return m_strPrototypeTag; };

	_bool Is_PoolObject() { return m_bIsPoolObject; };

public:
	CGameObject*	Get_Object_Owner();
	void			Set_Object_Owner(CGameObject* pOwner);
	void			Delete_Object_Owner();
	



protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

protected:
	class CGameInstance* m_pGameInstance = { nullptr };

protected:
	class CTransform* m_pTransformCom = { nullptr };
	map<const wstring, class CComponent*>		m_Components;

protected:
	_bool						m_isCloned = { false };

protected:
	_bool						m_bDead = { false };
	_bool						m_bIsPoolObject = { false };

protected:
	wstring						m_strPrototypeTag;

protected:
	CGameObject*				m_pOwner = { nullptr };


protected: //! For.Tool
	_float						m_fModelWidth = { 0.f };
	_float						m_fModelHeight = { 0.f };


public:
	HRESULT Add_Component(_uint iLevelIndex, const wstring& strPrototypeTag,
		const wstring& strComTag, _Inout_ CComponent** ppOut, void* pArg = nullptr);
	HRESULT Remove_Component(const wstring& strComTag, _Inout_ CComponent** ppOut = nullptr);


public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual CGameObject* Pool() PURE;
	virtual void Free() override;
};

END