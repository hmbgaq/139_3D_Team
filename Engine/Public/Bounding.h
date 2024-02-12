#pragma once

#include "Base.h"

BEGIN(Engine)

class CCollider;

class CBounding abstract : public CBase
{
public:
	typedef struct
	{
		_float3		vCenter;
	}BOUNDING_DESC;
public:
	CBounding(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding() = default;

public:
	virtual void Update(_fmatrix TransformMatrix) = 0;
	virtual _bool Collision(shared_ptr<CCollider> pTargetCollider, _bool* pisCollision) = 0;

#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch, _vector vColor) = 0;
#endif


protected:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };


public:	
	virtual void Free() override;
};

END