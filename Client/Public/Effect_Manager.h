#pragma once
#include "Client_Defines.h"
#include "Engine_Macro.h"

#include "Base.h"
#include "GameInstance.h"


BEGIN(Client)
class CEffect;
class CEffect_Trail;

//! EFFECT_MANAGER 매크로 : GetInstance()안해도 됨. (EFFECT_MANAGER->Create_Effect(~~);)

//! 뭘 써야할지 모르겠다!! -> 일단 Create_Effect(string strFileName, CGameObject* pOwner = nullptr); 호출
//! 이펙트가 어떤 객체를 따라 움직여야한다! pOwner에 해당 객체 넣어주기(파트오브젝트 무기처럼 따라다님), bParentPivot이 true인지 확인

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

	//! 유정 : strFileName은 json파일 이름. ("../Bin/DataFiles/Data_Effect/" 경로에 저장해 놓은 json파일 중 원하는거 골라쓰기! (.json 붙어야함))
	CEffect* Create_Effect(_uint iLevelIndex, const wstring& strLayerTag, string strFileName, CGameObject* pOwner = nullptr);

	//! 명빈 : 편하게 쓰려고 기존 함수를 활용해 간소화 시킴
	CEffect* Create_Effect(string strFileName, CGameObject* pOwner = nullptr);

	// 더 깊은 경로에 있는 이펙트 데이터 파일 사용하고싶을 때 호출 : strAddPath는 "/" 제외 (Data_Effect/Zapper_Dash/Zapper_Dash_10.json)
	CEffect* Create_Effect(string strAddPath, string strFileName, CGameObject* pOwner = nullptr);


	// 특정 위치에 이펙트를 생성하고 싶은 경우 호출(vPos에 생성시킬 위치)
	CEffect* Create_Effect_Pos(string strFileName, _float3 vPos, CGameObject* pOwner = nullptr);
	CEffect* Create_Effect_Pos(string strAddPath, string strFileName, _float3 vPos, CGameObject* pOwner = nullptr);

	// 트레일이 달린 이펙트를 생성할 때 사용(이펙트jsno파일, 트레일json파일, 이펙트가 따라움직여야할 객체가 있는 경우 오너 넣기)
	CEffect* Create_Effect_With_Trail(string strEffectFileName, string strTrailFileName, CGameObject* pOwner = nullptr);

	// 두두두두 이펙트 지나간 자리에 이펙트 생성(누적시간 변수, 생성시간 텀, 타임델타, 생성할 이펙트 파일.json, 생성할 위치, 바라볼 타겟이 있는지, 타겟의 위치)
	HRESULT Tick_Create_Effect(_float* fTimeAcc, _float fCreateTime, _float fTimeDelta, string strEffectFileName
		, _float3 vPos = _float3(0.f, 0.f, 0.f)
		, _bool bLookTarget = FALSE, _float4 vTargetPos = _float4(0.f, 0.f, 0.f, 0.f));



public:
	// Trail ========================================================================================================================================

	// 트레일 생성(준비)
	CEffect_Trail* Ready_Trail(_uint iLevelIndex, const wstring& strLayerTag, string strFileName, CGameObject* pOwner = nullptr);

	// 트레일 생성(준비) : 간소화
	CEffect_Trail* Ready_Trail(string strFileName, CGameObject* pOwner = nullptr);


private:
	class CGameInstance*	m_pGameInstance = { nullptr };
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };


public:
	virtual void	Free();
};

END

