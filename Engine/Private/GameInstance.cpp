#include "..\Public\GameInstance.h"

#include "Collision_Manager.h"
#include "Graphic_Device.h"
#include "Object_Manager.h"
#include "Target_Manager.h"
#include "Event_Manager.h"
#include "TImer_Manager.h"
#include "Level_Manager.h"
#include "Light_Manager.h"
#include "Input_Device.h"
#include "Font_Manager.h"
#include "PhysX_Manager.h"
#include "RandomManager.h"
#include "Sound_Manager.h"

#include "Renderer.h"
#include "Frustum.h"
#include "Mesh.h"

#include "PhysXCollider.h"
#include "PhysXController.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{
}

HRESULT CGameInstance::Initialize_Engine(_uint iNumLevels, _uint iNumCollsionLayer, _uint iNumPhysXCollsionLayer, HINSTANCE hInstance, const GRAPHIC_DESC& GraphicDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext)
{
	/* 그래픽 디바이스를 초기화 하자.*/
	m_pGraphic_Device = CGraphic_Device::Create(GraphicDesc, ppDevice, ppContext);
	NULL_CHECK_RETURN(m_pGraphic_Device, E_FAIL);

	m_pInput_Device = CInput_Device::Create(hInstance, GraphicDesc.hWnd);
	NULL_CHECK_RETURN(m_pInput_Device, E_FAIL);

	/* 타이머를 사용할 준비를 하자. */
	m_pTimer_Manager = CTimer_Manager::Create();
	NULL_CHECK_RETURN(m_pTimer_Manager, E_FAIL);

	m_pLevel_Manager = CLevel_Manager::Create();
	NULL_CHECK_RETURN(m_pLevel_Manager, E_FAIL);

	m_pObject_Manager = CObject_Manager::Create(iNumLevels);
	NULL_CHECK_RETURN(m_pObject_Manager, E_FAIL);

	m_pComponent_Manager = CComponent_Manager::Create(iNumLevels);
	NULL_CHECK_RETURN(m_pComponent_Manager, E_FAIL);

	m_pFont_Manager = CFont_Manager::Create(*ppDevice, *ppContext);
	NULL_CHECK_RETURN(m_pFont_Manager, E_FAIL);

	m_pTarget_Manager = CTarget_Manager::Create(*ppDevice, *ppContext);
	NULL_CHECK_RETURN(m_pTarget_Manager, E_FAIL);

	m_pRenderer = CRenderer::Create(*ppDevice, *ppContext);
	NULL_CHECK_RETURN(m_pRenderer, E_FAIL);

	m_pPipeLine = CPipeLine::Create();
	NULL_CHECK_RETURN(m_pPipeLine, E_FAIL);

	m_pLight_Manager = CLight_Manager::Create();
	NULL_CHECK_RETURN(m_pLight_Manager, E_FAIL);

	m_pFrustum = CFrustum::Create();
	NULL_CHECK_RETURN(m_pFrustum, E_FAIL);

	m_pCollision_Manager = CCollision_Manager::Create(iNumCollsionLayer);
	NULL_CHECK_RETURN(m_pCollision_Manager, E_FAIL);

	m_pEvent_Manager = CEvent_Manager::Create();
	NULL_CHECK_RETURN(m_pEvent_Manager, E_FAIL);

	//TODO: 레벨 확인헤야
	m_pPhysX_Manager = CPhysX_Manager::Create(*ppDevice, *ppContext, iNumPhysXCollsionLayer);
	NULL_CHECK_RETURN(m_pPhysX_Manager, E_FAIL);

	m_pRandom_Manager = CRandom_Manager::Create();
	NULL_CHECK_RETURN(m_pRandom_Manager, E_FAIL);

	m_pSoundManager = CSound_Manager::Create();
	NULL_CHECK_RETURN(m_pSoundManager, E_FAIL);

	return S_OK;
}

void CGameInstance::Tick_Engine(_float fTimeDelta)
{
	if (nullptr == m_pLevel_Manager ||
		nullptr == m_pObject_Manager ||
		nullptr == m_pPipeLine ||
		nullptr == m_pInput_Device ||
		nullptr == m_pFrustum)
		return;

	Update_Hitlag(fTimeDelta);
	Update_RadialBlurTime(m_fTimeDelta);
	Update_ChromaTime(m_fTimeDelta);

	m_pInput_Device->Tick();

	m_pObject_Manager->Priority_Tick(m_fTimeDelta);

	m_pObject_Manager->Tick(m_fTimeDelta);

	m_pPipeLine->Tick();

	m_pFrustum->Tick();

	m_pEvent_Manager->Tick(m_fTimeDelta);

	m_pCollision_Manager->Tick(m_fTimeDelta);

	m_pPhysX_Manager->Tick(m_fTimeDelta);

	m_pObject_Manager->Late_Tick(m_fTimeDelta);

	m_pLevel_Manager->Tick(m_fTimeDelta);



}

void CGameInstance::Clear(_uint iLevelIndex)
{
	if (nullptr == m_pObject_Manager ||
		nullptr == m_pComponent_Manager //|| 
		/*nullptr == m_pEvent_Manager*/)
		return;

	/* 오브젝트 매니져에 레벨별로 구분해 놓은 객체들 중 특정된 객체들을 지운다.  */
	m_pObject_Manager->Clear(iLevelIndex);

	/* 컴포넌트 매니져에 레벨별로 구분해 놓은 컴포넌트들 중 특정된 객체들을 지운다.  */
	m_pComponent_Manager->Clear(iLevelIndex);

	//m_pRenderer->

	/*m_pEvent_Manager->Clear();*/
}

HRESULT CGameInstance::Render_Engine()
{
	if (nullptr == m_pLevel_Manager ||
		nullptr == m_pRenderer)
		return E_FAIL;

	m_pRenderer->Draw_RenderGroup();

#ifdef _DEBUG
	m_pLevel_Manager->Render();
#endif

	m_pInput_Device->LateTick();

	Open_Requested_Level();
	return S_OK;
}



HRESULT CGameInstance::Clear_BackBuffer_View(_float4 vClearColor)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;
	return m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);
}

HRESULT CGameInstance::Clear_DepthStencil_View()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;
	return m_pGraphic_Device->Clear_DepthStencil_View();
}

HRESULT CGameInstance::Present()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;
	return m_pGraphic_Device->Present();
}

ID3D11RenderTargetView * CGameInstance::Get_BackBufferRTV() const
{
	if (nullptr == m_pGraphic_Device)
		return nullptr;

	return m_pGraphic_Device->Get_BackBufferRTV();
}

