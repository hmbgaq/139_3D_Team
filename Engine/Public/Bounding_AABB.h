#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CBounding_AABB final : public CBounding
{
public:
	typedef struct tagAABB: public BOUNDING_DESC
	{
		_float3		vExtents;
	}BOUNDING_AABB_DESC;
private:
	CBounding_AABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding_AABB() = default;

public:
	BoundingBox* Get_Bounding() {
		return m_pAABB;
	}

	BoundingBox* Get_OriginBounding() {
		return m_pOriginalAABB;
	}


	virtual void Set_matScale(_matrix _matScale) override { m_matScale = _matScale; }
	virtual void Set_Test(_vector _vTest) override { m_vTest = _vTest; }
	virtual void Set_Scale(_float _sizeX, _float _sizeY, _float _sizeZ);
	virtual void Set_Center(_float _sizeX, _float _sizeY, _float _sizeZ);

public:
	HRESULT Initialize(BOUNDING_DESC* pBoundingDesc);
#ifdef _DEBUG
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch, _vector vColor) override;
#endif
	virtual void Update(_fmatrix TransformMatrix);

public:
	virtual _bool Collision(class CCollider* pTargetCollider, _bool* pisCollision) override;

private:
	/* 초기상태를 가지는 데이터 */
	BoundingBox*			m_pOriginalAABB = { nullptr };

	/* 월드로 갱신된 상태를 가지는 데이터 */
	BoundingBox*			m_pAABB = { nullptr };

public:
	static CBounding_AABB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, BOUNDING_DESC* pBoundingDesc);
	virtual void Free() override;
};

END