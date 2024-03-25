#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END

BEGIN(Client)

class CEnvironment_Interact final : public CGameObject
{
public:
	enum INTERACT_TYPE { INTERACT_JUMP100, INTERACT_JUMP200, INTERACT_JUMP300, INTERACT_VAULT100, INTERACT_VAULT200, INTERACT_WAGONJUMP, INTERACT_WAGONEVENT, INTERACT_END };
	enum INTERACT_STATE { INTERACTSTATE_LOOP, INTERACTSTATE_ONCE, INTERACTSTATE_END };

public:
	typedef struct tagEnvironmentInteractObjectDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_float4			vPos = { 0.f, 0.f, 0.f, 0.f };
		_uint			iShaderPassIndex = { 0 };
		wstring			strModelTag;
		_float4x4		WorldMatrix;

		INTERACT_TYPE	eInteractType = INTERACT_END;
		INTERACT_STATE  eInteractState = INTERACTSTATE_END;
		_float3			vColliderSize = { 1.f, 1.f, 1.f};
		_float3			vColliderCenter = { 0.f, 1.f, 0.f };
		_float3			vPlayerRootMoveRate = { 1.f, 1.f, 1.f};
		_int			iPlayAnimationIndex = { 0 };

		_bool			bAnimModel = { false };
		_bool			bPreview = true; //! 미리보기용 오브젝트인지 확인
		_bool			bUseGravity = true;

		_bool			bLevelChange = false;
		LEVEL			eChangeLevel = LEVEL_INTRO_BOSS;
		string			strSplineJsonPath = "";
	}ENVIRONMENT_INTERACTOBJECT_DESC;

private:
	CEnvironment_Interact(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CEnvironment_Interact(const CEnvironment_Interact& rhs);
	virtual ~CEnvironment_Interact() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void		Priority_Tick(_float fTimeDelta) override;
	virtual void		Tick(_float fTimeDelta) override;
	virtual void		Late_Tick(_float fTimeDelta) override;
	virtual HRESULT		Render() override;
	virtual HRESULT		Render_Shadow() override;

public:
	virtual _bool		Write_Json(json& Out_Json) override;
	virtual void		Load_FromJson(const json& In_Json) override;



public:
	ENVIRONMENT_INTERACTOBJECT_DESC*	Get_EnvironmentDesc() { return &m_tEnvironmentDesc; }
	wstring&							Get_ModelTag() { return m_tEnvironmentDesc.strModelTag; }
	_bool								Is_AnimModel() { return m_tEnvironmentDesc.bAnimModel; }
	
#ifdef _DEBUG
	void								Set_ColliderSize(_float3 vColliderSize);
	void								Set_ColliderCenter(_float3 vColliderCenter);
	void								Set_InteractType(INTERACT_TYPE eInteractType) { m_tEnvironmentDesc.eInteractType = eInteractType; }
	void								Set_InteractState(INTERACT_STATE eInteractState) { m_tEnvironmentDesc.eInteractState = eInteractState; }
	void								Set_PlayerRootMoveRate(_float3 vRootMoveRate) { m_tEnvironmentDesc.vPlayerRootMoveRate = vRootMoveRate;}
	void								Set_UseGravity(_bool bUseGravity) { m_tEnvironmentDesc.bUseGravity = bUseGravity; }
	
	void								Set_LevelChangeType(_bool bLevelChange, LEVEL eLevel);
#endif // _DEBUG
	void								Set_ShaderPassIndex(_int iShaderPassIndex) { m_tEnvironmentDesc.iShaderPassIndex = iShaderPassIndex; }
	void								Set_SplineJsonPath(string strJsonPath) { m_tEnvironmentDesc.strSplineJsonPath = strJsonPath;}
	void								Set_SplineDivergingCount(_int iDivergingCount) { m_iDivergingCount = iDivergingCount;} 

#ifdef _DEBUG
public: //! For.Tool
	virtual _bool						Picking(_Out_ _float3* vPickedPos) override;

#endif 

public:
	void								Start_Environment_Animation() { m_bPlay = true; }

public:
	void								Interact();

public:
	void								Start_SplineEvent();
	void								Reset_TestEvent();

	void								Start_Spline(vector<_float4>* SplinePoints);
	void								Reset_StartMatrix();
	void								Spline_Loop(const _float fTimeDelta);
	void								Spline_Clear();
	
	void								Set_SplineSpeed(_float fSpeed) { m_fSplineSpeed = fSpeed; m_pTransformCom->Set_Speed(m_fSplineSpeed);}
	void								Set_ArrivalTime(_float fArriavalTime) { m_fArrivalTime = fArriavalTime;}

	void								Set_SplineCheck(_int iIndex, _bool bCheck) { m_vecPointChecks[iIndex] = bCheck; }

private:
	HRESULT								Load_SplineJson();
	HRESULT								Init_WagonEvent();
	void								Change_WagonTrack(const _float fTimeDelta);
	

private:
	CShader*							m_pShaderCom = { nullptr };	
	CModel*								m_pModelCom = { nullptr };
	CCollider*							m_pColliderCom = { nullptr };

	_int								m_iCurrentLevelIndex = -1;

private:
	ENVIRONMENT_INTERACTOBJECT_DESC		m_tEnvironmentDesc = {};
	_bool								m_bPlay = false;
	
	_bool								m_bInteract = false;
	
	_bool								m_bFindPlayer = false;

	_bool								m_bSpline = false;
	_float4x4							m_InitMatrix;

	
	vector<_float4>						m_vecSplinePoints;
	map<wstring, vector<_float4>>		m_mapSplineVectors;
	
	map<wstring, _float>					m_mapSplineSpeeds;

	wstring								m_strCurrentSplineTrack = L"";
	_int								m_iCurrentTrackIndex = 0;


	vector<_bool>						m_vecPointChecks;
	_float								m_fSplineTimeAcc = 0.f;
	_float								m_fSplineEndRaidus = 1.f;
	_int								m_iCurrentSplineIndex = 0;
	_int								m_iDivergingCount = 0;
	_bool								m_bPlaySpline = false;
	_float								m_fSplineSpeed = 1.f;
	_float								m_fArrivalTime = 3.f; // ! 스플라인에 마지막 점까지 도착하는데 걸리는 시간.

private:
	CPlayer*						    m_pPlayer = { nullptr };

private:
	HRESULT						Ready_Components();
	HRESULT						Ready_InteractCollider(INTERACT_TYPE eInteractType);
	HRESULT						Bind_ShaderResources();


public:
	/* 원형객체를 생성한다. */
	static CEnvironment_Interact* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;

	virtual void Free() override;

};

END