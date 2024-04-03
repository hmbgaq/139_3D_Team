	#include "stdafx.h"
#include "Effect_Manager.h"

#include "GameInstance.h"

#include "Effect.h"
#include "Effect_Void.h"
#include "Effect_Trail.h"

IMPLEMENT_SINGLETON(CEffect_Manager);

CEffect_Manager::CEffect_Manager()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CEffect_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	if (nullptr == pDevice || nullptr == pContext)
		return E_FAIL;

	m_pDevice = pDevice;
	Safe_AddRef(m_pDevice);

	m_pContext = pContext;
	Safe_AddRef(m_pContext);

	return S_OK;
}


CEffect* CEffect_Manager::Create_Effect(_uint iLevelIndex, const wstring& strLayerTag, string strFileName, CGameObject* pOwner)
{
	CEffect::EFFECT_DESC	tEffectDesc = {};
	CEffect* pEffect = dynamic_cast<CEffect*>(m_pGameInstance->Add_CloneObject_And_Get(iLevelIndex, strLayerTag, TEXT("Prototype_GameObject_Effect"), &tEffectDesc));

	string strPath = "../Bin/DataFiles/Data_Effect";
	string strLoadPath = strPath + "/" + strFileName;

	json In_Json;
	CJson_Utility::Load_Json(strLoadPath.c_str(), In_Json);

	if (nullptr != pOwner)
		pEffect->Set_Object_Owner(pOwner);	// 부모 설정 (부모가 있고, 이펙트의 bParentPivot이 True이면 오너객체를 따라다님)

	pEffect->Load_FromJson(In_Json);
	
	return	pEffect;

	/* 사용 예시 */
	//CEffect* pEffect = EFFECT_MANAGER->Create_Effect(LEVEL_TOOL, LAYER_EFFECT, "Test_Effect.json");
}

CEffect* CEffect_Manager::Create_Effect(_uint iLevelIndex, string strAddPath, string strFileName, CGameObject* pOwner)
{
	CEffect::EFFECT_DESC	tEffectDesc = {};
	CEffect* pEffect = dynamic_cast<CEffect*>(m_pGameInstance->Add_CloneObject_And_Get(iLevelIndex, LAYER_EFFECT, TEXT("Prototype_GameObject_Effect"), &tEffectDesc));

	string strPath = "../Bin/DataFiles/Data_Effect";
	string strLoadPath = strPath + "/" + strAddPath + "/" + strFileName;

	json In_Json;
	CJson_Utility::Load_Json(strLoadPath.c_str(), In_Json);

	if (nullptr != pOwner)
		pEffect->Set_Object_Owner(pOwner);	// 부모 설정 (부모가 있고, 이펙트의 bParentPivot이 True이면 오너객체를 따라다님)

	pEffect->Load_FromJson(In_Json);

	return	pEffect;
}


CEffect* CEffect_Manager::Create_Effect(string strFileName, CGameObject* pOwner)
{
	return Create_Effect(m_pGameInstance->Get_NextLevel(), LAYER_EFFECT, strFileName, pOwner);
}


CEffect* CEffect_Manager::Create_Effect(string strAddPath, string strFileName, CGameObject* pOwner, _bool bUseSocket, string strBoneTag)
{

	CEffect::EFFECT_DESC	tEffectDesc = {};
	CEffect* pEffect = dynamic_cast<CEffect*>(m_pGameInstance->Add_CloneObject_And_Get(m_pGameInstance->Get_NextLevel(), LAYER_EFFECT, TEXT("Prototype_GameObject_Effect"), &tEffectDesc));

	string strPath = "../Bin/DataFiles/Data_Effect";
	string strLoadPath = strPath + "/" + strAddPath + strFileName;

	json In_Json;
	CJson_Utility::Load_Json(strLoadPath.c_str(), In_Json);

	if (nullptr != pOwner)
		pEffect->Set_Object_Owner(pOwner);	// 부모 설정 (부모가 있고, 이펙트의 bParentPivot이 True이면 오너객체를 따라다님)


	pEffect->Load_FromJson(In_Json);


	if (bUseSocket)
	{
		pEffect->Get_Desc()->bUseSocket = bUseSocket;
		pEffect->Get_Desc()->strBoneTag = strBoneTag;
	}
		

	return	pEffect;
}

