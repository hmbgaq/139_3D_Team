#include "stdafx.h"
#include "UI_Video.h"
#include "GameInstance.h"
#include "Json_Utility.h"

#include "Data_Manager.h"
//#include <mfapi.h>
//#include <mfidl.h>
//#include <mfreadwrite.h>
//#include <mferror.h>

// #include <avformat.h>
//#include <libavformat/avformat.h>
//#include <libavcodec/avcodec.h>
//#include <libswresample/swresample.h>
//#include <libswscale/swscale.h>
//#include <libavutil/avutil.h>

CUI_Video::CUI_Video(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CUI(pDevice, pContext, strPrototypeTag)
{

}

CUI_Video::CUI_Video(const CUI_Video& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_Video::Initialize_Prototype()
{
	//TODO 원형객체의 초기화과정을 수행한다.
	/* 1.서버로부터 값을 받아와서 초기화한다 .*/
	/* 2.파일입출력을 통해 값을 받아온다.*/

	return S_OK;
}

HRESULT CUI_Video::Initialize(void* pArg)
{
	if (pArg != nullptr)
		m_tUIInfo = *(UI_DESC*)pArg;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(__super::Initialize(&m_tUIInfo))) //!  트랜스폼 셋팅, m_tUIInfo의 bWorldUI 가 false 인 경우에만 직교위치 셋팅
		return E_FAIL;

	LoadVideo(L"../Bin/Resources/Textures/UI/Video/MainMenu/ew_mainmenu_intro.avi");

	return S_OK;
}

void CUI_Video::Priority_Tick(_float fTimeDelta)
{

}

void CUI_Video::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);


}

void CUI_Video::Late_Tick(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this)))
		return;
}

HRESULT CUI_Video::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(ReadVideoFrame()))
		return E_FAIL;

	//! 이 셰이더에 0번째 패스로 그린다.
	m_pShaderCom->Begin(m_iShaderNum); //! Shader_PosTex 7번 패스 = VS_MAIN,  PS_UI_HP

	//! 내가 그리려고 하는 정점, 인덱스 버퍼를 장치에 바인딩해
	m_pVIBufferCom->Bind_VIBuffers();

	//! 바인딩된 정점, 인덱스를 그려
	m_pVIBufferCom->Render();
	
	return S_OK;
}

void CUI_Video::UI_Ready(_float fTimeDelta)
{
}

void CUI_Video::UI_Enter(_float fTimeDelta)
{
}

void CUI_Video::UI_Loop(_float fTimeDelta)
{
}

void CUI_Video::UI_Exit(_float fTimeDelta)
{
}

HRESULT CUI_Video::Ready_Components()
{
	//! For.Com_Texture1 // 흰색 바
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("ui_element_health_bar_damagel"),
		TEXT("Com_Texture_WhiteBar"), reinterpret_cast<CComponent**>(&m_pTextureCom[HPBAR_WHITE]))))
		return E_FAIL;

	//! For.Com_Shader
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	//! For.Com_VIBuffer
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* 효과가 필요한 녀석은 Map텍스쳐도 추가해주기 */
	return S_OK;
}

HRESULT CUI_Video::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	
	//// 비디오 텍스처 바인딩
	//if (FAILED(m_pTextureVideo->Bind_ShaderResource(m_pShaderCom, "g_VideoTexture")))
	//	return E_FAIL;

	if (FAILED(m_pTextureCom[HPBAR_WHITE]->Bind_ShaderResource(m_pShaderCom, "g_HpBarWhite_Texture")))	// Hp White
		return E_FAIL;

	return S_OK;
}

