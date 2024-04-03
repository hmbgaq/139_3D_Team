#pragma once

#include "Renderer.h"
#include "PipeLine.h"
#include "Component_Manager.h"

/* 클라이언트에서 엔진의 기능을 사용하기위해 반드시 거쳐야하는 객체. */

/* DX11을 통해 필요한 객체를 생성하고 렌더링하는 작업을 수행한다. */
/* 지금 생성한 레벨(씬)을 보관하고 필요에따라 업데이트 수행. */
/* 내가 생성한 객체들을 보관하고 업데이트하고 렌더한다. */
/* 내가 생성한 여러 컴포넌트들의 원형을 보관하고 복제하여 가져온다. */

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public: /* For.Engine */
	/* 엔진라이브러리를 사용하기위한 준비를 모두 거친다. */
	HRESULT		Initialize_Engine(_uint iNumLevels, _uint iNumCollsionLayer, _uint iNumPhysXCollsionLayer, HINSTANCE hInstance, const GRAPHIC_DESC& GraphicDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext);
	void		Tick_Engine(_float fTimeDelta);
	HRESULT		Render_Engine();
	void		Clear(_uint iLevelIndex);

public: /* For.Graphic_Device */
	HRESULT		Clear_BackBuffer_View(_float4 vClearColor);
	HRESULT		Clear_DepthStencil_View();
	HRESULT		Present();
	ID3D11RenderTargetView* Get_BackBufferRTV() const;
	ID3D11DepthStencilView* Get_DSV() const;
	GRAPHIC_DESC*			Get_GraphicDesc();
	ID3D11ShaderResourceView* Get_DepthSRV();
	GFSDK_SSAO_Context_D3D11* Get_AOContext();


public: /* For.Input_Device */
	_byte		Get_DIKeyState(_ubyte byKeyID);
	_byte		Get_DIMouseState(MOUSEKEYSTATE eMouse);
	_long		Get_DIMouseMove(MOUSEMOVESTATE eMouseState);

	_bool		Key_Pressing(_ubyte byKeyID);
	_bool		Key_Down(_ubyte byKeyID);
	_bool		Key_Up(_ubyte byKeyID);
	_bool		Mouse_Pressing(MOUSEKEYSTATE eMouse);
	_bool		Mouse_Down(MOUSEKEYSTATE eMouse);
	_bool		Mouse_Up(MOUSEKEYSTATE eMouse);

public: /* For.Timer_Manager */
	HRESULT		Add_Timer(const wstring& strTimeTag);
	_float		Compute_TimeDelta(const wstring& strTimeTag);

public: /* For.Level_Manager */
	HRESULT		Open_Level(_uint iCurrentLevelIndex, class CLevel* pNewLevel);
	_uint		Get_NextLevel();
	_uint		Get_CurrentLevel();
	void		Set_CurrentLevel(_uint CurrentLevel);

public: /* For.Object_Manager */
	HRESULT				Add_Prototype(const wstring& strPrototypeTag, class CGameObject* pPrototype);
	HRESULT				Add_CloneObject(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg = nullptr);
	CGameObject*		Clone_Prototype(const wstring& strPrototypeTag, void* pArg = nullptr);
	class CComponent*	Get_Component(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComponentTag, _uint iIndex = 0, const wstring& strPartTag = TEXT(""));

	list<CGameObject*>* Get_GameObjects(_uint iLevelIndex, const wstring & strLayerTag);
	void				Get_CloneGameObjects(_uint iLevelIndex, vector<CGameObject*>*clonevector);
	class CGameObject*	Get_GameObect_Last(_uint iLevelIndex, const wstring & strLayerTag);
	class CGameObject*	Add_CloneObject_And_Get(_uint iLevelIndex, const wstring & strLayerTag, const wstring & strPrototypeTag, void* pArg = nullptr);
	class CCharacter*	Get_Player();
	void				Set_Player(class CCharacter* _pPlayer);
	HRESULT				Create_PoolObjects(const wstring & strPrototypeTag, _uint iSize = 10);
	void				Fill_PrototypeTags(vector<string>*_vector);

public: /* For.Component_Manager */
	HRESULT				Add_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag, class CComponent* pPrototype);
	class CComponent*	Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg = nullptr);

