#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CPhysXCollider final : public CComponent
{

private:
	CPhysXCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPhysXCollider(const CPhysXCollider& rhs);
	virtual ~CPhysXCollider() = default;

private:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg) override;

public:
	_uint	Get_PColliderIndex() const { return m_iColliderIndex; }

//public:
//	CPhysXCollider* CallBack_CollisionEnter;
//	CPhysXCollider* CallBack_CollisionStay;
//	CPhysXCollider* CallBack_CollisionExit;

private:
	_uint m_iColliderIndex = { 0 };

private:
	void Free();
};

END