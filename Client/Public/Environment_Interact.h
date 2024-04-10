#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
class CNavigation;
class CRigidBody;
END


BEGIN(Client)
// !Add Interaction UI
class CUI_Manager;
class CUI;
class CUI_Interaction;


class CEnvironment_Interact final : public CGameObject
{
public:
	enum INTERACT_TYPE { 
							INTERACT_JUMP100, 
							INTERACT_JUMP200, 
							INTERACT_JUMP300, 
							INTERACT_VAULT100, 
							INTERACT_VAULT200, 
							INTERACT_WAGONPUSH, 
							INTERACT_WAGONJUMP, 
							INTERACT_WAGONEVENT, 
							INTEARCT_WAGONROPEJUMP, 
							INTERACT_CLIMB100, 
							INTERACT_CLIMB200, 
							INTERACT_CLIMB300, 
							INTERACT_CLIMB450,
							INTERACT_SLIDE,
							INTERACT_LEVER,
							INTERACT_PLANK,
							INTERACT_ROPECLIMB,
							INTERACT_ROPEDOWN,
							INTERACT_DOOROPEN,
							INTERACT_LADDERUP,
							INTERACT_WHIPSWING,
							INTERACT_WHIPPULL,
							INTERACT_ROTATIONVALVE,
							INTERACT_NONE,
							INTERACT_END 
					    };
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

		_float3			vBodyColliderSize = { 1.f, 1.f, 1.f };
		_float3			vBodyColliderCenter = { 0.f, 1.f, 0.f };

		_float3			vInteractColliderSize = { 1.f, 1.f, 1.f};
		_float3			vInteractColliderCenter = { 0.f, 1.f, 0.f };
		_float3			vPlayerRootMoveRate = { 1.f, 1.f, 1.f};
		_int			iPlayAnimationIndex = { 0 };

		_bool			bAnimModel = { false };
		_bool			bPreview = true; //! 미리보기용 오브젝트인지 확인
		_bool			bUseGravity = true; //! 상호작용시 플레이어가 중력의 영향을 받을 것인지

		_bool			bLevelChange = false; //! 상호작용시 레벨을 체인지 할 것인지.
		LEVEL			eChangeLevel = LEVEL_INTRO_BOSS;
		string			strSplineJsonPath = "";

		_int			iInteractGroupIndex = -1; //! 특정 상호작용 오브젝트가 활성화될시 다른 상호작용 오브젝트도 활성시키기 위한 그룹핑인덱스
		_float4			vEnablePosition = {}; //!  특정 상호작용 오브젝트가 다른 상호작용 오브젝트를 활성화시키기 위한 위치 조건을 위한 위치벡터
		string			strEnableJsonPath = "";

		_float3			vMoveRangeColliderSize = { 1.f, 1.f, 1.f };
		_float3			vMoveRangeColliderCenter = { 0.f, 1.f, 0.f };

		_float3			vInteractMoveColliderSize = { 1.f, 1.f, 1.f};
		_float3			vInteractMoveColliderCenter = { 0.f, 1.f, 0.f};

		_float4			vArrivalPosition = {}; //! 특정 상호작용 오브젝트가 위치벡터에 도달하면 종료시키기위한 위치벡터
		_float4			vOffset = {}; //!  특정 상호작용 오브젝트를 기준으로 위치해야하기 위한 오프셋
		_float			fRotationAngle = 0.f; //! 특정 상호작용 오브젝트가 활성화될시 회전해야할 각도
		_float			fRotationSpeed = 0.f; //! 회전해야하는 오브젝트인경우 회전 속도를 저장시키기위함.
		ROTATION_LERP_STATE eRotationState = LERP_Y; //! 회전할 방향

		_bool			bOffset = false; //! 오프셋 위치가 적용시켜져야하는 지.
		_bool			bRotate = false; //! 활성화시 회전해야할지
		