public: /* For.Renderer */
	HRESULT		Add_RenderGroup(CRenderer::RENDERGROUP eGroupID, class CGameObject* pGameObject);
	HRESULT		Add_DebugRender(class CComponent* pDebugCom);
	CRenderer*	Get_Renderer(); /* 툴용 */
	HRESULT		Off_Shader();
	void		Set_ToolPBRTexture_InsteadLevel(_int iPBRTexture);
	HRESULT		Add_CascadeObject(_uint iIndex, CGameObject* pObject);

#ifdef _DEBUG
	void		Set_RenderDebugCom(_bool _bRenderDebug);
	void		Set_RenderDebugTarget(_bool _bRenderTarget);
#endif

public: /* For.PipeLine */
	void		Set_Transform(CPipeLine::D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix);
	void		Set_Transform(CPipeLine::D3DTRANSFORMSTATE eState, _float4x4 TransformMatrix);
	_matrix		Get_TransformMatrix(CPipeLine::D3DTRANSFORMSTATE eState);
	_float4x4	Get_TransformFloat4x4(CPipeLine::D3DTRANSFORMSTATE eState);
	_matrix		Get_TransformMatrixInverse(CPipeLine::D3DTRANSFORMSTATE eState);
	_float4x4	Get_TransformFloat4x4Inverse(CPipeLine::D3DTRANSFORMSTATE eState);
	_float4		Get_CamPosition();
	_float4		Get_CamDirection();
	_float4		Get_CamSetting();
	_float		Get_CamFar();
	_float4x4*	Get_Shadow_Proj();
	void		Set_ShadowProj(_float4x4 * pMatrix);
	void		Set_CascadeBoxes(BoundingOrientedBox * pBoxes);

	//!			레이캐스트
	RAY			Get_MouseRayWorld(HWND g_hWnd, const unsigned int	g_iWinSizeX, const unsigned int	g_iWinSizeY);
	RAY			Get_MouseRayLocal(HWND g_hWnd, const unsigned int	g_iWinSizeX, const unsigned int	g_iWinSizeY, _matrix matWorld);

#ifdef _DEBUG
	_bool		Picking_Mesh(RAY ray, _float3 * out, vector<class CMesh*> Meshes);
	_bool		Picking_Vertex(RAY ray, _float3 * out, _uint triNum, VTXMESH * pVertices, _uint * pIndices);
#endif // _DEBUG

public: /* For.Font_Manager */
	HRESULT		Add_Font(const wstring& strFontTag, const wstring& strFontFilePath);
	// 매개변수 : ->폰트테그, 내용, 위치, 색상, 크기, 내용이 나올 원점(0~1 : UV좌표와 비슷함), 회전<-
	HRESULT		Render_Font(const wstring& strFontTag, const wstring& strText, const _float2 & vPosition, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f ,1.f), _float fScale = 1.f, _float2 vOrigin = _float2(0.f, 0.f), _float fRotation = 0.f);

