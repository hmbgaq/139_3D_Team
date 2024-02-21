#include "..\Public\PhysXController.h"

CPhysXController::CPhysXController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CPhysXController::CPhysXController(const CPhysXController& rhs)
	: CComponent(rhs)
{
}

void CPhysXController::Free()
{
}