// 비디오 로드 함수 : 지원하는 확장자 (mp4, avi, wmv, mov, mpeg, flv)
HRESULT CUI_Video::LoadVideo(const wchar_t* filePath)
{
	//HRESULT hr = S_OK;
	//IMFSourceReader* m_pSourceReader = nullptr;
	//// Media Foundation 초기화 함수
	//hr = MFStartup(MF_VERSION);
	//if (FAILED(hr)) return hr;

	//// 비디오 파일 열기 : m_pSourceReader에 비디오 파일을 읽는데 사용되는 소스리더 객체를 저장함. (대충 비디오 정보 저장)
	//hr = MFCreateSourceReaderFromURL(filePath, nullptr, &m_pSourceReader);
	//if (FAILED(hr)) 
	//{
	//	MFShutdown(); // Media Foundation 종료
	//	return hr;
	//}

	//// 비디오 형식 가져오기 : 첫 번째 비디오 스트림의 현재 미디어 유형 정보를 m_pMediaType에 가져온다.
	//hr = m_pSourceReader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, &m_pMediaType);
	//if (FAILED(hr)) 
	//{
	//	m_pSourceReader->Release();
	//	MFShutdown(); // Media Foundation 종료
	//	return hr;
	//}

	//// 비디오 프레임의 너비와 높이 가져오기
	//UINT32 width = 0, height = 0;
	//hr = MFGetAttributeSize(m_pMediaType, MF_MT_FRAME_SIZE, &width, &height);
	//if (FAILED(hr)) 
	//{
	//	m_pMediaType->Release();
	//	m_pSourceReader->Release();
	//	MFShutdown(); // Media Foundation 종료
	//	return hr;
	//}

	//// 비디오 프레임 속성 설정
	//IMFMediaType* pOutputType = nullptr;
	//hr = MFCreateMediaType(&pOutputType);
	//if (FAILED(hr)) 
	//{
	//	m_pMediaType->Release();
	//	m_pSourceReader->Release();
	//	MFShutdown(); // Media Foundation 종료
	//	return hr;
	//}

	//hr = pOutputType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
	//if (FAILED(hr)) 
	//{
	//	pOutputType->Release();
	//	m_pMediaType->Release();
	//	m_pSourceReader->Release();
	//	MFShutdown(); // Media Foundation 종료
	//	return hr;
	//}

	//hr = pOutputType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32);
	//if (FAILED(hr)) 
	//{
	//	pOutputType->Release();
	//	m_pMediaType->Release();
	//	m_pSourceReader->Release();
	//	MFShutdown(); // Media Foundation 종료
	//	return hr;
	//}

	//hr = m_pSourceReader->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, nullptr, pOutputType);
	//if (FAILED(hr)) 
	//{
	//	pOutputType->Release();
	//	m_pMediaType->Release();
	//	m_pSourceReader->Release();
	//	MFShutdown(); // Media Foundation 종료
	//	return hr;
	//}

	//pOutputType->Release();

	//// 프레임 읽기
	//DWORD dwStreamIndex, dwFlags;
	//LONGLONG llTimestamp;
	//IMFSample* pSample = nullptr;
	//IMFMediaBuffer* pBuffer = nullptr;

	//// 첫 번째 동영상의 프레임 읽기
	//// 첫 번째 동영상에서 프레임을 읽어 4개의 매개변수에 정보들을 담아준다. (pSample에 해당 프레임 데이터를 담아주고, 이 데이터를 추출 해 텍스처로 변환한 후 랜더 시켜서 동영상을 보여준다.)
	//hr = m_pSourceReader->ReadSample((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, &dwStreamIndex, &dwFlags, &llTimestamp, &pSample);
	//if (FAILED(hr)) return hr;

	//// 프레임 끝 검사
	//if (dwFlags & MF_SOURCE_READERF_ENDOFSTREAM) 
	//{
	//	// 더 이상 프레임이 없음
	//	pSample->Release();
	//	return S_FALSE;
	//}

	//if (pSample == nullptr) 
	//{
	//	// 프레임이 없음
	//	return S_FALSE;
	//}

	//hr = pSample->ConvertToContiguousBuffer(&pBuffer);
	//if (FAILED(hr)) 
	//{
	//	pSample->Release();
	//	return hr;
	//}

	//// 프레임의 이미지 데이터 가져오기
	//BYTE* pFrameData = nullptr;
	//DWORD cbMaxLength, cbCurrentLength;
	//hr = pBuffer->Lock(&pFrameData, &cbMaxLength, &cbCurrentLength);
	//if (FAILED(hr)) 
	//{
	//	pBuffer->Release();
	//	pSample->Release();
	//	return hr;
	//}

	//// 프레임의 너비와 높이 가져오기
	//UINT32 width, height;
	//hr = MFGetAttributeSize(m_pMediaType, MF_MT_FRAME_SIZE, &width, &height);
	//if (FAILED(hr)) 
	//{
	//	pBuffer->Unlock();
	//	pBuffer->Release();
	//	pSample->Release();
	//	return hr;
	//}


	//ID3D11ShaderResourceView* pShaderResourceView = nullptr;

	//// ShaderResourceView로 프레임 데이터 복사
	//hr = CUI_Video::CreateShaderResourceViewFromVideoFrame(
	//														m_pDevice,
	//														m_pContext,
	//														pFrameData,
	//														width,
	//														height,
	//														&pShaderResourceView);

	//// 셰이더로 던져주자
	//m_pShaderCom->Bind_SRV("g_Video_Texture", pShaderResourceView);

	////// 텍스처로 프레임 데이터 복사
	////hr = CopyVideoFrameToTexture(m_pContext, pFrameData, width, height, m_pTextureVideo);
	////if (FAILED(hr)) 
	////{
	////	pBuffer->Unlock();
	////	pBuffer->Release();
	////	pSample->Release();
	////	return hr;
	////}

	//pBuffer->Unlock();
	//pBuffer->Release();
	//pSample->Release();

	return S_OK;
}

