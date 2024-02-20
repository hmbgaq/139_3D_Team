#include "..\Public\GameInstance.h"
#include "Collision_Manager.h"
#include "Event_Manager.h"
#include "Graphic_Device.h"
#include "Object_Manager.h"
#include "Target_Manager.h"
#include "TImer_Manager.h"
#include "Level_Manager.h"
#include "Input_Device.h"
#include "Light_Manager.h"
#include "Font_Manager.h"
#include "Renderer.h"
#include "Frustum.h"
#include "Mesh.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{
}

HRESULT CGameInstance::Initialize_Engine(_uint iNumLevels, _uint iNumLayer, HINSTANCE hInstance, const GRAPHIC_DESC& GraphicDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext)
{
	/* 그래픽 디바이스를 초기화 하자.*/
	m_pGraphic_Device = CGraphic_Device::Create(GraphicDesc, ppDevice, ppContext);
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pInput_Device = CInput_Device::Create(hInstance, GraphicDesc.hWnd);
	if (nullptr == m_pInput_Device)
		return E_FAIL;

	/* 타이머를 사용할 준비를 하자. */
	m_pTimer_Manager = CTimer_Manager::Create();
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	m_pLevel_Manager = CLevel_Manager::Create();
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	m_pObject_Manager = CObject_Manager::Create(iNumLevels);
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	m_pComponent_Manager = CComponent_Manager::Create(iNumLevels);
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	m_pFont_Manager = CFont_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	m_pTarget_Manager = CTarget_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	m_pRenderer = CRenderer::Create(*ppDevice, *ppContext);
	if (nullptr == m_pRenderer)
		return E_FAIL;

	m_pPipeLine = CPipeLine::Create();
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	m_pLight_Manager = CLight_Manager::Create();
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	m_pFrustum = CFrustum::Create();
	if (nullptr == m_pFrustum)
		return E_FAIL;

	m_pCollision_Manager = CCollision_Manager::Create(iNumLayer);
	if (nullptr == m_pCollision_Manager)
		return E_FAIL;

	m_pEvent_Manager = CEvent_Manager::Create();
	if (nullptr == m_pEvent_Manager)
		return E_FAIL;


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

	m_pInput_Device->Tick();

	m_pObject_Manager->Priority_Tick(fTimeDelta);

	m_pObject_Manager->Tick(fTimeDelta);

	m_pPipeLine->Tick();

	m_pFrustum->Tick();

	m_pEvent_Manager->Tick(fTimeDelta);

	m_pCollision_Manager->Tick(fTimeDelta);

	m_pObject_Manager->Late_Tick(fTimeDelta);

	m_pLevel_Manager->Tick(fTimeDelta);
}

void CGameInstance::Clear(_uint iLevelIndex)
{
	if (nullptr == m_pObject_Manager ||
		nullptr == m_pComponent_Manager || 
		nullptr == m_pEvent_Manager)
		return;

	/* 오브젝트 매니져에 레벨별로 구분해 놓은 객체들 중 특정된 객체들을 지운다.  */
	m_pObject_Manager->Clear(iLevelIndex);

	/* 컴포넌트 매니져에 레벨별로 구분해 놓은 컴포넌트들 중 특정된 객체들을 지운다.  */
	m_pComponent_Manager->Clear(iLevelIndex);

	m_pEvent_Manager->Clear();
}

HRESULT CGameInstance::Render_Engine()
{
	if (nullptr == m_pLevel_Manager ||
		nullptr == m_pRenderer)
		return E_FAIL;

	m_pRenderer->Pre_Setting();

	m_pRenderer->Draw_RenderGroup();


#ifdef _DEBUG
	m_pLevel_Manager->Render();
#endif

	m_pInput_Device->LateTick();
	
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

void CGameInstance::Set_CurrentLevel(_uint CurrentLevel)
{
	m_pLevel_Manager->Set_CurrentLevel(CurrentLevel);
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
	if (FAILED(Add_CloneObject(iLevelIndex, strLayerTag, strPrototypeTag, pArg)))
		return nullptr;

	return Get_GameObect_Last(iLevelIndex, strLayerTag);
}

CCharacter* CGameInstance::Get_Player()
{
	return m_pObject_Manager->Get_Player();
}

void CGameInstance::Set_Player(CCharacter* _pPlayer)
{
	m_pObject_Manager->Set_Player(_pPlayer);
}

HRESULT CGameInstance::Create_PoolObjects(const wstring& strPrototypeTag, _uint iSize)
{
	return m_pObject_Manager->Create_PoolObjects(strPrototypeTag, iSize);
}

void CGameInstance::Fill_PrototypeTags(vector<string>* _vector)
{
	m_pObject_Manager->Fill_PrototypeTags(_vector);
}


HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const wstring & strPrototypeTag, CComponent * pPrototype)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;


	return m_pComponent_Manager->Add_Prototype(iLevelIndex, strPrototypeTag, pPrototype);
}

CComponent * CGameInstance::Clone_Component(_uint iLevelIndex, const wstring & strPrototypeTag, void * pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Clone_Component(iLevelIndex, strPrototypeTag, pArg);
}

HRESULT CGameInstance::Add_RenderGroup(CRenderer::RENDERGROUP eGroupID, CGameObject * pGameObject)
{
	if (nullptr == m_pRenderer)
		return E_FAIL;

	return m_pRenderer->Add_RenderGroup(eGroupID, pGameObject);
}

