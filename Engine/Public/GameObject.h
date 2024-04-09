#pragma once

#include "Base.h"

BEGIN(Engine)

class CCollider;
class CPhysXCollider;

class ENGINE_DLL CGameObject abstract : public CBase
{

public:
	enum DESCTYPE { OBJECT_DESC, MONSTER_DESC, DESCTYPE_END };

public:
	typedef struct tagGameObjectDesc
	{
		tagGameObjectDesc() {}
		tagGameObjectDesc(_float fSpeed, _float fRotation)
			: fSpeedPerSec(fSpeed), fRotationPerSec(fRotation) {}

		_float	fSpeedPerSec = 0.f;
		_float	fRotationPerSec = 0.f;

		DESCTYPE eDescType = DESCTYPE_END;
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

public: /* For. Additional Render */
	virtual HRESULT Render_Shadow() { return S_OK; }
	virtual HRESULT Render_CSM(_uint i) { return S_OK; }
	virtual HRESULT Render_OutLine() { return S_OK; }
	virtual HRESULT Render_Ice() { return S_OK; }

public: /* For. Picking */
	virtual _bool	Picking(_Out_ _float3 * vPickedPos);

public: /* For. Component */
	virtual class CComponent* Find_Component(const wstring & strComTag, const wstring & strPartTag = TEXT(""));

public: /* For. Json */
	virtual _bool		Write_Json(json & Out_Json) override;
	virtual void		Load_FromJson(const json & In_Json) override;

public: /* For. Transform */
	class CTransform*	Get_Transform();
	_vector				Get_Position_Vector();
	_float3				Get_Position();
	_float3				Get_Look();
	_vector				Calc_Look_Dir(_vector vTargetPos);
	_vector				Calc_Look_Dir_XZ(_vector vTargetPos);

	void				Set_Position(const _float3& vState);
	void				Set_WorldMatrix(_float4x4 matrix);
	virtual HRESULT		Set_InitPosition(const _float3& vPos);

public: /* For. Model */
	void				Set_ModelWidth(_float fWidth) { m_fModelWidth = fWidth; }
	void				Set_ModelHeight(_float fHeight) { m_fModelHeight = fHeight; }
	_float				Get_ModelWidth() { return m_fModelWidth; }
	_float 				Get_ModelHeight() { return m_fModelHeight; }

public: /*For. Render */
	void Set_RenderPass(_int iPass) { m_iRenderPass = iPass; }
	_int Get_Rednerpass() { return m_iRenderPass; }
	_bool				m_bORM_Available		= false;
	_bool				m_bEmissive_Available	= false;
	_float				m_fCamFar = {};

public: /* For. Base Setting */
	const wstring&		Get_ProtoTypeTag() { return m_strPrototypeTag; };
	_bool				Is_PoolObject() { return m_bIsPoolObject; };

	CGameObject*		Get_Object_Owner();
	void				Set_Object_Owner(CGameObject* pOwner);
	void				Delete_Object_Owner();

	virtual void		Set_Enable(_bool _Enable) override;
	_bool				Is_Dead() { return m_bDead; }
	void				Set_Dead(_bool _bDead) { m_bDead = _bDead; }

public: /* For. Collision */
	virtual void		OnCollisionEnter(CCollider * other) {};
	virtual void		OnCollisionStay(CCollider * other) {};
	virtual void		OnCollisionExit(CCollider * other) {};

	virtual void		OnPhysXCollisionEnter(CPhysXCollider* pOtherCollider) {};
	virtual void		OnPhysXCollisionStay(CPhysXCollider* pOtherCollider) {};
	virtual void		OnPhysXCollisionExit(CPhysXCollider* pOtherCollider) {};

protected: /* Base Setting Value */
	map<const wstring, CComponent*>		m_Components;

	ID3D11Device*								m_pDevice			= { nullptr };
	ID3D11DeviceContext*						m_pContext			= { nullptr };

	class CGameInstance*						m_pGameInstance		= { nullptr };
	class CTransform*							m_pTransformCom		= { nullptr };
	CGameObject*								m_pOwner			= { nullptr };

	_uint										m_iCurrnetLevel		= { 0 };
	_int										m_iRenderPass		= { 0 };

	_bool										m_isCloned			= { false };
	_bool										m_bDead				= { false };
	_bool										m_bIsPoolObject		= { false };
	wstring										m_strPrototypeTag	= {};

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