HRESULT CUI_Video::CopyVideoFrameToTexture(ID3D11DeviceContext* pContext, BYTE* pFrameData, UINT32 width, UINT32 height, ID3D11Texture2D* pTexture)
{
	HRESULT hr = S_OK;

	// 텍스처 매핑 및 프레임 데이터 복사
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	hr = pContext->Map(pTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr))
		return hr;

	BYTE* pTextureData = reinterpret_cast<BYTE*>(mappedResource.pData);
	UINT32 rowPitch = width * 4; // RGBA 형식이므로 너비 * 4

	for (UINT32 y = 0; y < height; ++y) 
	{
		memcpy(pTextureData, pFrameData, rowPitch);
		pTextureData += mappedResource.RowPitch; // 다음 행으로 이동
		pFrameData += rowPitch; // 다음 행으로 이동
	}

	pContext->Unmap(pTexture, 0);

	return S_OK;
}

HRESULT CUI_Video::CreateShaderResourceViewFromVideoFrame(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, BYTE* pFrameData, UINT32 width, UINT32 height, ID3D11ShaderResourceView** ppShaderResourceView)
{
	//HRESULT hr = S_OK;

	//// 텍스처 생성
	//ID3D11Texture2D* pTexture = nullptr;
	//D3D11_TEXTURE2D_DESC desc;
	//desc.Width = width;
	//desc.Height = height;
	//desc.MipLevels = 1;
	//desc.ArraySize = 1;
	//desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // RGBA 형식
	//desc.SampleDesc.Count = 1;
	//desc.SampleDesc.Quality = 0;
	//desc.Usage = D3D11_USAGE_DEFAULT;
	//desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	//desc.CPUAccessFlags = 0;
	//desc.MiscFlags = 0;

	//hr = pDevice->CreateTexture2D(&desc, nullptr, &pTexture);
	//if (FAILED(hr))
	//	return hr;

	//// 텍스처 매핑 및 프레임 데이터 복사
	//D3D11_MAPPED_SUBRESOURCE mappedResource;
	//hr = pContext->Map(pTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	//if (FAILED(hr))
	//{
	//	pTexture->Release();
	//	return hr;
	//}

	//BYTE* pTextureData = reinterpret_cast<BYTE*>(mappedResource.pData);
	//UINT32 rowPitch = width * 4; // RGBA 형식이므로 너비 * 4

	//for (UINT32 y = 0; y < height; ++y)
	//{
	//	memcpy(pTextureData, pFrameData, rowPitch);
	//	pTextureData += mappedResource.RowPitch; // 다음 행으로 이동
	//	pFrameData += rowPitch; // 다음 행으로 이동
	//}

	//pContext->Unmap(pTexture, 0);

	//// SRV 생성
	//D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	//ZeroMemory(&srvDesc, sizeof(srvDesc));
	//srvDesc.Format = desc.Format;
	//srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	//srvDesc.Texture2D.MipLevels = desc.MipLevels;

	//ID3D11ShaderResourceView* pShaderResourceView = nullptr;
	//hr = pDevice->CreateShaderResourceView(pTexture, &srvDesc, &pShaderResourceView);
	//if (FAILED(hr))
	//{
	//	pTexture->Release();
	//	return hr;
	//}

	//pTexture->Release(); // SRV 생성 후 텍스처는 더 이상 필요하지 않으므로 해제

	//*ppShaderResourceView = pShaderResourceView;

	return S_OK;
}

