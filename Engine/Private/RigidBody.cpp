#include "..\Public\RigidBody.h"

#include "GameObject.h"
#include "Transform.h"
#include "Character.h"
#include "Navigation.h"

CRigidBody::CRigidBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CRigidBody::CRigidBody(const CRigidBody& rhs)
	: CComponent(rhs)
{
}

void CRigidBody::Set_Transform(CTransform* pOwnerTransform)
{
	m_pOwnerTransform = pOwnerTransform;
}

HRESULT CRigidBody::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRigidBody::Initialize(void* pArg)
{
	return S_OK;
}

void CRigidBody::Tick(_float fTimeDelta)
{
	//if (nullptr != m_pOwnerTransform && true == m_pOwnerTransform->Get_Enable())
	//{
	//	
	//}


}

void CRigidBody::Late_Tick(_float fTimeDelta)
{
	
	_vector vMoveValue;

	vMoveValue = XMVectorSet(0.f, 0.f, 0.f, 0.f);

	for (auto iter = m_Forces.begin(); iter != m_Forces.end();)
	{
		(*iter).fPower -= (m_fDrag + m_fMass) * fTimeDelta;

		//이동 값이 없는 경우
		if ((*iter).fPower < 0.f)
		{
			iter = m_Forces.erase(iter);
		}

		// 이동 값이 아직 있는 경우
		else
		{
			vMoveValue += XMLoadFloat3(&(*iter).vDir) * (*iter).fPower;
			iter++;
		}
	}

	if (true == m_bUseGravity)
	{
		m_pOwnerTransform->Update_GravityAcc(fTimeDelta);

		_float4 vMoveValueFloat4;
		XMStoreFloat4(&vMoveValueFloat4, vMoveValue);

		_float fGravityAcc = m_pOwnerTransform->Get_GravityAcc();
		vMoveValueFloat4.y -= 9.81f * fGravityAcc * fTimeDelta;
		vMoveValue = XMLoadFloat4(&vMoveValueFloat4);

	}


	CCharacter* pCharacter = dynamic_cast<CCharacter*>(m_pOwner);
	CNavigation* pNavigation = nullptr;
	if (nullptr != pCharacter)
	{
		pNavigation = pCharacter->Get_Navigation();
	}

	if (nullptr != m_pOwnerTransform && true == m_pOwnerTransform->Get_Enable())
	{
		m_pOwnerTransform->Move_On_Navigation(vMoveValue, pNavigation);
		//m_pOwnerTransform->Move_On_Navigation_ForSliding(vMoveValue, fTimeDelta, pNavigation);
	}
		

}

void CRigidBody::Add_Force(_vector In_vDir, _float In_fPower)
{
	_float3 vDirFloat3;
	XMStoreFloat3(&vDirFloat3, In_vDir);

	if (fabsf(vDirFloat3.x + vDirFloat3.y + vDirFloat3.z) < DBL_EPSILON)
	{
		return;
	}

	FORCE tForce{ vDirFloat3, In_fPower };

	//m_Forces.clear();
	m_Forces.push_back(tForce);
}

CRigidBody* CRigidBody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRigidBody* pInstance = new CRigidBody(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRigidBody");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CRigidBody::Clone(void* pArg)
{
	CRigidBody* pInstance = new CRigidBody(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CRigidBody");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRigidBody::Free()
{
	__super::Free();

	Safe_Release(m_pOwnerTransform);
}
