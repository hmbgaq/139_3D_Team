#include "Collider.h"
#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"
#include "GameInstance.h"
#include "GameObject.h"

CCollider::CCollider(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CCollider::CCollider(const CCollider & rhs)
	: CComponent(rhs)
	, m_eType(rhs.m_eType)
#ifdef _DEBUG
	, m_pBatch(rhs.m_pBatch)
	, m_pEffect(rhs.m_pEffect)
	, m_pInputLayout(rhs.m_pInputLayout)
#endif // _DEBUG
{
	#ifdef _DEBUG
		Safe_AddRef(m_pInputLayout);
	#endif
}

void CCollider::Set_ColliderSize(_float _sizeX, _float _sizeY, _float _sizeZ)
{
	/* --- 소영 추가 --- */
	/* Sphere = x에만 값채우고 안채워도됨.
	aabb, obb = x y z 모두다 채워야함 
	-> 반지름 기준이라는거 의식하기 */
	m_pBounding->Set_Scale(_sizeX, _sizeY, _sizeZ);
}

void CCollider::Set_Center(_float _sizeX, _float _sizeY, _float _sizeZ)
{
	m_pBounding->Set_Center(_sizeX, _sizeY, _sizeZ);
}

HRESULT CCollider::Initialize_Prototype(TYPE eType)
{
	m_eType = eType;

#ifdef _DEBUG
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
	m_pEffect = new BasicEffect(m_pDevice);
	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderByteCode = { nullptr };
	size_t	iShaderCodeLength = { 0 };

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderCodeLength);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, iShaderCodeLength, &m_pInputLayout)))
		return E_FAIL;

#endif

	return S_OK;
}

HRESULT CCollider::Initialize(void * pArg)
{
	CBounding::BOUNDING_DESC*		pBoundingDesc = (CBounding::BOUNDING_DESC*)pArg;

	m_iLayer = pBoundingDesc->iLayer;

	switch (m_eType)
	{
	case TYPE_SPHERE:
		m_pBounding = CBounding_Sphere::Create(m_pDevice, m_pContext, pBoundingDesc);
		break;
	case TYPE_AABB:
		m_pBounding = CBounding_AABB::Create(m_pDevice, m_pContext, pBoundingDesc);
		break;
	case TYPE_OBB:
		m_pBounding = CBounding_OBB::Create(m_pDevice, m_pContext, pBoundingDesc);
		break;
	}

	m_pGameInstance->Add_Collision(m_iLayer, this);

	return S_OK;
}

void CCollider::Update(_fmatrix TransformMatrix)
{
	m_pBounding->Update(TransformMatrix);
}

_bool CCollider::Is_Collision(CCollider* pTargetCollider)
{
	return m_pBounding->Collision(pTargetCollider, &m_isCollision);
}

void CCollider::Collision(CCollider * pTargetCollider)
{
	m_isCollision = true;

	_bool bIs_SameColl = false;
	for (CCollider* pPreOtherCollider : m_PreOtherColliders)
	{
		if (pPreOtherCollider == pTargetCollider) 
		{
			OnCollisionStay(pTargetCollider);
			bIs_SameColl = true;
			break;
		}
	}

	if (false == bIs_SameColl)
	{
		OnCollisionEnter(pTargetCollider);
	}

	m_OtherColliders.push_back(pTargetCollider);

}

void CCollider::End_CollisionCheck()
{
	if (m_OtherColliders.empty()) 
	{
		m_isCollision = false;
	}

	_bool isErase = false;

	for (auto iter = m_PreOtherColliders.begin(); iter != m_PreOtherColliders.end();)
	{
		isErase = false;

		for (auto& elem : m_OtherColliders)
		{
			if ((*iter) == elem)
			{
				iter = m_PreOtherColliders.erase(iter);
				isErase = true;
				break;
			}
		}

		if (!isErase)
			iter++;

	}

	for (auto& elem : m_PreOtherColliders)
	{
		OnCollisionExit(elem);
	}

	m_PreOtherColliders = m_OtherColliders;
	m_OtherColliders.clear();
}


void CCollider::OnCollisionEnter(CCollider* other)
{
	if (false == m_bEnable || nullptr == m_pOwner || false == m_pOwner->Get_Enable() || true == m_pOwner->Is_Dead())
		return;

	if (true == m_pOwner->Get_Enable() && nullptr != m_pOwner)
		m_pOwner->OnCollisionEnter(other);
}

void CCollider::OnCollisionStay(CCollider* other)
{
	if (false == m_bEnable || nullptr == m_pOwner)
		return;

	if (true == m_pOwner->Get_Enable() && nullptr != m_pOwner)
		m_pOwner->OnCollisionStay(other);
}

void CCollider::OnCollisionExit(CCollider* other)
{
	if (false == m_bEnable || nullptr == m_pOwner)
		return;

	if (true == m_pOwner->Get_Enable() && nullptr != m_pOwner)
		m_pOwner->OnCollisionExit(other);
}

void CCollider::Set_Enable(_bool _Enable)
{
	if (nullptr == this)
		return;


	if (false == m_bEnable && true == _Enable)
	{
		m_pGameInstance->Add_Collision(m_iLayer, this);
	}
	__super::Set_Enable(_Enable);

}

#ifdef _DEBUG

HRESULT CCollider::Render()
{
	if (nullptr == m_pBounding)
		return E_FAIL;
	if (m_bEnable == true)
	{
		m_pContext->GSSetShader(nullptr, nullptr, 0);

		m_pBatch->Begin();

		m_pEffect->SetWorld(XMMatrixIdentity());
		m_pEffect->SetView(m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
		m_pEffect->SetProjection(m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));

		m_pContext->IASetInputLayout(m_pInputLayout);

		m_pEffect->Apply(m_pContext);

		m_pBounding->Render(m_pBatch, m_isCollision == true ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));

		m_pBatch->End();
	}
	

	return S_OK;
}

#endif

CCollider * CCollider::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eType)
{
	CCollider*		pInstance = new CCollider(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype(eType)))
	{
		MSG_BOX("Failed to Created : CCollider");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CCollider::Clone(void* pArg)
{
	CCollider*		pInstance = new CCollider(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCollider");
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CCollider::Free()
{
	__super::Free();

#ifdef _DEBUG

	if(false == m_isCloned)
	{
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pEffect);
	}

	Safe_Release(m_pInputLayout);

#endif

	Safe_Release(m_pBounding);

}