ID3D11DepthStencilView * CGameInstance::Get_DSV() const
{
	if (nullptr == m_pGraphic_Device)
		return nullptr;

	return m_pGraphic_Device->Get_DSV();
}

GRAPHIC_DESC* CGameInstance::Get_GraphicDesc()
{
	if (nullptr == m_pGraphic_Device)
		return nullptr;

	return m_pGraphic_Device->Get_GraphicDesc();
}

ID3D11ShaderResourceView* CGameInstance::Get_DepthSRV()
{
	if (nullptr == m_pGraphic_Device)
		return nullptr;

	return m_pGraphic_Device->Get_DepthSRV();
}

GFSDK_SSAO_Context_D3D11* CGameInstance::Get_AOContext()
{
	if (nullptr == m_pGraphic_Device)
		return nullptr;

	return m_pGraphic_Device->Get_AOContext();
}

_byte CGameInstance::Get_DIKeyState(_ubyte byKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;
	return m_pInput_Device->Get_DIKeyState(byKeyID);
}

_byte CGameInstance::Get_DIMouseState(MOUSEKEYSTATE eMouse)
{

	if (nullptr == m_pInput_Device)
		return 0;
	return m_pInput_Device->Get_DIMouseState(eMouse);
}

_long CGameInstance::Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMouseMove(eMouseState);
}

_bool CGameInstance::Key_Pressing(_ubyte byKeyID)
{
	return m_pInput_Device->Key_Pressing(byKeyID);
}

_bool CGameInstance::Key_Down(_ubyte byKeyID)
{
	return m_pInput_Device->Key_Down(byKeyID);
}

_bool CGameInstance::Key_Up(_ubyte byKeyID)
{
	return m_pInput_Device->Key_Up(byKeyID);
}

_bool CGameInstance::Mouse_Pressing(MOUSEKEYSTATE eMouse)
{
	return m_pInput_Device->Mouse_Pressing(eMouse);
}

_bool CGameInstance::Mouse_Down(MOUSEKEYSTATE eMouse)
{
	return m_pInput_Device->Mouse_Down(eMouse);
}

_bool CGameInstance::Mouse_Up(MOUSEKEYSTATE eMouse)
{
	return m_pInput_Device->Mouse_Up(eMouse);
}

HRESULT CGameInstance::Add_Timer(const wstring & strTimeTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Add_Timer(strTimeTag);
}

_float CGameInstance::Compute_TimeDelta(const wstring & strTimeTag)
{
	if (nullptr == m_pTimer_Manager)
		return 0.0f;

	return m_pTimer_Manager->Compute_TimeDelta(strTimeTag);
}

HRESULT CGameInstance::Request_Level_Opening(_uint iCurrentLevelIndex, CLevel* pNewLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	m_iCurrentLevelIndex = iCurrentLevelIndex;
	m_pNewLevel = pNewLevel;
	m_bIsRequestOpenLevel = true;

	return S_OK;
}

HRESULT CGameInstance::Open_Requested_Level()
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	if (true == m_bIsRequestOpenLevel)
	{
		m_bIsRequestOpenLevel = false;
		Off_Shader();					 /* 셰이더 옵션 다 끄고 오픈하도록 함*/
		return m_pLevel_Manager->Open_Level(m_iCurrentLevelIndex, m_pNewLevel);
	}

	return S_OK;
}

HRESULT CGameInstance::Open_Level(_uint iCurrentLevelIndex, CLevel * pNewLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Open_Level(iCurrentLevelIndex, pNewLevel);
}

_uint CGameInstance::Get_NextLevel()
{
	return m_pLevel_Manager->Get_NextLevel();
}

// 현재 레벨 받아오기 추가
_uint CGameInstance::Get_CurrentLevel()
{
	return m_pLevel_Manager->Get_CurrentLevel();
}

void CGameInstance::Set_CurrentLevel(_uint CurrentLevel)
{
	m_pLevel_Manager->Set_CurrentLevel(CurrentLevel);
}

HRESULT CGameInstance::Set_ShaderOption(_uint CurrentLevel, string filePath)
{
	NULL_CHECK_RETURN(m_pLevel_Manager, E_FAIL);

	m_pLevel_Manager->Set_ShaderOption(CurrentLevel, filePath);

	return S_OK;
}

HRESULT CGameInstance::Add_Prototype(const wstring & strPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype(strPrototypeTag, pPrototype);
}

HRESULT CGameInstance::Add_CloneObject(_uint iLevelIndex, const wstring & strLayerTag, const wstring & strPrototypeTag, void * pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_CloneObject(iLevelIndex, strLayerTag, strPrototypeTag, pArg);
}

CGameObject * CGameInstance::Clone_Prototype(const wstring & strPrototypeTag, void * pArg)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Clone_Prototype(strPrototypeTag, pArg);
}

CComponent * CGameInstance::Get_Component(_uint iLevelIndex, const wstring & strLayerTag, const wstring & strComponentTag, _uint iIndex, const wstring& strPartTag)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_Component(iLevelIndex, strLayerTag, strComponentTag, iIndex, strPartTag);
}

list<class CGameObject*>* CGameInstance::Get_GameObjects(_uint iLevelIndex, const wstring& strLayerTag)
{
	return m_pObject_Manager->Get_GameObjects(iLevelIndex, strLayerTag);
}

void CGameInstance::Get_CloneGameObjects(_uint iLevelIndex, vector<CGameObject*>* clonevector)
{
	m_pObject_Manager->Get_CloneGameObjects(iLevelIndex, clonevector);
}

CGameObject* CGameInstance::Get_GameObect_Last(_uint iLevelIndex, const wstring& strLayerTag)
{
	list<class CGameObject*>* pGameObjects = Get_GameObjects(iLevelIndex, strLayerTag);
	if (nullptr == pGameObjects)
		return nullptr;

	return pGameObjects->back();
}

CGameObject* CGameInstance::Add_CloneObject_And_Get(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg)
{
	FAILED_CHECK_RETURN(Add_CloneObject(iLevelIndex, strLayerTag, strPrototypeTag, pArg), nullptr);

	return Get_GameObect_Last(iLevelIndex, strLayerTag);
}

CCharacter* CGameInstance::Get_Player()
{
	NULL_CHECK_RETURN(m_pObject_Manager, nullptr);

	return m_pObject_Manager->Get_Player();
}

void CGameInstance::Set_Player(CCharacter* _pPlayer)
{
	NULL_CHECK_RETURN(m_pObject_Manager, );

	m_pObject_Manager->Set_Player(_pPlayer);
}