CEffect* CEffect_Manager::Create_Effect(string strAddPath, string strFileName, _float3 vPos
										, _bool bLookTarget, _float3 vTargetPos)
{
	CEffect::EFFECT_DESC	tEffectDesc = {};
	CEffect* pEffect = dynamic_cast<CEffect*>(m_pGameInstance->Add_CloneObject_And_Get(m_pGameInstance->Get_NextLevel(), LAYER_EFFECT, TEXT("Prototype_GameObject_Effect"), &tEffectDesc));

	string strPath = "../Bin/DataFiles/Data_Effect";
	string strLoadPath = strPath + "/" + strAddPath + strFileName;

	json In_Json;
	CJson_Utility::Load_Json(strLoadPath.c_str(), In_Json);


	pEffect->Load_FromJson(In_Json);


	// 위치 설정
	pEffect->Set_Position(vPos);

	if (TRUE == bLookTarget)
	{
		// 타겟을 바라볼거면 Look_At 해주기
		_float4 vTargetPos_float4 = _float4(vTargetPos.x, vTargetPos.y, vTargetPos.z, 1.f);
		pEffect->Get_Transform()->Look_At(vTargetPos_float4);
	}

	return	pEffect;
}


HRESULT CEffect_Manager::Tick_Create_Effect(_float* fTimeAcc, _float fCreateTime, _float fTimeDelta, string strAddPath, string strEffectFileName
	, _float3 vPos
	, _bool bLookTarget, _float4 vTargetPos )
{

	*fTimeAcc += fTimeDelta; // 시간 누적

	if (*fTimeAcc >= fCreateTime) // 누적 시간이 생성 시간보다 커지면 이펙트 생성 & 누적 시간 초기화
	{
		*fTimeAcc = 0.f;

		// 현재 레벨에 생성
		CEffect* pEffect = Create_Effect(m_pGameInstance->Get_CurrentLevel(), strAddPath, strEffectFileName);

		CTransform* pTransform = pEffect->Get_Transform();

		pEffect->Set_Position(vPos); // 위치 세팅

		if (TRUE == bLookTarget)
		{
			// 타겟을 바라볼거면 look_At 해주기
			pTransform->Look_At(vTargetPos);
		}

	}

	return S_OK;
}


CEffect* CEffect_Manager::Create_Effect_With_Trail(string strEffectFileName, string strTrailFileName, CGameObject* pOwner)
{
	_uint iCurLevel = m_pGameInstance->Get_NextLevel();

	CEffect* pEffect = Create_Effect(iCurLevel, LAYER_EFFECT, strEffectFileName, pOwner);

	pEffect->Ready_Trail(iCurLevel, strTrailFileName);

	return pEffect;
}


CEffect_Trail* CEffect_Manager::Ready_Trail(_uint iLevelIndex, const wstring& strLayerTag, string strFileName, CGameObject* pOwner)
{
	CEffect_Void::EFFECTVOID_DESC	tVoidDesc = {};

	tVoidDesc.eType_Effect = { CEffect_Void::TRAIL };

	tVoidDesc.strTextureTag[CEffect_Void::TEXTURE_DIFFUSE] = TEXT("Prototype_Component_Texture_Effect_Diffuse");
	tVoidDesc.iTextureIndex[CEffect_Void::TEXTURE_DIFFUSE] = { 0 };

	tVoidDesc.strTextureTag[CEffect_Void::TEXTURE_MASK] = TEXT("Prototype_Component_Texture_Effect_Mask");
	tVoidDesc.iTextureIndex[CEffect_Void::TEXTURE_MASK] = { 0 };

	tVoidDesc.strTextureTag[CEffect_Void::TEXTURE_NOISE] = TEXT("Prototype_Component_Texture_Effect_Noise");
	tVoidDesc.iTextureIndex[CEffect_Void::TEXTURE_NOISE] = { 0 };

	CEffect_Trail* pTrail = dynamic_cast<CEffect_Trail*>(m_pGameInstance->Add_CloneObject_And_Get(iLevelIndex, strLayerTag, TEXT("Prototype_GameObject_Effect_Trail"), &tVoidDesc));

	string strPath = "../Bin/DataFiles/Data_Effect/Data_Trail";
	string strLoadPath = strPath + "/" + strFileName;

	json In_Json;
	CJson_Utility::Load_Json(strLoadPath.c_str(), In_Json);

	if (nullptr != pOwner)
		pTrail->Set_Object_Owner(pOwner);

	pTrail->Load_FromJson(In_Json);

	return pTrail;


	/* 사용 예시 */
	// m_pTrail = EFFECT_MANAGER->Ready_Trail(LEVEL_TOOL, LAYER_EFFECT, "Test_Trail.json"); // 또는 Part_Preview클래스 참고
}


CEffect_Trail* CEffect_Manager::Ready_Trail(string strFileName, CGameObject* pOwner)
{
	return Ready_Trail(m_pGameInstance->Get_NextLevel(), LAYER_EFFECT, strFileName, pOwner);
}

void CEffect_Manager::Free()
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
