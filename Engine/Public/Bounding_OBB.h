#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CCollider;

class CBounding_OBB final : public CBounding
{
public:
	typedef struct tabOBB : public BOUNDING_DESC
	{
		_float3		vExtents;
		_float3		vRotation;
	}BOUNDING_OBB_DESC;
public:
	CBounding_OBB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);	
	virtual ~CBounding_OBB() = default;

public:
	const shared_ptr<BoundingOrientedBox> Get_Bounding() {
		return m_pOBB;
	}

public:
	HRESULT Initialize(BOUNDING_DESC* pBoundingDesc);
	virtual void Update(_fmatrix TransformMatrix);
#ifdef _DEBUG
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch, _vector vColor) override;
#endif
	
public:
	virtual _bool Collision(shared_ptr<CCollider> pTargetCollider, _bool* pisCollision) override;

private:
	shared_ptr<BoundingOrientedBox>			m_pOriginalOBB = { nullptr };
	shared_ptr<BoundingOrientedBox>			m_pOBB = { nullptr };



public:
	static CBounding_OBB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, BOUNDING_DESC* pBoundingDesc);
	virtual void Free() override;
};

END