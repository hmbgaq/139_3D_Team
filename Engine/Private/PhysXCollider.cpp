#include "..\Public\PhysXCollider.h"

CPhysXCollider::CPhysXCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CPhysXCollider::CPhysXCollider(const CPhysXCollider& rhs)
	: CComponent(rhs)
{
}

HRESULT CPhysXCollider::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPhysXCollider::Initialize(void* pArg)
{
	return S_OK;
}

void CPhysXCollider::Free()
{
}