HRESULT CGameInstance::Create_PoolObjects(const wstring& strPrototypeTag, _uint iSize)
{
	NULL_CHECK_RETURN(m_pObject_Manager, E_FAIL);

	return m_pObject_Manager->Create_PoolObjects(strPrototypeTag, iSize);
}

void CGameInstance::Fill_PrototypeTags(vector<string>* _vector)
{
	NULL_CHECK_RETURN(m_pObject_Manager, );	

	m_pObject_Manager->Fill_PrototypeTags(_vector);
}


HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const wstring & strPrototypeTag, CComponent * pPrototype)
{
	NULL_CHECK_RETURN(m_pComponent_Manager, E_FAIL);

	return m_pComponent_Manager->Add_Prototype(iLevelIndex, strPrototypeTag, pPrototype);
}

CComponent * CGameInstance::Clone_Component(_uint iLevelIndex, const wstring & strPrototypeTag, void * pArg)
{
	NULL_CHECK_RETURN(m_pComponent_Manager, nullptr);

	return m_pComponent_Manager->Clone_Component(iLevelIndex, strPrototypeTag, pArg);
}

HRESULT CGameInstance::Add_RenderGroup(CRenderer::RENDERGROUP eGroupID, CGameObject * pGameObject)
{
	NULL_CHECK_RETURN(m_pRenderer, E_FAIL);

	return m_pRenderer->Add_RenderGroup(eGroupID, pGameObject);
}

HRESULT CGameInstance::Add_DebugRender(CComponent * pDebugCom)
{
	NULL_CHECK_RETURN(m_pRenderer, E_FAIL);

	return m_pRenderer->Add_DebugRender(pDebugCom);
}

CRenderer* CGameInstance::Get_Renderer()
{
	NULL_CHECK_RETURN(m_pRenderer, nullptr);

	return m_pRenderer;
}

HRESULT CGameInstance::Off_Shader()
{
	NULL_CHECK_RETURN(m_pRenderer, E_FAIL);

	return m_pRenderer->Off_Shader();
}

void CGameInstance::Set_ToolPBRTexture_InsteadLevel(_int iPBRTexture)
{
	NULL_CHECK_RETURN(m_pRenderer, );

	return m_pRenderer->Set_ToolPBRTexture_InsteadLevel(iPBRTexture);
}

HRESULT CGameInstance::Add_CascadeObject(_uint iIndex, CGameObject* pObject)
{
	NULL_CHECK_RETURN(m_pRenderer, E_FAIL);

	return m_pRenderer->Add_CascadeObject(iIndex, pObject);
}

void CGameInstance::Set_PlayerDead(_bool bOption)
{
	NULL_CHECK_RETURN(m_pRenderer, );

	return m_pRenderer->Set_PlayerDeadState(bOption);
}

void CGameInstance::Set_PlayerRebirthState(_bool bOption)
{
	NULL_CHECK_RETURN(m_pRenderer, );

	return m_pRenderer->Set_PlayerRebirthState(bOption);
}

void CGameInstance::Set_UIRender_Tool(_bool bOption)
{
	NULL_CHECK_RETURN(m_pRenderer, );

	return m_pRenderer->Set_UIRender_Tool(bOption);
}

void CGameInstance::Set_RenderTarget_Type(CRenderer::TARGET_TYPE eType)
{
	NULL_CHECK_RETURN(m_pRenderer, );

	return m_pRenderer->Set_RenderTarget_Type(eType);
}

HRESULT CGameInstance::Change_DebugRenderTarget(CRenderer::TARGET_TYPE type)
{
	NULL_CHECK_RETURN(m_pRenderer, E_FAIL);

	Clear_All_DebugRenderTarget();
	return m_pRenderer->Change_DebugRenderTarget(type);
}

HRESULT CGameInstance::Clear_All_DebugRenderTarget()
{
	NULL_CHECK_RETURN(m_pTarget_Manager, E_FAIL);

	return m_pTarget_Manager->Clear_All_DebugRenderTarget();
}

#ifdef _DEBUG
void CGameInstance::Set_RenderDebugCom(_bool _bRenderDebug)
{
	NULL_CHECK_RETURN(m_pRenderer, );

	m_pRenderer->Set_DebugCom(_bRenderDebug);
}

void CGameInstance::Set_RenderDebugTarget(_bool _bRenderTarget)
{
	/* 디버그용 렌더타겟 */
	NULL_CHECK_RETURN(m_pRenderer, );
	m_pRenderer->Set_DebugRenderTarget(_bRenderTarget);
}
#endif


void CGameInstance::Set_Transform(CPipeLine::D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix)
{
	NULL_CHECK_RETURN(m_pPipeLine, );

	m_pPipeLine->Set_Transform(eState, TransformMatrix);
}

void CGameInstance::Set_Transform(CPipeLine::D3DTRANSFORMSTATE eState, _float4x4 TransformMatrix)
{
	NULL_CHECK_RETURN(m_pPipeLine, );

	m_pPipeLine->Set_Transform(eState, TransformMatrix);
}

_matrix CGameInstance::Get_TransformMatrix(CPipeLine::D3DTRANSFORMSTATE eState)
{
	NULL_CHECK_RETURN(m_pPipeLine, XMMatrixIdentity());

	return m_pPipeLine->Get_TransformMatrix(eState);
}

_float4x4 CGameInstance::Get_TransformFloat4x4(CPipeLine::D3DTRANSFORMSTATE eState)
{
	NULL_CHECK_RETURN(m_pPipeLine, _float4x4());

	return m_pPipeLine->Get_TransformFloat4x4(eState);
}

_matrix CGameInstance::Get_TransformMatrixInverse(CPipeLine::D3DTRANSFORMSTATE eState)
{
	NULL_CHECK_RETURN(m_pPipeLine, XMMatrixIdentity());

	return m_pPipeLine->Get_TransformMatrixInverse(eState);
}

_float4x4 CGameInstance::Get_TransformFloat4x4Inverse(CPipeLine::D3DTRANSFORMSTATE eState)
{
	NULL_CHECK_RETURN(m_pPipeLine, _float4x4());

	return m_pPipeLine->Get_TransformFloat4x4Inverse(eState);
}

_float4 CGameInstance::Get_CamPosition()
{
	if (nullptr == m_pPipeLine)
		return _float4();

	return m_pPipeLine->Get_CamPosition();
}

_float4 CGameInstance::Get_CamDirection()
{
	if (nullptr == m_pPipeLine)
		return _float4();

	return m_pPipeLine->Get_CamDirection();
}