public: /* For.Target_Manager */
	HRESULT		Add_RenderTarget(const wstring& strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT		Add_MRT(const wstring& strMRTTag, const wstring& strTargetTag);
	HRESULT		Begin_MRT(const wstring & strMRTTag, ID3D11DepthStencilView * pDSV = nullptr, _bool bClear = true);
	HRESULT		End_MRT();
	HRESULT		Clear_MRT(const wstring& strMRTTag);
	HRESULT		Clear_Target(const wstring& strMRTTag, const wstring& strTargetTag);
	HRESULT		Bind_RenderTarget_ShaderResource(const wstring& strTargetTag, class CShader* pShader, const _char* pConstantName);
	class CRenderTarget* Find_RenderTarget(const wstring& strTargetTag);
	void		Create_RenderTarget(const wstring& strTargetTag);
	
#ifdef _DEBUG
	HRESULT		Ready_RenderTarget_Debug(const wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT		Render_Debug_RTVs(const wstring& strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif

public: /* For.Light_Manager */
	HRESULT			Add_Light(const LIGHT_DESC& LightDesc, _int & outLightIndex);
	class CLight*   Add_Light_AndGet(const LIGHT_DESC& LightDesc, _uint& outLightIndex);
	class CLight*	Find_Light(const _int iIndex);
	void			Change_Light_Desc(const _int iIndex, LIGHT_DESC newDesc);
	class CLight*	Get_DirectionLight();
	class CLight_Manager* Get_LightManager();
	void			Get_AllLight(list<class CLight*>* pTemp);
	_float4x4		Get_StaticLight();
	HRESULT			Ready_StaticLightMatrix(_float3 vPos, _float3 vLook);


	HRESULT		Render_Lights(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
	_bool		Remove_Light(const _uint& iIndex);
	HRESULT		Set_ShadowLight(_uint iLevelIndex, _float4 vEye, _float4 vAt, _float4 vUp);
	HRESULT		Add_ShadowLight_View(_uint iLevelIndex, _vector vEye, _vector vAt, _vector vUp);
	HRESULT		Add_ShadowLight_Proj(_uint iLevelIndex, _float fFovAngleY, _float fAspectRatio, _float fNearZ, _float fFarZ);
	_float4x4	Get_ShadowLightViewMatrix(_uint iLevelIndex);
	_float4x4	Get_ShadowLightViewMatrix_Inverse(_uint iLevelIndex);
	_float4x4	Get_ShadowLightProjMatrix(_uint iLevelIndex);
	_float		Get_ShadowLightFar(_uint iLevelIndex);
	_float4		Get_ShadowLightPos(_uint iLevelIndex);

public: /* For.Frustum */
	void		Transform_Frustum_ToLocalSpace(_fmatrix WorldMatrix);
	_bool		isIn_WorldPlanes(_fvector vPoint, _float fRadius = 0.f);
	_bool		isIn_LocalPlanes(_fvector vPoint, _float fRadius);

public: /* For.Collision_Manager */
	void		Add_Collision(const _uint& In_iLayer, CCollider* _pCollider);

	void		Check_Group(const _uint& In_iLeftLayer, const _uint& In_iRightLayer);


public: /* For.Event_Manager */
	void		Add_Event(class IEvent* pEvent);
	void		Clear_Event();

public: /* For.PhysX_Manager */
	void					Register_PhysXCollider(class CPhysXCollider* pPhysXCollider);
	class CPhysXCollider*	Find_PhysXCollider(const _uint iPhysXColliderIndex);

	void					Register_PhysXController(class CPhysXController* pPhysXController);
	class CPhysXController*	Find_PhysXController(const _uint iPhysXControllerIndex);

	void					Check_PhysXFilterGroup(const _uint In_iLeftLayer, const _uint In_iRightLayer);
	_uint					Get_PhysXFilterGroup(const _uint In_iIndex);

	PxRigidDynamic*			Create_DynamicActor(const PxTransform& transform, const PxGeometry& geometry, PxMaterial* pMaterial = nullptr);
	PxRigidDynamic*			Create_DynamicActor(const PxTransform& transform);
	PxRigidStatic*			Create_StaticActor(const PxTransform& transform, const PxGeometry& geometry, PxMaterial* pMaterial = nullptr);
	PxRigidStatic*			Create_StaticActor(const PxTransform& transform);

	void					Add_DynamicActorAtCurrentScene(PxRigidDynamic& DynamicActor);
	void					Add_StaticActorAtCurrentScene(PxRigidStatic& StaticActor);

	void					Create_Material(_float fStaticFriction, _float fDynamicFriction, _float fRestitution, PxMaterial** ppOut);
	void					Create_ConvexMesh(PxVec3** pVertices, _uint iNumVertice, PxConvexMesh** ppOut);
	void					Create_ConvexMesh(const PxConvexMeshDesc& In_MeshDesc, PxConvexMesh** ppOut);
	void					Create_Shape(const PxGeometry& Geometry, PxMaterial* pMaterial, const _bool isExculsive, const PxShapeFlags In_ShapeFlags, PxShape** ppOut);
	void					Create_MeshFromTriangles(const PxTriangleMeshDesc& In_MeshDesc, PxTriangleMesh** ppOut);
	void					Create_Controller(const PxCapsuleControllerDesc& In_ControllerDesc, PxController** ppOut);

public: /* For.Random_Manager*/
	const _float&			Random_Float(_float fMin, _float fMax);
	const _int&				Random_Int(_int iMin, _int iMax);
	const _bool&			Random_Coin(_float fProbality);
	int64_t					GenerateUniqueID();

public: /* For.Sound_Manager*/
	void Play_Sound(const wstring& strGroupKey, const wstring& strSoundKey, CHANNELID eID, float fVolume);
	void Play_BGM(const wstring& strGroupKey, const wstring& strSoundKey, float fVolume);
	void Stop_Sound(CHANNELID eID);
	void Stop_All();
	void Set_ChannelVolume(CHANNELID eID, float fVolume);

public: /* Common */
	void		String_To_WString(string _string, wstring & _wstring);
	void		WString_To_String(wstring _wstring, string & _string);
	wstring		SliceObjectTag(const wstring& strObjectTag);
	

#pragma region 성희
	// wstring을 string으로 변환 해주는 함수
	string		Convert_WString_To_String(wstring _wstring);
	// string을 wchar로 변환 해주는 함수
	WCHAR*		StringTowchar(const std::string& str);
	//	wstring을 char로 변환 해주는 함수
	char*		ConverWStringtoC(const wstring& wstr);
	//	char를 wchar_t로 변환 해주는 함수
	wchar_t*	ConverCtoWC(char* str);
	// WCHAR*를 string으로 변환 해주는 함수
	std::string WideStringToString(const wchar_t* wideStr);
	// 경로에서 파일이름과 확장자만 추출해주는 함수
	std::string GetFileName(const std::string& filePath);
	// 확장자를 제거해주는 함수
	std::string RemoveExtension(const std::string& filePath);
	// 포지션 값을 직교로 계산해주는 함수
	_vector		Convert_Orthogonal(_vector vPosition);
#pragma endregion End

#pragma region 유정
	string		Wstring_To_UTF8(const wstring& wstr);
	wstring		Char_To_Wstring(char* szChar);

	const wstring	Remove_LastNumChar(const wstring& str, const _uint& iNumCutCount);
	const string	Remove_LastNumChar(const string& str, const _uint& iNumCutCount);
	const wstring	Get_LastNumChar(const wstring& str, const _uint& iNumCutCount);
#pragma endregion End

#pragma region 소영
	void Get_ModelTag(vector<string>* pVector);
#pragma endregion


#pragma region 명빈
public:
	void Hitlag(_float fTime = 1.f) { m_fHitlag_Time = fTime; };
	_float Get_TimeDelta() { return m_fTimeDelta; };
	void Update_Hitlag(_float fTimeDelta);

	void Set_RadialBlurTime(_float fTime = 0.3f) { m_fRadialBlurTime = max(m_fRadialBlurTime, fTime); };
	void Update_RadialBlurTime(_float fTimeDelta);
#pragma endregion

private:
	class CGraphic_Device*			m_pGraphic_Device = { nullptr };
	class CInput_Device*			m_pInput_Device = { nullptr };
	class CTimer_Manager*			m_pTimer_Manager = { nullptr };
	class CLevel_Manager*			m_pLevel_Manager = { nullptr };
	class CObject_Manager*			m_pObject_Manager = { nullptr };
	class CComponent_Manager*		m_pComponent_Manager = { nullptr };
	class CRenderer*				m_pRenderer = { nullptr };
	class CPipeLine*				m_pPipeLine = { nullptr };
	class CFont_Manager*			m_pFont_Manager = { nullptr };
	class CTarget_Manager*			m_pTarget_Manager = { nullptr };
	class CLight_Manager*			m_pLight_Manager = { nullptr };
	class CFrustum*					m_pFrustum = { nullptr };
	class CCollision_Manager*		m_pCollision_Manager = { nullptr };
	class CEvent_Manager*			m_pEvent_Manager = { nullptr };
	class CPhysX_Manager*			m_pPhysX_Manager = { nullptr };
	class CRandom_Manager*			m_pRandom_Manager = { nullptr };
	class CSound_Manager*			m_pSoundManager = { nullptr };


private:
	_float m_fHitlag_Time = { 0.f };
	_float m_fTimeDelta = { 0.f };
	_float m_fRadialBlurTime = { 0.f };


public:
	void Release_Manager();
	static void Release_Engine();
};

END