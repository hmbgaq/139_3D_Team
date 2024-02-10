#include "..\Public\Light_Manager.h"
#include "Light.h"

CLight_Manager::CLight_Manager()
{
}

HRESULT CLight_Manager::Initialize()
{
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
	for (auto iter = m_Lights.begin(); iter != m_Lights.end();)
	{
		if (iIndex == (*iter)->Get_LightDesc().Get_LightIndex())
		{
			m_Lights.erase(iter);
			return true;
		}
		else
		{
			++iter;
		}
	}

	return false;
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
HRESULT CLight_Manager::Render(CShader * pShader, CVIBuffer_Rect * pVIBuffer)
{
	for (auto& pLight : m_Lights)
		pLight->Render(pShader, pVIBuffer);	

	return S_OK;
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