_float4 CGameInstance::Get_CamSetting()
{
	if (nullptr == m_pPipeLine)
		return _float4();

	return m_pPipeLine->Get_CamSetting();
}

_float CGameInstance::Get_CamFar()
{
	if (nullptr == m_pPipeLine)
		return _float();

	return m_pPipeLine->Get_CamFar();
}

_float4x4* CGameInstance::Get_Shadow_Proj()
{
	if (nullptr == m_pPipeLine)
		return nullptr;

	return m_pPipeLine->Get_ShadowProj();
}

void CGameInstance::Set_ShadowProj(_float4x4* pMatrix)
{
	if (nullptr == m_pPipeLine)
		return;

	return m_pPipeLine->Set_ShadowProj(pMatrix);
}

void CGameInstance::Set_CascadeBoxes(BoundingOrientedBox* pBoxes)
{
	if (nullptr == m_pPipeLine)
		return;

	return m_pPipeLine->Set_CascadeBoxes(pBoxes);
}


RAY CGameInstance::Get_MouseRayWorld(HWND g_hWnd, const unsigned int g_iWinSizeX, const unsigned int g_iWinSizeY)
{
	//return m_pPipeLine->Get_MouseRayWorld(g_hWnd, g_iWinSizeX, g_iWinSizeY);
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	_vector vProjPos(XMVectorSet(pt.x / (g_iWinSizeX * 0.5f) - 1.f, pt.y / -(g_iWinSizeY * 0.5f) + 1.f, 0.f, 0.f));

	_matrix ProjMatrixInv = Get_TransformMatrixInverse(CPipeLine::D3DTRANSFORMSTATE::D3DTS_PROJ);

	_vector vViewPos(XMVector3TransformCoord(vProjPos, ProjMatrixInv));

	_vector vRayDir(vViewPos);
	_vector vRayPos(XMVectorSet(0.f, 0.f, 0.f, 1.f));

	_matrix ViewMatrixInv = Get_TransformMatrixInverse(CPipeLine::D3DTRANSFORMSTATE::D3DTS_VIEW);

	RAY MouseRay;
	ZeroMemory(&MouseRay, sizeof(RAY));

	XMStoreFloat3(&MouseRay.vDirection, XMVector3Normalize(XMVector3TransformNormal(vRayDir, ViewMatrixInv)));
	XMStoreFloat4(&MouseRay.vPosition, XMVector3TransformCoord(vRayPos, ViewMatrixInv));

	MouseRay.fLength = 1000000.0f;
	return MouseRay;
}

RAY CGameInstance::Get_MouseRayLocal(HWND g_hWnd, const unsigned int	g_iWinSizeX, const unsigned int	g_iWinSizeY, _matrix matWorld)
{
	//return m_pPipeLine->Get_MouseRayLocal(g_hWnd, g_iWinSizeX, g_iWinSizeY, matWorld);

	RAY vMouseRayLocal;
	ZeroMemory(&vMouseRayLocal, sizeof(vMouseRayLocal));

	RAY vMouseRayWorld = Get_MouseRayWorld(g_hWnd, g_iWinSizeX, g_iWinSizeY);

	_vector vMousePos = XMLoadFloat4(&vMouseRayWorld.vPosition);
	_vector vMouseDir = XMLoadFloat3(&vMouseRayWorld.vDirection);

	_matrix matWorldInv = XMMatrixInverse(nullptr, matWorld);
	vMousePos = XMVector3TransformCoord(vMousePos, matWorldInv);
	vMouseDir = XMVector3Normalize(XMVector3TransformNormal(vMouseDir, matWorldInv));

	XMStoreFloat4(&vMouseRayLocal.vPosition, vMousePos);
	XMStoreFloat3(&vMouseRayLocal.vDirection, vMouseDir);
	vMouseRayLocal.fLength = 1000000.0f;

	return vMouseRayLocal;
}

#ifdef _DEBUG
_bool CGameInstance::Picking_Mesh(RAY ray, _float3* out, vector<class CMesh*> Meshes)
{
	//_vector		vPickedPos;
	//_vector		vVec0, vVec1, vVec2;

	_vector		vRayPos = XMLoadFloat4(&ray.vPosition);
	_vector		vRayDir = XMLoadFloat3(&ray.vDirection);
	_float		fMinDist = 100000.f;

	_bool bIsPicked = false;
	_float3 vPickedPos = { 0.f, 0.f, 0.f };

	for (CMesh* mesh : Meshes)
	{
		
		if (nullptr == mesh)
			return false;

		if (mesh->Picking(ray, out))
		{
			bIsPicked = true;

			_vector vPos = XMLoadFloat3(out);
			_float3 rayPos = XMLoadFloat4(&ray.vPosition);

			rayPos.y += 2;

			_float3 vDist = rayPos - *out;
			//XMStoreFloat3(&vDist, vRayPos - vPos);

			_float fDist = sqrt(vDist.x * vDist.x + vDist.y * vDist.y + vDist.z * vDist.z);


			if (fMinDist >= fDist)
			{
				fMinDist = fDist;
				vPickedPos = *out;

				
			}
		}
	}

	//if (bIsPicked)
	//{
	//	*out = vPickedPos;
	//}

	*out = vPickedPos;

	return bIsPicked;
}
#endif

#ifdef _DEBUG
_bool CGameInstance::Picking_Vertex(RAY ray, _float3* out, _uint triNum, VTXMESH* pVertices, _uint* pIndices)
{
	_vector		vPickedPos;
	_vector		vVec0, vVec1, vVec2;

	_vector		vRayPos = XMLoadFloat4(&ray.vPosition);
	_vector		vRayDir = XMLoadFloat3(&ray.vDirection);
	_float		fDist = 0.f;

	for (_uint i = 0; i < triNum; ++i)
	{
		_int iStartIndex = i * 3;

		_float3 vVtxPos1 = pVertices[pIndices[iStartIndex + 0]].vPosition;
		_float3 vVtxPos2 = pVertices[pIndices[iStartIndex + 1]].vPosition;
		_float3 vVtxPos3 = pVertices[pIndices[iStartIndex + 2]].vPosition;

		vVec0 = XMLoadFloat3(&vVtxPos1);
		vVec1 = XMLoadFloat3(&vVtxPos2);
		vVec2 = XMLoadFloat3(&vVtxPos3);

		if (DirectX::TriangleTests::Intersects(
			vRayPos, vRayDir,
			vVec0, vVec1, vVec2,
			fDist))
		{
			vPickedPos = vRayPos + XMVector3Normalize(vRayDir) * fDist;
			XMStoreFloat3(out, vPickedPos);
			return true;
		}
	}
	return false;
}
#endif

