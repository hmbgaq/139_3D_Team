#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CBounding_OBB final : public CBounding
{
public:
	typedef struct tabOBB : public BOUNDING_DESC
	{
		_float3		vExtents;
		_float3		vRotation;
	}BOUNDING_OBB_DESC;
private:
	CBounding_OBB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);	
	virtual ~CBounding_OBB() = default;

public:
	const BoundingOrientedBox* Get_Bounding() {
		return m_pOBB;
	}
	virtual void Set_matScale(_matrix _matScale) override { m_matScale = _matScale; }
	virtual void Set_Test(_vector _vTest) override { m_vTest = _vTest; }
	virtual void Set_Scale(_float _sizeX, _float _sizeY, _float _sizeZ);
	virtual void Set_Center(_float _sizeX, _float _sizeY, _float _sizeZ);

public:
	HRESULT Initialize(BOUNDING_DESC* pBoundingDesc);
	virtual void Update(_fmatrix TransformMatrix);
#ifdef _DEBUG
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch, _vector vColor) override;
#endif
	
public:
	virtual _bool Collision(class CCollider* pTargetCollider, _bool* pisCollision) override;

private:
	BoundingOrientedBox*			m_pOriginalOBB = { nullptr };
	BoundingOrientedBox*			m_pOBB = { nullptr };



public:
	static CBounding_OBB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, BOUNDING_DESC* pBoundingDesc);
	virtual void Free() override;
};

END