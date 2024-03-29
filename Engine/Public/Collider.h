#pragma once

#include "Component.h"

#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"

/* 충돌체. */
/* 충돌체를 정의하는 기능 + 충돌체을 그리는 기능 + 충돌을 비교하는 기능 . */

BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
public:
	enum TYPE { TYPE_SPHERE, TYPE_AABB, TYPE_OBB, TYPE_END};

private:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;

public:
	TYPE Get_ColliderType() const {	return m_eType;	}
	class CBounding* Get_Bounding() { return m_pBounding; }

	void  Set_Bounding(CBounding* _Bounding) { m_pBounding = _Bounding; }
	void  Set_isCollision(_bool _isCollision) { m_isCollision = _isCollision; }

	void Set_ColliderSize(_float _sizeX, _float _sizeY, _float _sizeZ);

public:
	virtual HRESULT Initialize_Prototype(TYPE eType);
	virtual HRESULT Initialize(void* pArg);
public:
	void Update(_fmatrix TransformMatrix);
	_bool Is_Collision(CCollider* pTargetCollider);
	void Collision(CCollider* pTargetCollider);
	void End_CollisionCheck();


public:
	virtual void			OnCollisionEnter(CCollider* other);
	virtual void			OnCollisionStay(CCollider* other);
	virtual void			OnCollisionExit(CCollider* other);

public:
	virtual void Set_Enable(_bool _Enable) override;

public:
	_uint Get_Layer() { return m_iLayer; }

#ifdef _DEBUG
public:
	virtual HRESULT Render();
#endif

	TYPE							m_eType = { TYPE_END };
	class CBounding*				m_pBounding = { nullptr };
	_bool							m_isCollision = { false };

	list<CCollider*>				m_PreOtherColliders;
	list<CCollider*>				m_OtherColliders;

	_uint							m_iLayer = { 0 };

#ifdef _DEBUG
private:
	PrimitiveBatch<VertexPositionColor>*		m_pBatch = { nullptr };
	BasicEffect*								m_pEffect = { nullptr };
	ID3D11InputLayout*							m_pInputLayout = { nullptr };
#endif


public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END