HRESULT CGameInstance::Add_Font(const wstring & strFontTag, const wstring & strFontFilePath)
{
	NULL_CHECK_RETURN(m_pFont_Manager, E_FAIL);
	return m_pFont_Manager->Add_Font(strFontTag, strFontFilePath);
}

HRESULT CGameInstance::Render_Font(const wstring & strFontTag, const wstring & strText, const _float2 & vPosition, _fvector vColor, _float fScale, _float2 vOrigin, _float fRotation)
{
	NULL_CHECK_RETURN(m_pFont_Manager, E_FAIL);
	return m_pFont_Manager->Render(strFontTag, strText, vPosition, vColor, fScale, vOrigin, fRotation);
}

HRESULT CGameInstance::Add_RenderTarget(const wstring & strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4 & vClearColor)
{
	NULL_CHECK_RETURN(m_pTarget_Manager, E_FAIL);
	return m_pTarget_Manager->Add_RenderTarget(strTargetTag, iSizeX, iSizeY, ePixelFormat, vClearColor);
}

HRESULT CGameInstance::Add_MRT(const wstring & strMRTTag, const wstring & strTargetTag)
{
	NULL_CHECK_RETURN(m_pTarget_Manager, E_FAIL);
	return m_pTarget_Manager->Add_MRT(strMRTTag, strTargetTag);
}

HRESULT CGameInstance::Begin_MRT(const wstring & strMRTTag, ID3D11DepthStencilView* pDSV, _bool bClear)
{
	NULL_CHECK_RETURN(m_pTarget_Manager, E_FAIL);

	return m_pTarget_Manager->Begin_MRT(strMRTTag, pDSV, bClear);
}

HRESULT CGameInstance::End_MRT()
{
	NULL_CHECK_RETURN(m_pTarget_Manager, E_FAIL);
	return m_pTarget_Manager->End_MRT();
}

HRESULT CGameInstance::Clear_MRT(const wstring& strMRTTag)
{
	NULL_CHECK_RETURN(m_pTarget_Manager, E_FAIL);
	return m_pTarget_Manager->Clear_MRT(strMRTTag);
}

HRESULT CGameInstance::Clear_Target(const wstring& strMRTTag, const wstring& strTargetTag)
{
	NULL_CHECK_RETURN(m_pTarget_Manager, E_FAIL);
	return m_pTarget_Manager->Clear_Target(strMRTTag, strTargetTag);
}

HRESULT CGameInstance::Bind_RenderTarget_ShaderResource(const wstring & strTargetTag, CShader * pShader, const _char * pConstantName)
{
	return m_pTarget_Manager->Bind_ShaderResource(strTargetTag, pShader, pConstantName);
}

CRenderTarget* CGameInstance::Find_RenderTarget(const wstring& strTargetTag)
{
	return m_pTarget_Manager->Find_RenderTarget(strTargetTag);
}

void CGameInstance::Create_RenderTarget(const wstring& strTargetTag)
{
	m_pTarget_Manager->Create_RenderTarget(strTargetTag);
}

#ifdef _DEBUG

HRESULT CGameInstance::Ready_RenderTarget_Debug(const wstring & strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	return m_pTarget_Manager->Ready_Debug(strTargetTag, fX, fY, fSizeX, fSizeY);
}
HRESULT CGameInstance::Render_Debug_RTVs(const wstring & strMRTTag, CShader * pShader, CVIBuffer_Rect * pVIBuffer)
{
	return m_pTarget_Manager->Render_Debug(strMRTTag, pShader, pVIBuffer);
}

#endif

HRESULT CGameInstance::Add_Light(const LIGHT_DESC& LightDesc, _int& outLightIndex)
{
	NULL_CHECK_RETURN(m_pLight_Manager, E_FAIL);

	return m_pLight_Manager->Add_Light(LightDesc, outLightIndex);
}

CLight* CGameInstance::Add_Light_AndGet(const LIGHT_DESC& LightDesc, _uint& outLightIndex)
{
	if(m_pLight_Manager == nullptr)
		return nullptr;

	return m_pLight_Manager->Add_Light_AndGet(LightDesc, outLightIndex);
}

CLight* CGameInstance::Find_Light(const _int iIndex)
{
	NULL_CHECK_RETURN(m_pLight_Manager, nullptr);

	return m_pLight_Manager->Find_Light(iIndex);
}

void CGameInstance::Change_Light_Desc(const _int iIndex, LIGHT_DESC newDesc)
{
	NULL_CHECK(m_pLight_Manager);

	return m_pLight_Manager->Change_Light_Desc(iIndex, newDesc);
}

CLight* CGameInstance::Get_DirectionLight()
{
	NULL_CHECK_RETURN(m_pLight_Manager, nullptr);

	return m_pLight_Manager->Get_DirectionLight();
}

CLight_Manager* CGameInstance::Get_LightManager()
{
	NULL_CHECK_RETURN(m_pLight_Manager, nullptr);

	return m_pLight_Manager;
}

void CGameInstance::Get_AllLight(list<class CLight*>* pTemp)
{
	NULL_CHECK_RETURN(m_pLight_Manager, );

	return m_pLight_Manager->Get_AllLight(pTemp);
}

_float4x4 CGameInstance::Get_StaticLight()
{
	NULL_CHECK_RETURN(m_pLight_Manager, _float4x4());

	return m_pLight_Manager->Get_StaticLight();
}

HRESULT CGameInstance::Ready_StaticLightMatrix(_float3 vPos, _float3 vLook)
{
	NULL_CHECK_RETURN(m_pLight_Manager, E_FAIL);

	return m_pLight_Manager->Ready_StaticLightMatrix(vPos, vLook);
}

HRESULT CGameInstance::Render_Lights(CShader * pShader, CVIBuffer_Rect * pVIBuffer)
{
	NULL_CHECK_RETURN(m_pLight_Manager, E_FAIL);

	return m_pLight_Manager->Render(pShader, pVIBuffer);
}

_bool CGameInstance::Remove_Light(const _uint& iIndex)
{
	NULL_CHECK_RETURN(m_pLight_Manager, false);

	return m_pLight_Manager->Remove_Light(iIndex);
}

_bool CGameInstance::Remove_AllLight()
{
	NULL_CHECK_RETURN(m_pLight_Manager, false);

	return m_pLight_Manager->Remove_AllLight();
}

