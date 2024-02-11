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
	TYPE Get_ColliderType() const {
		return m_eType;
	}

	class CBounding* Get_Bounding() {
		return m_pBounding;
	}

public:
	virtual HRESULT Initialize_Prototype(TYPE eType);
	virtual HRESULT Initialize(void* pArg);
public:
	void Update(_fmatrix TransformMatrix);
	_bool Collision(CCollider* pTargetCollider);

#ifdef _DEBUG
public:
	virtual HRESULT Render();
#endif

private:
	TYPE					m_eType = { TYPE_END };
	class CBounding*		m_pBounding = { nullptr };
	_bool					m_isCollision = { false };

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