HRESULT CGameInstance::Add_DebugRender(CComponent * pDebugCom)
{
	if (nullptr == m_pRenderer)
		return E_FAIL;

	return m_pRenderer->Add_DebugRender(pDebugCom);
}


#ifdef _DEBUG
void CGameInstance::Set_RenderDebug(_bool _bRenderDebug)
{
	m_pRenderer->Set_RenderDebug(_bRenderDebug);
}
#endif


void CGameInstance::Set_Transform(CPipeLine::D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix)
{
	if (nullptr == m_pPipeLine)
		return;

	m_pPipeLine->Set_Transform(eState, TransformMatrix);
}

void CGameInstance::Set_Transform(CPipeLine::D3DTRANSFORMSTATE eState, _float4x4 TransformMatrix)
{
	if (nullptr == m_pPipeLine)
		return;

	m_pPipeLine->Set_Transform(eState, TransformMatrix);
}

_matrix CGameInstance::Get_TransformMatrix(CPipeLine::D3DTRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_TransformMatrix(eState);
}

_float4x4 CGameInstance::Get_TransformFloat4x4(CPipeLine::D3DTRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_TransformFloat4x4(eState);
}

_matrix CGameInstance::Get_TransformMatrixInverse(CPipeLine::D3DTRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_TransformMatrixInverse(eState);
}

_float4x4 CGameInstance::Get_TransformFloat4x4Inverse(CPipeLine::D3DTRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_TransformFloat4x4Inverse(eState);
}

_float4 CGameInstance::Get_CamPosition()
{
	if (nullptr == m_pPipeLine)
		return _float4();

	return m_pPipeLine->Get_CamPosition();
}

_float4 CGameInstance::Get_CamSetting()
{
	if (nullptr == m_pPipeLine)
		return _float4();

	return m_pPipeLine->Get_CamSetting();
}

_float CGameInstance::Get_CamFar()
{
	return m_pPipeLine->Get_CamFar();
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
	return m_pFont_Manager->Add_Font(strFontTag, strFontFilePath);
}

HRESULT CGameInstance::Render_Font(const wstring & strFontTag, const wstring & strText, const _float2 & vPosition, _fvector vColor, _float fScale, _float2 vOrigin, _float fRotation)
{
	return m_pFont_Manager->Render(strFontTag, strText, vPosition, vColor, fScale, vOrigin, fRotation);
}

HRESULT CGameInstance::Add_RenderTarget(const wstring & strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4 & vClearColor)
{
	return m_pTarget_Manager->Add_RenderTarget(strTargetTag, iSizeX, iSizeY, ePixelFormat, vClearColor);
}

HRESULT CGameInstance::Add_MRT(const wstring & strMRTTag, const wstring & strTargetTag)
{
	return m_pTarget_Manager->Add_MRT(strMRTTag, strTargetTag);
}

HRESULT CGameInstance::Begin_MRT(const wstring & strMRTTag, ID3D11DepthStencilView* pDSV, _bool bClear)
{
	return m_pTarget_Manager->Begin_MRT(strMRTTag, pDSV, bClear);
}

HRESULT CGameInstance::End_MRT()
{
	return m_pTarget_Manager->End_MRT();
}

HRESULT CGameInstance::Bind_RenderTarget_ShaderResource(const wstring & strTargetTag, CShader * pShader, const _char * pConstantName)
{
	return m_pTarget_Manager->Bind_ShaderResource(strTargetTag, pShader, pConstantName);
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
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Add_Light(LightDesc, outLightIndex);
}

HRESULT CGameInstance::Render_Lights(CShader * pShader, CVIBuffer_Rect * pVIBuffer)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Render(pShader, pVIBuffer);
}

void CGameInstance::Transform_Frustum_ToLocalSpace(_fmatrix WorldMatrix)
{
	return m_pFrustum->Transform_ToLocalSpace(WorldMatrix);
}

_bool CGameInstance::isIn_WorldPlanes(_fvector vPoint, _float fRadius)
{
	return m_pFrustum->isIn_WorldPlanes(vPoint, fRadius);
}

_bool CGameInstance::isIn_LocalPlanes(_fvector vPoint, _float fRadius)
{

	return m_pFrustum->isIn_LocalPlanes(vPoint, fRadius);
}

void CGameInstance::Add_Collision(const _uint& In_iLayer, CCollider* _pCollider)
{
	m_pCollision_Manager->Add_Collision(In_iLayer, _pCollider);
}

void CGameInstance::Add_Event(IEvent* pEvent)
{
	m_pEvent_Manager->Add_Event(pEvent);
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
	string out_string;

	return out_string.assign(_wstring.begin(), _wstring.end());
}

WCHAR* CGameInstance::StringTowchar(const std::string& str)
{
	// std::wstring으로 변환
	std::wstring wstr(str.begin(), str.end());
	// c_str() 함수를 사용하여 WCHAR* 포인터로 변환

	return const_cast<WCHAR*>(wstr.c_str());
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
	// std::wstring으로부터 std::string으로 변환
	std::wstring wstr(wideStr);
	// std::string으로 변환
	return std::string(wstr.begin(), wstr.end());
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
	Safe_Release(m_pEvent_Manager);
	Safe_Release(m_pCollision_Manager);
	Safe_Release(m_pFrustum);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pLevel_Manager);
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