HRESULT CGameInstance::Set_ShadowLight(_uint iLevelIndex, _float4 vEye, _float4 vAt, _float4 vUp)
{
	NULL_CHECK_RETURN(m_pLight_Manager, E_FAIL);
	return m_pLight_Manager->Set_ShadowLight(iLevelIndex, vEye, vAt, vUp);
}

HRESULT CGameInstance::Add_ShadowLight_View(_uint iLevelIndex, _vector vEye, _vector vAt, _vector vUp)
{
	NULL_CHECK_RETURN(m_pLight_Manager, E_FAIL);
	return m_pLight_Manager->Add_ShadowLight_View(iLevelIndex, vEye, vAt, vUp);
}

HRESULT CGameInstance::Add_ShadowLight_Proj(_uint iLevelIndex, _float fFovAngleY, _float fAspectRatio, _float fNearZ, _float fFarZ)
{
	NULL_CHECK_RETURN(m_pLight_Manager, E_FAIL);
	return m_pLight_Manager->Add_ShadowLight_Proj(iLevelIndex, fFovAngleY, fAspectRatio, fNearZ, fFarZ);
}

_float4x4 CGameInstance::Get_ShadowLightViewMatrix(_uint iLevelIndex)
{
	NULL_CHECK_RETURN(m_pLight_Manager, _float4x4());
	return m_pLight_Manager->Get_ShadowLightViewMatrix(iLevelIndex);
}

_float4x4 CGameInstance::Get_ShadowLightViewMatrix_Inverse(_uint iLevelIndex)
{
	NULL_CHECK_RETURN(m_pLight_Manager, _float4x4());
	return m_pLight_Manager->Get_ShadowLightViewMatrix_Inverse(iLevelIndex);
}

_float4x4 CGameInstance::Get_ShadowLightProjMatrix(_uint iLevelIndex)
{
	NULL_CHECK_RETURN(m_pLight_Manager, _float4x4());
	return m_pLight_Manager->Get_ShadowLightProjMatrix(iLevelIndex);
}

_float CGameInstance::Get_ShadowLightFar(_uint iLevelIndex)
{
	NULL_CHECK_RETURN(m_pLight_Manager, _float());
	return m_pLight_Manager->Get_ShadowLightFar(iLevelIndex);
}

_float4 CGameInstance::Get_ShadowLightPos(_uint iLevelIndex)
{
	NULL_CHECK_RETURN(m_pLight_Manager, _float4());
	return m_pLight_Manager->Get_ShadowLightPos(iLevelIndex);
}

void CGameInstance::Transform_Frustum_ToLocalSpace(_fmatrix WorldMatrix)
{
	NULL_CHECK_RETURN(m_pFrustum, );

	return m_pFrustum->Transform_ToLocalSpace(WorldMatrix);
}

_bool CGameInstance::isIn_WorldPlanes(_fvector vPoint, _float fRadius)
{
	NULL_CHECK_RETURN(m_pFrustum, false);

	return m_pFrustum->isIn_WorldPlanes(vPoint, fRadius);
}

_bool CGameInstance::isIn_LocalPlanes(_fvector vPoint, _float fRadius)
{
	NULL_CHECK_RETURN(m_pFrustum, false);

	return m_pFrustum->isIn_LocalPlanes(vPoint, fRadius);
}

void CGameInstance::Add_Collision(const _uint& In_iLayer, CCollider* _pCollider)
{
	NULL_CHECK_RETURN(m_pCollision_Manager, );

	m_pCollision_Manager->Add_Collision(In_iLayer, _pCollider);
}

void CGameInstance::Check_Group(const _uint& In_iLeftLayer, const _uint& In_iRightLayer)
{
	NULL_CHECK_RETURN(m_pCollision_Manager, );

	m_pCollision_Manager->Check_Group(In_iLeftLayer, In_iRightLayer);
}

void CGameInstance::Add_Event(IEvent* pEvent)
{
	NULL_CHECK_RETURN(m_pEvent_Manager, );

	m_pEvent_Manager->Add_Event(pEvent);
}

void CGameInstance::Clear_Event()
{
//	NULL_CHECK_RETURN(m_pEvent_Manager, );
	if (nullptr == m_pEvent_Manager)
		return;

	m_pEvent_Manager->Clear();
}

void CGameInstance::Register_PhysXCollider(CPhysXCollider* pPhysXCollider)
{
	NULL_CHECK_RETURN(m_pPhysX_Manager, );

	m_pPhysX_Manager->Register_PhysXCollider(pPhysXCollider);
}

CPhysXCollider* CGameInstance::Find_PhysXCollider(const _uint iPhysXColliderIndex)
{
	NULL_CHECK_RETURN(m_pPhysX_Manager, nullptr );

	return m_pPhysX_Manager->Find_PhysXCollider(iPhysXColliderIndex);
}

void CGameInstance::Register_PhysXController(CPhysXController* pPhysXController)
{
	NULL_CHECK_RETURN(m_pPhysX_Manager, );

	m_pPhysX_Manager->Register_PhysXController(pPhysXController);
}

CPhysXController* CGameInstance::Find_PhysXController(const _uint iPhysXControllerIndex)
{
	NULL_CHECK_RETURN(m_pPhysX_Manager, nullptr);

	return m_pPhysX_Manager->Find_PhysXController(iPhysXControllerIndex);
}

void CGameInstance::Check_PhysXFilterGroup(const _uint In_iLeftLayer, const _uint In_iRightLayer)
{
	NULL_CHECK_RETURN(m_pPhysX_Manager, );

	m_pPhysX_Manager->Check_PhysXFilterGroup(In_iLeftLayer, In_iRightLayer);
}

_uint CGameInstance::Get_PhysXFilterGroup(const _uint In_iIndex)
{
	NULL_CHECK_RETURN(m_pPhysX_Manager, 0);

	return m_pPhysX_Manager->Get_PhysXFilterGroup(In_iIndex);
}

PxRigidDynamic* CGameInstance::Create_DynamicActor(const PxTransform& transform, const PxGeometry& geometry, PxMaterial* pMaterial)
{
	NULL_CHECK_RETURN(m_pPhysX_Manager, nullptr);
	return m_pPhysX_Manager->Create_DynamicActor(transform, geometry, pMaterial);
}

PxRigidDynamic* CGameInstance::Create_DynamicActor(const PxTransform& transform)
{
	NULL_CHECK_RETURN(m_pPhysX_Manager, nullptr);
	return m_pPhysX_Manager->Create_DynamicActor(transform);
}