		_bool			bOwner = false; //! 오너임을 알기위함.
		_bool			bRootTranslate = false; //! 플레이어의 루트모션에 따라 이동되여야하는지
		_bool			bArrival = false; //! 특정 지점에 가야하는 지
		_bool			bMoveCollider = false;
		_bool			bEnable = false; //! 활성화 시킬 위치가 있는 상호작용일 경우
		_bool			bInteractMoveMode = false; //! 플레이어를 강제로 이동시켜야할 경우

		vector<_int>	vecUpdateCellIndex; //! 활성화, 비활성화 시킬 셀들의 인덱스
		

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
	virtual HRESULT		Render_OutLine() override;
	virtual HRESULT		Render_CSM(_uint i) override;

public:
	virtual _bool		Write_Json(json& Out_Json) override;
	virtual void		Load_FromJson(const json& In_Json) override;

public:
	virtual void		OnCollisionEnter(CCollider* other) override;
	virtual void		OnCollisionStay(CCollider* other) override;
	virtual void		OnCollisionExit(CCollider* other) override;

private:
	void				Collision_Push_ForPlayer(CCollider* other);
	void				Collision_Push_ForOtherInteract(CCollider* other);


public:
	ENVIRONMENT_INTERACTOBJECT_DESC*	Get_EnvironmentDesc() { return &m_tEnvironmentDesc; }
	wstring&							Get_ModelTag() { return m_tEnvironmentDesc.strModelTag; }
	string&								Get_StringModelTag() { return m_pGameInstance->Convert_WString_To_String(m_tEnvironmentDesc.strModelTag);}
	_bool								Is_AnimModel() { return m_tEnvironmentDesc.bAnimModel; }
	
#ifdef _DEBUG
	void								Set_InteractColliderSize(_float3 vInteractColliderSize);
	void								Set_InteractColliderCenter(_float3 vInteractColliderCenter);
	void								Set_InteractType(INTERACT_TYPE eInteractType) { m_tEnvironmentDesc.eInteractType = eInteractType; }
	void								Set_InteractState(INTERACT_STATE eInteractState) { m_tEnvironmentDesc.eInteractState = eInteractState; }
	void								Set_PlayerRootMoveRate(_float3 vRootMoveRate) { m_tEnvironmentDesc.vPlayerRootMoveRate = vRootMoveRate;}
	void								Set_UseGravity(_bool bUseGravity) { m_tEnvironmentDesc.bUseGravity = bUseGravity; }
	


	void								Set_LevelChangeType(_bool bLevelChange, LEVEL eLevel);
	
#endif // _DEBUG
	_int								Get_AnimationIndex() { return m_tEnvironmentDesc.iPlayAnimationIndex; }
	void								Set_AnimationIndex(_uint iAnimIndex);
	void								Set_Animation(_uint iAnimIndex, CModel::ANIM_STATE _eAnimState = CModel::ANIM_STATE::ANIM_STATE_NORMAL, _uint iTargetKeyFrameIndex = 0);
	void								Set_ShaderPassIndex(_int iShaderPassIndex) { m_tEnvironmentDesc.iShaderPassIndex = iShaderPassIndex; }


	void								Set_Exit(_bool bExit) { m_bExit = bExit;}
	void								Set_ColliderSize(_float3 vColliderSize);
	void								Set_ColliderCenter(_float3 vColliderCenter);

	void								Set_MoveRangeColliderSize(_float3 vInteractColliderSize);
	void								Set_MoveRangeColliderCenter(_float3 vInteractColliderCenter);

	void								Set_InteractMoveColliderSize(_float3 vInteractColliderSize);
	void								Set_InteractMoveColliderCenter(_float3 vInteractColliderCenter);
	void								Set_InteractMoveMode(_bool bInteractMoveMode) { m_tEnvironmentDesc.bInteractMoveMode = bInteractMoveMode; m_bInteractMoveMode = bInteractMoveMode;}

public: //! For Public
	void								Add_Force(_vector In_vDir, _float In_fPower);

	void								StartGroupInteract();
	void								Reset_Interact();
	
