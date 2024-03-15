#include "Light_Manager.h"
#include "Light.h"

CLight_Manager::CLight_Manager()
{
}

HRESULT CLight_Manager::Initialize()
{
	XMMATRIX identityMatrix = XMMatrixIdentity();
	XMStoreFloat4x4(&Identity_Matrix, identityMatrix);

	return S_OK;
}

HRESULT CLight_Manager::Add_Light(const LIGHT_DESC & LightDesc, _int& outLightIndex)
{
	CLight*		pLight = CLight::Create(LightDesc);
	if (nullptr == pLight)
		return E_FAIL;

	m_Lights.push_back(pLight);

	outLightIndex = pLight->Get_LightIndex();

	return S_OK;
}

_bool CLight_Manager::Remove_Light(const _uint& iIndex)
{
	for (auto iter = m_Lights.begin(); iter != m_Lights.end(); ++iter)
	{
		if (iIndex == (*iter)->Get_LightIndex())
		{
			(*iter)->Decrease_GlobalIndex();
			m_Lights.erase(iter);
			return true;
		}
	}

	return false;
}

_bool CLight_Manager::Remove_AllLight()
{


	return _bool();
}

CLight* CLight_Manager::Find_Light(const _int iIndex)
{
	for (auto iter = m_Lights.begin(); iter != m_Lights.end();)
	{
		if (iIndex == (*iter)->Get_LightDesc().Get_LightIndex())
		{
			return *iter;
		}
		else
		{
			++iter;
		}
	}

	return nullptr;
}

void CLight_Manager::Change_Light_Desc(const _int iIndex, LIGHT_DESC newDesc)
{
	Find_Light(iIndex)->Set_LightDesc(newDesc);
}

HRESULT CLight_Manager::Render(CShader * pShader, CVIBuffer_Rect * pVIBuffer)
{
	for (auto& pLight : m_Lights)
		pLight->Render(pShader, pVIBuffer);	

	return S_OK;
}

HRESULT CLight_Manager::Set_ShadowLight(_uint iLevelIndex, _float4 vEye, _float4 vAt, _float4 vUp)
{
	return S_OK;
}

HRESULT CLight_Manager::Add_ShadowLight_View(_uint iLevelIndex, _vector vEye, _vector vAt, _vector vUp)
{
	/* _float4x4		ViewMatrix, ProjMatrix;

	XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(XMVectorSet(-20.f, 20.f, -20.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), g_iWinSizeX / (float)g_iWinSizeY, 0.1f, m_pGameInstance->Get_CamFar()));

	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix));*/

	auto iter = m_ShadowLight_ViewMatrix.find(iLevelIndex);

	if (iter != m_ShadowLight_ViewMatrix.end())
		return E_FAIL;

	_float4x4 ViewMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(vEye, vAt, vUp));

	m_ShadowLight_ViewMatrix.emplace(iLevelIndex, ViewMatrix);
	m_ShadowLight_Pos.emplace(iLevelIndex, vEye);

	return S_OK;
}

HRESULT CLight_Manager::Add_ShadowLight_Proj(_uint iLevelIndex, _float fFovAngleY, _float fAspectRatio, _float fNearZ, _float fFarZ)
{
	auto iter = m_ShadowLight_ProjMatrix.find(iLevelIndex);

	if (iter != m_ShadowLight_ProjMatrix.end())
		return E_FAIL;

	_float4x4 ProjMatrix;
	XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(fFovAngleY), fAspectRatio, fNearZ, fFarZ));
	//XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), g_iWinSizeX / (float)g_iWinSizeY, 0.1f,LightFar));

	m_ShadowLight_ProjMatrix.emplace(iLevelIndex, ProjMatrix);
	m_ShadowLight_Far.emplace(iLevelIndex, fFarZ);

	return S_OK;
}

_float4x4 CLight_Manager::Get_ShadowLightViewMatrix(_uint iLevelIndex)
{
	auto iter = m_ShadowLight_ViewMatrix.find(iLevelIndex);

	if (iter == m_ShadowLight_ViewMatrix.end())
		return Identity_Matrix;
	else
		return iter->second;
}

_float4x4 CLight_Manager::Get_ShadowLightViewMatrix_Inverse(_uint iLevelIndex)
{
	auto iter = m_ShadowLight_ViewMatrix.find(iLevelIndex);

	if (iter == m_ShadowLight_ViewMatrix.end())
		return Identity_Matrix;

	_float4x4 Result = {};
	XMStoreFloat4x4(&Result, XMMatrixInverse(nullptr, XMLoadFloat4x4(&iter->second)));

	return Result;
}

_float4x4 CLight_Manager::Get_ShadowLightProjMatrix(_uint iLevelIndex)
{
	auto iter = m_ShadowLight_ProjMatrix.find(iLevelIndex);

	if (iter == m_ShadowLight_ProjMatrix.end())
		return Identity_Matrix;
	else
		return iter->second;
}

_float CLight_Manager::Get_ShadowLightFar(_uint iLevelIndex)
{
	auto iter = m_ShadowLight_Far.find(iLevelIndex);

	if (iter == m_ShadowLight_Far.end())
		return 0.f;
	else
		return iter->second;
}

_float4 CLight_Manager::Get_ShadowLightPos(_uint iLevelIndex)
{
	auto iter = m_ShadowLight_Pos.find(iLevelIndex);

	if (iter == m_ShadowLight_Pos.end())
		return _float4();
	else
		return iter->second;
}

CLight_Manager * CLight_Manager::Create()
{
	CLight_Manager*		pInstance = new CLight_Manager();

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLight_Manager");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLight_Manager::Free()
{
	for (auto& pLight : m_Lights)
		Safe_Release(pLight);

	m_Lights.clear();
}