PxRigidStatic* CGameInstance::Create_StaticActor(const PxTransform& transform, const PxGeometry& geometry, PxMaterial* pMaterial)
{
	NULL_CHECK_RETURN(m_pPhysX_Manager, nullptr);
	return m_pPhysX_Manager->Create_StaticActor(transform, geometry, pMaterial);
}

PxRigidStatic* CGameInstance::Create_StaticActor(const PxTransform& transform)
{
	NULL_CHECK_RETURN(m_pPhysX_Manager, nullptr);
	return m_pPhysX_Manager->Create_StaticActor(transform);
}

void CGameInstance::Add_DynamicActorAtCurrentScene(PxRigidDynamic& DynamicActor)
{
	NULL_CHECK_RETURN(m_pPhysX_Manager, );
	m_pPhysX_Manager->Add_DynamicActorAtCurrentScene(DynamicActor);
}

void CGameInstance::Add_StaticActorAtCurrentScene(PxRigidStatic& StaticActor)
{
	NULL_CHECK_RETURN(m_pPhysX_Manager, );
	m_pPhysX_Manager->Add_StaticActorAtCurrentScene(StaticActor);
}

void CGameInstance::Create_Material(_float fStaticFriction, _float fDynamicFriction, _float fRestitution, PxMaterial** ppOut)
{
	NULL_CHECK_RETURN(m_pPhysX_Manager, );
	m_pPhysX_Manager->Create_Material(fStaticFriction, fDynamicFriction, fRestitution, ppOut);
}

void CGameInstance::Create_ConvexMesh(PxVec3** pVertices, _uint iNumVertice, PxConvexMesh** ppOut)
{
	NULL_CHECK_RETURN(m_pPhysX_Manager, );
	m_pPhysX_Manager->Create_ConvexMesh(pVertices, iNumVertice, ppOut);
}

void CGameInstance::Create_ConvexMesh(const PxConvexMeshDesc& In_MeshDesc, PxConvexMesh** ppOut)
{
	NULL_CHECK_RETURN(m_pPhysX_Manager, );
	m_pPhysX_Manager->Create_ConvexMesh(In_MeshDesc, ppOut);
}

void CGameInstance::Create_Shape(const PxGeometry& Geometry, PxMaterial* pMaterial, const _bool isExculsive, const PxShapeFlags In_ShapeFlags, PxShape** ppOut)
{
	NULL_CHECK_RETURN(m_pPhysX_Manager, );
	m_pPhysX_Manager->Create_Shape(Geometry, pMaterial, isExculsive, In_ShapeFlags, ppOut);
}

void CGameInstance::Create_MeshFromTriangles(const PxTriangleMeshDesc& In_MeshDesc, PxTriangleMesh** ppOut)
{
	NULL_CHECK_RETURN(m_pPhysX_Manager, );
	m_pPhysX_Manager->Create_MeshFromTriangles(In_MeshDesc, ppOut);
}

void CGameInstance::Create_Controller(const PxCapsuleControllerDesc& In_ControllerDesc, PxController** ppOut)
{
	NULL_CHECK_RETURN(m_pPhysX_Manager, );
	m_pPhysX_Manager->Create_Controller(In_ControllerDesc, ppOut);
}

const _float& CGameInstance::Random_Float(_float fMin, _float fMax)
{
	return m_pRandom_Manager->Random_Float(fMin, fMax);
}

const _int& CGameInstance::Random_Int(_int iMin, _int iMax)
{
	return m_pRandom_Manager->Random_Int(iMin, iMax);
}

const _bool& CGameInstance::Random_Coin(_float fProbality)
{
	return m_pRandom_Manager->Random_Coin(fProbality);
}

int64_t CGameInstance::GenerateUniqueID()
{
	return m_pRandom_Manager->GenerateUniqueID();
}

void CGameInstance::Play_Sound(const wstring& strGroupKey, const wstring& strSoundKey, CHANNELID eID, float fVolume)
{
	if (true == m_bUseSoundManager)
		m_pSoundManager->Play_Sound(strGroupKey, strSoundKey, eID, fVolume);
}

void CGameInstance::Play_BGM(const wstring& strGroupKey, const wstring& strSoundKey, float fVolume)
{
	if (true == m_bUseSoundManager)
		m_pSoundManager->Play_BGM(strGroupKey, strSoundKey, fVolume);
}

void CGameInstance::Stop_Sound(CHANNELID eID)
{
	if (true == m_bUseSoundManager)
		m_pSoundManager->Stop_Sound(eID);
}

void CGameInstance::Stop_All()
{
	if (true == m_bUseSoundManager)
		m_pSoundManager->Stop_All();
}

void CGameInstance::Set_ChannelVolume(CHANNELID eID, float fVolume)
{
	if (true == m_bUseSoundManager)
		m_pSoundManager->Set_ChannelVolume(eID, fVolume);
}

void CGameInstance::String_To_WString(string _string, wstring& _wstring)
{
	//std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> converter;
	//_wstring = converter.from_bytes(_string);

	//wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
	//_wstring = converter.from_bytes(_string);

	//_wstring.assign(_string.begin(), _string.end());

	//TODO C++ 17로 올리니 기존 Convert 함수들은 더 이상 지원하지 않아. window api에서 제공하는 변환 함수 사용으로 변경 - TO 승용

	int len = MultiByteToWideChar(CP_UTF8, 0, _string.c_str(), -1, nullptr, 0);
	if (len > 0) {
		_wstring.resize(len - 1);
		MultiByteToWideChar(CP_UTF8, 0, _string.c_str(), -1, &_wstring[0], len);
	}
}

void CGameInstance::WString_To_String(wstring _wstring, string& _string)
{
	//std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> converter;
	//_string = converter.to_bytes(_wstring);

	//wstring_convert<codecvt_utf8<wchar_t>> converter;
	//_string = converter.to_bytes(_wstring);
//	_string.assign(_wstring.begin(), _wstring.end());
	//TODO C++ 17로 올리니 기존 Convert 함수들은 더 이상 지원하지 않아. window api에서 제공하는 변환 함수 사용으로 변경 - TO 승용

	int len = WideCharToMultiByte(CP_UTF8, 0, _wstring.c_str(), -1, nullptr, 0, nullptr, nullptr);
	if (len > 0) 
	{
		_string.resize(len - 1);
		WideCharToMultiByte(CP_UTF8, 0, _wstring.c_str(), -1, &_string[0], len, nullptr, nullptr);
	}
}

