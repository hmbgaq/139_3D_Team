#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CCollider;

class CBounding_Sphere final : public CBounding
{
public:
	typedef struct tabSphere : public BOUNDING_DESC
	{
		_float		fRadius;
	}BOUNDING_SPHERE_DESC;
public:
	CBounding_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);	
	virtual ~CBounding_Sphere() = default;

public:
	const shared_ptr<BoundingSphere> Get_Bounding() {
		return m_pSphere;
	}

public:
	HRESULT Initialize(BOUNDING_DESC* pBoundingDesc);
#ifdef _DEBUG
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch, _vector vColor) override;
#endif
	virtual void Update(_fmatrix TransformMatrix);

public:
	virtual _bool Collision(shared_ptr<CCollider> pTargetCollider, _bool* pisCollision) override;

private:
	shared_ptr<BoundingSphere>			m_pOriginalSphere = { nullptr };
	shared_ptr<BoundingSphere>			m_pSphere = { nullptr };



public:
	static CBounding_Sphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, BOUNDING_DESC* pBoundingDesc);
	virtual void Free() override;
};

END