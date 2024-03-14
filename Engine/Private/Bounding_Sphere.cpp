#include "..\Public\Bounding_Sphere.h"
#include "DebugDraw.h"

#include "Collider.h"

CBounding_Sphere::CBounding_Sphere(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CBounding(pDevice, pContext)
{

}

void CBounding_Sphere::Set_Scale(_float _sizeX, _float _sizeY, _float _sizeZ)
{
	m_pSphere->Radius = _sizeX;
}

HRESULT CBounding_Sphere::Initialize(BOUNDING_DESC * pBoundingDesc)
{
	BOUNDING_SPHERE_DESC* pDesc = (BOUNDING_SPHERE_DESC*)pBoundingDesc;

	m_pOriginalSphere = new BoundingSphere(pDesc->vCenter, pDesc->fRadius);
	m_pSphere = new BoundingSphere(*m_pOriginalSphere);
	m_matScale = XMMatrixIdentity();
	m_vTest = XMVector3Normalize(m_vTest);
	return S_OK;
}


void CBounding_Sphere::Update(_fmatrix TransformMatrix)
{
	m_pOriginalSphere->Transform(*m_pSphere , m_matScale * TransformMatrix);
}

_bool CBounding_Sphere::Collision(CCollider * pTargetCollider, _bool * pisCollision)
{
	CBounding*	pTargetBounding = pTargetCollider->Get_Bounding();

	*pisCollision = false;

	switch (pTargetCollider->Get_ColliderType())
	{
	case CCollider::TYPE_AABB:
		*pisCollision = m_pSphere->Intersects(*((CBounding_AABB*)pTargetBounding)->Get_Bounding());
		break;
	case CCollider::TYPE_OBB:
		*pisCollision = m_pSphere->Intersects(*((CBounding_OBB*)pTargetBounding)->Get_Bounding());
		break;
	case CCollider::TYPE_SPHERE:
		*pisCollision = m_pSphere->Intersects(*((CBounding_Sphere*)pTargetBounding)->Get_Bounding());
		break;
	}
	return *pisCollision;
}

#ifdef _DEBUG

HRESULT CBounding_Sphere::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _vector vColor)
{
	if (nullptr == pBatch ||
		nullptr == m_pSphere)
		return E_FAIL;

	/* m_pAABB가 월드스페이스 상의 정보다. */
	DX::Draw(pBatch, *m_pSphere, vColor);

	return S_OK;
}

#endif

CBounding_Sphere * CBounding_Sphere::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, BOUNDING_DESC* pBoundingDesc)
{
	CBounding_Sphere*		pInstance = new CBounding_Sphere(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pBoundingDesc)))
	{
		MSG_BOX("Failed to Created : CBounding_Sphere");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBounding_Sphere::Free()
{
	__super::Free();
	Safe_Delete(m_pOriginalSphere);
	Safe_Delete(m_pSphere);

}