	void								Set_Interact(_bool bInteract) { m_bInteract = bInteract;}
	_int								Get_InteractGroupIndex() { return m_tEnvironmentDesc.iInteractGroupIndex; }
	void								Set_InteractGroupIndex(_int iGroupIndex) { m_tEnvironmentDesc.iInteractGroupIndex = iGroupIndex; }


	_bool								Is_OwnerInteract() { return m_bInteract;}
public:	//! For Spline
	void								Set_SplineJsonPath(string strJsonPath) { m_tEnvironmentDesc.strSplineJsonPath = strJsonPath;}
	void								Set_SplineDivergingCount(_int iDivergingCount) { m_iDivergingCount = iDivergingCount;} 

public: //! For RopeChainClimb _ RopeChainDown
	void								Rope_ChainFunction(const _float fTimeDelta);
	//_int								Get_RollDown // 2
	//_int								Get_ChainUpIndex(); //  7
	//_int								Get_RopeDownIndex(); 



#ifdef _DEBUG
public: //! For.Tool
	virtual _bool						Picking(_Out_ _float3* vPickedPos) override;

#endif 

public:
	void								Start_Environment_Animation() { m_bPlay = true; }

public:
	void								Interact();


public:	//! For Public
	HRESULT								Find_UI_For_InteractType();

	void								Move_For_PlayerRootMotion(); //! 플레이어의 애니메이션 움직임에 맞춰서 이동
	void								Move_For_Offset(); //! 특저 오브젝트의 위치(오프셋)기준으로 같이 이동
	void								Move_For_PlayerOffset();
	

	HRESULT								Find_InteractGroupObject(); //! 상호작용 활성화시 상호작용시켜야 할 오브젝트 찾기
	void								Set_OwnerObject(CEnvironment_Interact* pOwnerObject) { m_pOwnerInteract = pOwnerObject; }
	
	_bool								Check_OwnerEnablePosition(); //! 주체 상호작용오브젝트가 목표위치에 도달했는지 확인해주자
	
	void								Stop_PlayerForArrival();
	

	_bool								ArrivalCheck(); //! 위치벡터에 도달했는지
	_bool								RotationCheck(const _float fTimeDelta); //! 회전해야할 각도에 도달했는지.
	_bool								Check_MoveCollider();
	_bool								Check_InteractMoveCollider();
	_bool								EnableCheck();
	
	void								Set_InteractEnable(_bool bInteractEnable) { m_bInteractEnable = bInteractEnable; }

	//!_int			iInteractGroupIndex = -1; //! 특정 상호작용 오브젝트가 활성화될시 다른 상호작용 오브젝트도 활성시키기 위한 그룹핑인덱스
	//!_float4			vEnablePosition = {}; //!  특정 상호작용 오브젝트가 다른 상호작용 오브젝트를 활성화시키기 위한 위치 조건을 위한 위치벡터
	//!_float4			vArrivalPosition = {}; //! 특정 상호작용 오브젝트가 위치벡터에 도달하면 종료시키기위한 위치벡터
	//!_float4			vOffset = {}; //!  특정 상호작용 오브젝트를 기준으로 위치해야하기 위한 오프셋
	//!_float			fRadian = XMConvertToRadians(90.f); //! 특정 상호작용 오브젝트가 활성화될시 회전해야할 각도
	//!_float			fRotationSpeed = 90.f; //! 회전해야하는 오브젝트인경우 회전 속도를 저장시키기위함.
	//!_bool			bOffset = false; //! 오프셋 위치가 적용시켜져야하는 지.
	//! _bool			bLeft = true; // ! 왼쪽으로 회전할지 오른쪽으로 회전할지

	
public: //! For ToolTest
	HRESULT								Add_InteractGroupObject(CEnvironment_Interact* pInteractObject);


	void								Set_DescWorldMatrix() { m_tEnvironmentDesc.WorldMatrix = m_pTransformCom->Get_WorldMatrix();}

