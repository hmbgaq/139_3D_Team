#pragma once

#include "Base.h"

BEGIN(Engine)

class CBounding abstract : public CBase
{
public:
	typedef struct
	{
		_float3		vCenter;
		_uint		iLayer;
	}BOUNDING_DESC;
protected:
	CBounding(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding() = default;

public:
	virtual void Update(_fmatrix TransformMatrix) PURE;
	virtual _bool Collision(class CCollider* pTargetCollider, _bool* pisCollision) PURE;

public:
	virtual	void Set_matScale(_matrix matrix) PURE;
	virtual	void Set_Test(_vector _vTest) PURE;
	virtual void Set_Scale(_float _sizeX, _float _sizeY, _float _sizeZ) PURE;
	virtual void Set_Center(_float _sizeX, _float _sizeY, _float _sizeZ) PURE;

#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch, _vector vColor) PURE;
#endif


protected:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	_matrix						m_matScale = {};
	_vector						m_vTest = {};
public:	
	virtual void Free() override;
};

END