// 동영상을 프레임 단위로 변환하여 던져준다.
HRESULT CUI_Video::ReadVideoFrame()
{
	//if (m_pSourceReader == nullptr)
	//	return E_FAIL;

	//HRESULT hr = S_OK;

	//	// 프레임 읽기
	//DWORD dwStreamIndex, dwFlags;
	//LONGLONG llTimestamp;
	//IMFSample* pSample = nullptr;
	//IMFMediaBuffer* pBuffer = nullptr;

	//// 첫 번째 동영상의 프레임 읽기
	//// 첫 번째 동영상에서 프레임을 읽어 4개의 매개변수에 정보들을 담아준다. (pSample에 해당 프레임 데이터를 담아주고, 이 데이터를 추출 해 텍스처로 변환한 후 랜더 시켜서 동영상을 보여준다.)
	//if(m_pSourceReader != nullptr)
	//	hr = m_pSourceReader->ReadSample((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, &dwStreamIndex, &dwFlags, &llTimestamp, &pSample);

	//if (FAILED(hr)) return hr;

	//// 프레임 끝 검사
	//if (dwFlags & MF_SOURCE_READERF_ENDOFSTREAM) 
	//{
	//	// 더 이상 프레임이 없음
	//	pSample->Release();
	//	return S_FALSE;
	//}

	//if (pSample == nullptr) 
	//{
	//	// 프레임이 없음
	//	return S_FALSE;
	//}

	//hr = pSample->ConvertToContiguousBuffer(&pBuffer);
	//if (FAILED(hr)) 
	//{
	//	pSample->Release();
	//	return hr;
	//}

	//// 프레임의 이미지 데이터 가져오기
	//BYTE* pFrameData = nullptr;
	//DWORD cbMaxLength, cbCurrentLength;
	//hr = pBuffer->Lock(&pFrameData, &cbMaxLength, &cbCurrentLength);
	//if (FAILED(hr)) 
	//{
	//	pBuffer->Release();
	//	pSample->Release();
	//	return hr;
	//}

	//// 프레임의 너비와 높이 가져오기
	//UINT32 width, height;
	//hr = MFGetAttributeSize(m_pMediaType, MF_MT_FRAME_SIZE, &width, &height);
	//if (FAILED(hr)) 
	//{
	//	pBuffer->Unlock();
	//	pBuffer->Release();
	//	pSample->Release();
	//	return hr;
	//}


	//ID3D11ShaderResourceView* pShaderResourceView = nullptr;

	//// ShaderResourceView로 프레임 데이터 복사
	//hr = CUI_Video::CreateShaderResourceViewFromVideoFrame(
	//														m_pDevice,
	//														m_pContext,
	//														pFrameData,
	//														width,
	//														height,
	//														&pShaderResourceView);

	//// 셰이더로 던져주자
	//m_pShaderCom->Bind_SRV("g_Video_Texture", pShaderResourceView);

	////// 텍스처로 프레임 데이터 복사
	////hr = CopyVideoFrameToTexture(m_pContext, pFrameData, width, height, m_pTextureVideo);
	////if (FAILED(hr)) 
	////{
	////	pBuffer->Unlock();
	////	pBuffer->Release();
	////	pSample->Release();
	////	return hr;
	////}

	//pBuffer->Unlock();
	//pBuffer->Release();
	//pSample->Release();

	return S_OK;
}

HRESULT CUI_Video::CreateVideoTexture(ID3D11Device* pDevice, UINT32 width, UINT32 height, ID3D11Texture2D** ppTexture, ID3D11ShaderResourceView** ppTextureView)
{
	HRESULT hr = S_OK;

	// 텍스처 생성
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = width;
	desc.Height = height;
	desc.ArraySize = 1;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.SampleDesc.Count = 1;

	hr = pDevice->CreateTexture2D(&desc, nullptr, ppTexture);
	if (FAILED(hr)) return hr;

	// 텍스처 리소스 뷰 생성
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	hr = pDevice->CreateShaderResourceView(*ppTexture, &srvDesc, ppTextureView);
	if (FAILED(hr)) {
		(*ppTexture)->Release();
		return hr;
	}

	return S_OK;
}

HRESULT CUI_Video::ConvertVideoFrameToTexture(ID3D11DeviceContext* pContext, BYTE* pFrameData, UINT32 width, UINT32 height, ID3D11Texture2D* pTexture)
{
	HRESULT hr = S_OK;

	// 텍스처에 프레임 데이터 복사
	hr = CopyVideoFrameToTexture(pContext, pFrameData, width, height, pTexture);
	if (FAILED(hr)) return hr;

	return S_OK;
}

json CUI_Video::Save_Desc(json& out_json)
{
	/* 기본정보 저장 */
	__super::Save_Desc(out_json);


	/* 추가정보 저장 */


	return out_json;
}

void CUI_Video::Load_Desc()
{

}

CUI_Video* CUI_Video::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CUI_Video* pInstance = new CUI_Video(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_Video");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_Video::Clone(void* pArg)
{
	CUI_Video* pInstance = new CUI_Video(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_Video");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_Video::Pool()
{
	return new CUI_Video(*this);
}

void CUI_Video::Free()
{
	__super::Free();

	for (auto& pTexture : m_pTextureCom)
	{
		if (pTexture != nullptr)
			Safe_Release(pTexture);
	}

}