	void								Set_Rotate(_bool bRotate) { m_tEnvironmentDesc.bRotate = bRotate; }
	void								Set_RotationAngle(_float fAngle) { m_tEnvironmentDesc.fRotationAngle = fAngle;}
	void								Set_RotationSpeed(_float fRotationSpeed) { m_tEnvironmentDesc.fRotationSpeed = fRotationSpeed; m_pTransformCom->Set_RotationSpeed(XMConvertToRadians(fRotationSpeed)); }
	void								Set_RotationType(ROTATION_LERP_STATE eRotateState) { m_tEnvironmentDesc.eRotationState = eRotateState;}

	void								Set_OwnerPromotion(_bool bOwnerPromotion) { m_tEnvironmentDesc.bOwner = bOwnerPromotion; m_bInteractEnable = true;}
	void								Set_RootTranslate(_bool bRootTranslate) { m_tEnvironmentDesc.bRootTranslate = bRootTranslate;}
	void								Set_Offset(_bool bOffset, _float4 vOffset) { m_tEnvironmentDesc.bOffset = bOffset; m_tEnvironmentDesc.vOffset = vOffset; }
	void								Set_ArrivalMission(_bool bArrivalMission, _float4 vArrivalPosition) { m_tEnvironmentDesc.bArrival = bArrivalMission; m_tEnvironmentDesc.vArrivalPosition = vArrivalPosition;} //! 
	void								Set_EnablePosition(_float4 vEnablePosition) { m_tEnvironmentDesc.vEnablePosition= vEnablePosition; } //! 
	void								Set_Navigation(CNavigation* pNavigation) { m_pNavigationCom = pNavigation;}

	void								Set_MoveColliderRender(_bool bRender) { m_bMoveColliderRender = bRender;}
	void								Set_InteractMoveColliderRender(_bool bRender) { m_bInteractMoveColliderRender = bRender; }
	void								Set_InteractColliderRender(_bool bRender) { m_bInteractColliderRender = bRender; }
	void								Set_ColliderRender(_bool bRender) { m_bColliderRender = bRender; }

	vector<CEnvironment_Interact*>&		Get_InteractGroupVector() { return m_vecInteractGroup;}
	vector<string>&						Get_InteractGroupTag() { return m_vecInteractGroupTag;}
	CEnvironment_Interact*				Get_InteractOwner() { return m_pOwnerInteract; }
	void								Delete_InteractGroupObject(_int iIndex, _Out_ _int* iSelectIndex);

	void								Set_EnablePosition(vector<_float4>* EnablePositions);
	void								Reset_EnablePosition();
	void								Set_EnableJsonPath(string strJsonPath) { m_tEnvironmentDesc.strEnableJsonPath = strJsonPath;}
	void								Set_EnableForPoint(_bool bEnable) { m_tEnvironmentDesc.bEnable = bEnable; }

	void								Add_UpdateCellIndex(_int iCellIndex);
	vector<_int>&						Get_UpdateCellIndexs() { return m_tEnvironmentDesc.vecUpdateCellIndex;}

	void								Erase_UpdateCellForIndex(_int iCellIndex) { m_tEnvironmentDesc.vecUpdateCellIndex.erase(m_tEnvironmentDesc.vecUpdateCellIndex.begin() + iCellIndex);}
	void								Enable_UpdateCells();
	void								UnEnable_UpdateCells();

	void								Reset_Rotate() { m_bInteractEnable = true; m_pTransformCom->Set_WorldMatrix(m_tEnvironmentDesc.WorldMatrix); }

public: //! For RollerCoster Wagon && Spline
	void								Start_SplineEvent() { m_bSpline = true; }
	void								Reset_TestEvent();

	void								Start_Spline(vector<_float4>* SplinePoints);
	void								Start_SplineDouble(vector<_float4>* SplinePoints);
	void								Reset_StartMatrix();
	void								Spline_Loop(const _float fTimeDelta);
	void								Spline_LoopDoublePoint(const _float fTimeDelta);
	void								Spline_Clear();
	
