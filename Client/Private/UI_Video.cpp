#include "stdafx.h"
#include "UI_Video.h"
#include "GameInstance.h"
#include "Json_Utility.h"

#include "Data_Manager.h"
// #include <avformat.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#include <libavutil/avutil.h>

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

	//! 이 셰이더에 0번째 패스로 그린다.
	m_pShaderCom->Begin(m_iShaderNum); //! Shader_PosTex 7번 패스 = VS_MAIN,  PS_UI_HP

	//! 내가 그리려고 하는 정점, 인덱스 버퍼를 장치에 바인딩해
	m_pVIBufferCom->Bind_VIBuffers();

	//! 바인딩된 정점, 인덱스를 그려
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_Video::Ready_Components()
{
	//! For.Com_Texture1 // 흰색 바
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("ui_element_health_bar_damagel"),
		TEXT("Com_Texture_WhiteBar"), reinterpret_cast<CComponent**>(&m_pTextureCom[HPBAR_WHITE]))))
		return E_FAIL;

	//D3D11_TEXTURE2D_DESC textureDesc;
	//textureDesc.Width = g_iWinsizeX;
	//textureDesc.Height = g_iWinsizeY;
	//textureDesc.MipLevels = 1;
	//textureDesc.ArraySize = 1;
	//textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	//textureDesc.SampleDesc.Count = 1;
	//textureDesc.SampleDesc.Quality = 0;
	//textureDesc.Usage = D3D11_USAGE_DEFAULT;
	//textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
	//textureDesc.CPUAccessFlags = 0;
	//textureDesc.MiscFlags = 0;

	//m_pDevice->CreateTexture2D(&textureDesc, NULL, &m_pTextureVideo);

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

	//AVFormatContext* format_context;
	//const char* input_file;

	//// FFmpeg 라이브러리 초기화
	//avformat_open_input(&format_context, input_file, NULL, NULL);
	//avformat_find_stream_info(format_context, NULL);

	//// 비디오 스트림 정보 얻기
	//AVStream* video_stream = format_context->streams[0];
	//AVCodecContext* video_codec_context = video_stream->codecpar;

	//// SWScaler 컨텍스트 생성
	//SwsContext* sws_context = sws_getContext(
	//	video_codec_context->width, video_codec_context->height, video_codec_context->pix_fmt,
	//	video_codec_context->width, video_codec_context->height, AV_PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);

	//// 프레임 및 변환된 프레임
	//AVFrame* frame = av_frame_alloc();
	//AVFrame* converted_frame = av_frame_alloc();

	//// RGB 버퍼 생성
	//uint8_t* rgb_buffer = (uint8_t*)malloc(avpicture_get_size(AV_PIX_FMT_RGB24, video_codec_context->width, video_codec_context->height));

	//// 비디오 디코딩 및 변환 루프
	//while (av_read_frame(format_context, &packet) >= 0) {
	//	if (packet.stream_index == video_stream->index) {
	//		// 프레임 디코딩
	//		avcodec_decode_video2(video_codec_context, frame, &got_frame, &packet);

	//		if (got_frame) {
	//			// 프레임 변환
	//			sws_scale(sws_context, (const uint8_t**)frame->data, frame->linesize, 0,
	//				video_codec_context->height, converted_frame->data, converted_frame->linesize);

	//			// RGB 버퍼에 복사
	//			avpicture_layout(converted_frame, AV_PIX_FMT_RGB24, video_codec_context->width, video_codec_context->height, rgb_buffer, NULL);
	//		}
	//	}

	//	// 패킷 정보 해제
	//	av_packet_unref(&packet);
	//}


	if (FAILED(m_pTextureCom[HPBAR_WHITE]->Bind_ShaderResource(m_pShaderCom, "g_HpBarWhite_Texture")))	// Hp White
		return E_FAIL;

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
