#pragma once
#include "Client_Defines.h"
#include "Engine_Macro.h"

#include "Base.h"
#include "GameInstance.h"


BEGIN(Client)
class CEffect;
class CEffect_Trail;

//! EFFECT_MANAGER 매크로 : GetInstance()안해도 됨. (EFFECT_MANAGER->Create_Effect(~~);)
//! 이펙트는 스태틱 레벨

class CEffect_Manager : public CBase
{
	DECLARE_SINGLETON(CEffect_Manager);

private:
	CEffect_Manager(); 
	virtual ~CEffect_Manager() = default;

public:
	HRESULT		Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:	
	// Effect ======================================================================================================================================
	CEffect* Play_Effect(string strAddPath, string strFileName, CGameObject* pOwner = nullptr, _bool bUseSocket = false, string strBoneTag = "");
	CEffect* Play_Effect(string strAddPath, string strFileName, CGameObject* pOwner, _float3 vPos, _bool bLookTarget = false, _float3 vTargetPos = _float3(0.f, 0.f, 0.f));
	CEffect* Play_Effect_StaticPivot(string strAddPath, string strFileName, CGameObject* pOwner, _float4x4 matPivot);


	// 두두두두 이펙트 지나간 자리에 이펙트 생성(누적시간 변수, 생성시간 텀, 타임델타, 생성할 이펙트 파일.json, 생성할 위치, 바라볼 타겟이 있는지, 타겟의 위치)
	HRESULT Generate_Effect(_float* fTimeAcc, _float fGenerateTimeTerm, _float fTimeDelta, string strAddPath, string strFileName
		                   , _float3 vPos
						   , _bool bLookTarget = false, _float3 vTargetPos = _float3(0.f, 0.f, 0.f)
							, _bool bScaleLerp = false, _float3* vScaleAcc = nullptr);


	HRESULT Generate_Effect_AttachBone(_float* fTimeAcc, _float fGenerateTimeTerm, _float fTimeDelta, string strAddPath, string strFileName
										, CGameObject* pOwner = nullptr, _bool bUseSocket = false, string strBoneTag = "");

public:
	// Trail ========================================================================================================================================

	// 트레일 생성(준비)
	CEffect_Trail* Ready_Trail(_uint iLevelIndex, const wstring& strLayerTag, string strFileName, CGameObject* pOwner = nullptr);

	// 트레일 생성(준비) : 간소화
	CEffect_Trail* Ready_Trail(string strFileName, CGameObject* pOwner = nullptr);



public:
	// Pool ===========================================================================================================================================
	HRESULT				Ready_EffectPool();

	HRESULT				Add_ToPool(_uint iLevelIndex, string strAddPath, string strFileName, _uint iCount = 1, _bool bHasTrail = false, string strTrailFileName = "");
	HRESULT				Return_ToPool(CEffect* pEffect);


private:
	HRESULT				Clear_EffectPool();
	queue<CEffect*>*	Get_EffectPool(string strFileName);

private:
	// strAddPath, strFileName ( "Player/Zapper_Dash/", "Zapper_Dash_26.json" )
	CEffect*			Load_Effect(_uint iLevelIndex, string strAddPath, string strFileName, _bool bHasTrail = false, string strTrailFileName = "");


//#ifdef _DEBUG
	CEffect*			Create_Effect_ForDebug(string strAddPath, string strFileName, CGameObject* pOwner = nullptr, _bool bUseSocket = false, string strBoneTag = "");
	CEffect*			Create_Effect_ForDebug(string strAddPath, string strFileName, _float3 vPos, _bool bLookTarget = false, _float3 vTargetPos = _float3(0.f, 0.f, 0.f));
	CEffect*			Create_Effect_ForDebug_StaticPivot(string strAddPath, string strFileName, CGameObject* pOwner, _float4x4 matPivot);
//#endif // _DEBUG



private:
	static const _uint iMaxManyEffect			= { 200 };
	static const _uint iMaxEffect				= { 50 };
	static const _uint iMaxFewEffect			= { 5 };
	unordered_map<string, queue<CEffect*>>		m_EffectPool;


private:
	class CGameInstance*	m_pGameInstance = { nullptr };
	ID3D11Device*			m_pDevice		= { nullptr };
	ID3D11DeviceContext*	m_pContext		= { nullptr };


public:
	virtual void	Free();
};

END

