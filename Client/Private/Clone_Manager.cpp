#include "stdafx.h"
#include "Clone_Manager.h"

#include "GameInstance.h"

#include "Effect.h"

IMPLEMENT_SINGLETON(CClone_Manager);

CClone_Manager::CClone_Manager()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);


}

HRESULT CClone_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	if (nullptr == pDevice || nullptr == pContext)
		return E_FAIL;

	m_pDevice = pDevice;
	Safe_AddRef(m_pDevice);

	m_pContext = pContext;
	Safe_AddRef(m_pContext);

	return S_OK;
}


CEffect* CClone_Manager::Create_Effect(_uint iLevelIndex, const wstring& strLayerTag, string strFileName)
{
	CEffect::EFFECT_DESC	tEffectDesc = {};
	CEffect* pEffect = dynamic_cast<CEffect*>(m_pGameInstance->Add_CloneObject_And_Get(iLevelIndex, strLayerTag, TEXT("Prototype_GameObject_Effect"), &tEffectDesc));

	json In_Json;

	string strPath = "../Bin/DataFiles/Data_Effect/";

	string strLoadPath = strPath + "\\" + strFileName;
	CJson_Utility::Load_Json(strLoadPath.c_str(), In_Json);

	pEffect->Load_FromJson(In_Json);

	return	pEffect;
}

void CClone_Manager::Free()
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