	void								Set_SplineSpeed(_float fSpeed) { m_fSplineSpeed = /*15.f;*/fSpeed; m_pTransformCom->Set_Speed(/*15.f*/fSpeed); }
	void								Set_ArrivalTime(_float fArriavalTime) { m_fArrivalTime = fArriavalTime;}

	void								Set_SplineCheck(_int iIndex, _bool bCheck) { m_vecPointChecks[iIndex] = bCheck; }

private:
	HRESULT								Load_SplineJson();
	HRESULT								Load_EnableJson();
	HRESULT								Init_WagonEvent();
	void								Change_WagonTrack(const _float fTimeDelta);
	_vector								CatmullRomInterpolation(_fvector p0, _fvector p1, _fvector p2, _fvector p3, _float t);
	vector<_float4>						CreateSmoothSpline(vector<_float4>& points, _int segments);
	

private:
	CShader*							m_pShaderCom = { nullptr };	
	CModel*								m_pModelCom = { nullptr };


	CCollider*							m_pColliderCom = { nullptr };
	CRigidBody*							m_pRigidBodyCom = { nullptr };

	CCollider*							m_pInteractColliderCom = { nullptr };
	CCollider*							m_pMoveRangeColliderCom = { nullptr };

	CCollider*							m_pInteractMoveColliderCom = { nullptr };

	CNavigation*						m_pNavigationCom = { nullptr };
	
	

	_int								m_iCurrentLevelIndex = -1;

private:
	ENVIRONMENT_INTERACTOBJECT_DESC		m_tEnvironmentDesc = {};
	_bool								m_bPlay = false;
	
	_bool								m_bTest = false;

	_bool								m_bInteract = false;
	_bool								m_bInteractStart = false;
	
	_bool								m_bFindPlayer = false;

	_bool								m_bSpline = false;
	_bool								m_bInteractEnable = true;
	_bool								m_bInteractMoveMode = false;
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
	_float4								m_vArrivalPosition = { 99999.f, 99999.f, 99999.f, 1.f };

	_int								m_iCalcCount = 0;
	_bool								m_bArrival = false;
	_bool								m_bMove = true;
	
	_bool								m_bExit = false; 



	vector<CEnvironment_Interact*>		m_vecInteractGroup;
	vector<string>						m_vecInteractGroupTag; //! 툴 또는 디버깅용
	CEnvironment_Interact*				m_pOwnerInteract = { nullptr }; //! 특정 상호작용 오브젝트가 이동된다면 같이 움직여져야 할 경우 찾아야함.

	
	
	vector<_float4>						m_vecEnablePosition;


	_bool								m_bMoveColliderRender = false;
	_bool								m_bInteractMoveColliderRender = false;
	_bool								m_bInteractColliderRender = true;
	_bool								m_bColliderRender = false;

private:
	CPlayer*						    m_pPlayer = { nullptr };

	// !성희 추가
	CUI_Manager*						m_pUIManager = { nullptr };
	CUI_Interaction*					m_pUI_Interaction = { nullptr };
	CUI*								m_pWeaknessUI = { nullptr };

private:
	HRESULT						Ready_Components();
	HRESULT						Ready_InteractCollider(INTERACT_TYPE eInteractType);
	HRESULT						Bind_ShaderResources();

	/* 소영추가 - 렌더용 */
public:

private:
	HRESULT		Classification_Model(); 
	vector<_int>	m_vChainMesh = {};

	_bool		m_bIncrease = true;
	_bool		m_bRenderOutLine = { false };


	/* Origin Shader */
	_float		m_gCamFar				= 0.f;

	/* OutLine Shader */
	_float		m_fTimeAcc				= 0.f; /* 디퓨즈 자체 */

	_float4		m_vLineColor			= { 1.f, 1.f, 1.f, 1.f };
	_bool		m_bLineIncrease			= true;
	_float		m_fLineThick			= 0.f;
	_float		m_fLineTimeAcc			= 0.f;
	_float		m_fLineThick_Ratio		= 0.f;

public:
	/* 원형객체를 생성한다. */
	static CEnvironment_Interact* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;

	virtual void Free() override;

};

END