string CGameInstance::Convert_WString_To_String(wstring _wstring)
{
	int len;
	int slength = (int)_wstring.length() + 1;
	len = WideCharToMultiByte(CP_ACP, 0, _wstring.c_str(), slength, 0, 0, 0, 0);
	std::string r(len, '\0');
	WideCharToMultiByte(CP_ACP, 0, _wstring.c_str(), slength, &r[0], len, 0, 0);
	return r;
	//string out_string;
	//
	//return out_string.assign(_wstring.begin(), _wstring.end());
}

WCHAR* CGameInstance::StringTowchar(const std::string& str)
{
	int len;
	int slength = (int)str.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;

	return const_cast<WCHAR*>(r.c_str());

	// std::wstring으로 변환
	//std::wstring wstr(str.begin(), str.end());
	//// c_str() 함수를 사용하여 WCHAR* 포인터로 변환
	//
	//return const_cast<WCHAR*>(wstr.c_str());
}

char* CGameInstance::ConverWStringtoC(const wstring& wstr)
{
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
	char* result = new char[size_needed];
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, result, size_needed, NULL, NULL);

	char* newResult = result;

	result = nullptr;
	delete[] result;

	return newResult;
}

wchar_t* CGameInstance::ConverCtoWC(char* str)
{
	_tchar* pStr;
	int strSize = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, NULL);
	pStr = new WCHAR[strSize];
	MultiByteToWideChar(CP_ACP, 0, str, (_int)strlen(str) + (size_t)1, pStr, strSize);


	_tchar* newResult = pStr;

	pStr = nullptr;
	delete[] pStr;

	return newResult;
}

std::string CGameInstance::WideStringToString(const wchar_t* wideStr)
{
	char ch[260];
	char DefChar = ' ';
	WideCharToMultiByte(CP_ACP, 0, wideStr, -1, ch, 260, &DefChar, NULL);

	string ss(ch);
	return ss;
	//// std::wstring으로부터 std::string으로 변환
	//std::wstring wstr(wideStr);
	//// std::string으로 변환
	//return std::string(wstr.begin(), wstr.end());
}

std::string CGameInstance::GetFileName(const std::string& filePath)
{
	size_t lastSlashPos = filePath.find_last_of("/");
	if (lastSlashPos != std::string::npos)
	{
		return filePath.substr(lastSlashPos + 1);
	}
	else
	{
		// 경로 구분자가 없을 경우 전체 경로를 반환
		return filePath;
	}
}

std::string CGameInstance::RemoveExtension(const std::string& filePath)
{
		size_t lastDotPos = filePath.find_last_of(".");
	if (lastDotPos != std::string::npos) 
	{
		return filePath.substr(0, lastDotPos);
	}
	else
	{
		// 확장자가 없는 경우 그대로 반환
		return filePath;
	}
}

_vector CGameInstance::Convert_Orthogonal(_vector vPosition)
{
	_float fPosX = vPosition.m128_f32[0] + g_iWinsizeX * 0.5f;
	_float fPosY = vPosition.m128_f32[1] + g_iWinsizeY * 0.5f;

	_vector vOrthoPos = { fPosX, fPosY, vPosition.m128_f32[2], vPosition.m128_f32[3] };
	return vOrthoPos;
}

string CGameInstance::Wstring_To_UTF8(const wstring& wstr)
{
	_int iUtf8Length = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
	if (iUtf8Length == 0)
	{
		return string();
	}

	string strUtf8(iUtf8Length + 1, 0);
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &strUtf8[0], iUtf8Length + 1, nullptr, nullptr);

	return strUtf8;
}

wstring CGameInstance::Char_To_Wstring(char* szChar)
{
	return wstring(szChar, szChar + strlen(szChar));
}

const wstring CGameInstance::Remove_LastNumChar(const wstring& str, const _uint& iNumCutCount)
{
	size_t iLength = str.length();

	if (iLength <= iNumCutCount)
		return L"";

	wstring res = str.substr(0, iLength - iNumCutCount);

	return res;
}

const string CGameInstance::Remove_LastNumChar(const string& str, const _uint& iNumCutCount)
{
	size_t iLength = str.length();

	if (iLength <= iNumCutCount)
		return "";

	string res = str.substr(0, iLength - iNumCutCount);

	return res;
}

const wstring CGameInstance::Get_LastNumChar(const wstring& str, const _uint& iNumCutCount)
{
	size_t iLength = str.length();

	if (iLength <= iNumCutCount)
		return str;

	wstring res = str.substr(iLength - iNumCutCount);

	return res;
}

void CGameInstance::Get_ModelTag(vector<string>* pVector)
{
	NULL_CHECK_RETURN(m_pComponent_Manager, );

	m_pComponent_Manager->Get_ModelTag(pVector);
}

void CGameInstance::Update_Hitlag(_float fTimeDelta)
{
	m_fTimeDelta = fTimeDelta;

	if (0 < m_fHitlag_Time)
	{
		m_fTimeDelta /= 5;
		m_fHitlag_Time -= fTimeDelta;
	}
	else
	{
		m_fHitlag_Time = 0;
	}
}

void CGameInstance::Update_RadialBlurTime(_float fTimeDelta)
{
	m_fRadialBlurTime = m_fRadialBlurTime - fTimeDelta > 0 ? m_fRadialBlurTime - fTimeDelta : 0.f;

	_bool bIsActivateRadialBlur = 0 < m_fRadialBlurTime;
	Get_Renderer()->Set_Radial_Blur_Active(bIsActivateRadialBlur);
}

void CGameInstance::Update_ChromaTime(_float fTimeDelta)
{
	m_fChromaTime = m_fChromaTime - fTimeDelta > 0 ? m_fChromaTime - fTimeDelta : 0.f;

	_bool bIsActivateChroma = 0 < m_fChromaTime;

	Get_Renderer()->Set_Chroma_Active(bIsActivateChroma);
}

wstring CGameInstance::SliceObjectTag(const wstring& strObjectTag) //! 마지막 _ 기준으로 잘라서 오브젝트 이름만 가져오자 - TO 승용
{
	size_t pos = strObjectTag.rfind(L"_");
	if (pos != wstring::npos)
	{
		return strObjectTag.substr(pos + 1);
	}

	return {};
}

void CGameInstance::Release_Manager()
{
	//Safe_Release(m_pSoundManager);
	Safe_Release(m_pRandom_Manager);
	Safe_Release(m_pPhysX_Manager);
	Safe_Release(m_pEvent_Manager);
	Safe_Release(m_pCollision_Manager);
	Safe_Release(m_pFrustum);
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pGraphic_Device);
}

void CGameInstance::Release_Engine()
{
	CGameInstance::GetInstance()->Release_Manager();
	CGameInstance::GetInstance()->DestroyInstance();
}
