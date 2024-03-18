#pragma once
#include "Client_Defines.h"
#include "UI.h"

/* 체력 프레임 */
class CUI_Video final : public CUI
{
public: /* 각 UI파츠마다 어떤걸 얼마나 가질지 설정해주자. */
	enum TEXTUREKIND { HPBAR_WHITE, HPBAR_RED, HP_DECAL, TEXTURE_END };

private:
	CUI_Video(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CUI_Video(const CUI_Video& rhs);
	virtual ~CUI_Video() = default;

public:
	virtual HRESULT			Initialize_Prototype() override; //! 원형객체의 초기화를 위한 함수.
	virtual HRESULT			Initialize(void* pArg) override; //! 사본객체의 초기화를 위한 함수. ( void*를 매개인자로 받아 특수한 사본객체 처리가 가능하다. )
	virtual void			Priority_Tick(_float fTimeDelta) override;
	virtual void			Tick(_float fTimeDelta) override;
	virtual void			Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	virtual HRESULT			Ready_Components() override;
	virtual HRESULT			Bind_ShaderResources() override;

private:
	CTexture*				m_pTextureCom[TEXTURE_END] = { nullptr };
	ID3D11Texture2D*		m_pTextureVideo = nullptr;
	TEXTUREKIND				m_eTexture_Kind = TEXTURE_END;

private: /* Video Load */
	HRESULT LoadVideo(const wchar_t* filePath);
	// 비디오 프레임을 텍스처로 복사하는 함수
	HRESULT CopyVideoFrameToTexture(ID3D11DeviceContext* pContext, BYTE* pFrameData, UINT32 width, UINT32 height, ID3D11Texture2D* pTexture);
	// 비디오 텍스처 생성 및 데이터 채우기 함수
	HRESULT CreateVideoTexture(ID3D11Device* pDevice, UINT32 width, UINT32 height, ID3D11Texture2D** ppTexture, ID3D11ShaderResourceView** ppTextureView);
	// 비디오 프레임을 텍스처로 변환하는 함수
	HRESULT ConvertVideoFrameToTexture(ID3D11DeviceContext* pContext, BYTE* pFrameData, UINT32 width, UINT32 height, ID3D11Texture2D* pTexture);
	// 비디오 프레임을 ShaderResourceView로 변환하는 함수
	HRESULT CreateShaderResourceViewFromVideoFrame(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, BYTE* pFrameData, UINT32 width, UINT32 height, ID3D11ShaderResourceView** ppShaderResourceView);
	// 비디오 프레임 순회
	HRESULT	ReadVideoFrame();

public:
	// Media Foundation 관련 변수
	//IMFSourceReader* m_pSourceReader = nullptr;
	//IMFMediaType* m_pMediaType = nullptr;
	//IMFAttributes* m_pAttributes = nullptr;
	//IMFMediaBuffer* m_pBuffer = nullptr;
	BYTE* m_pTextureData = nullptr;
	ID3D11ShaderResourceView* m_pTextureView = nullptr;

public:
	virtual json			Save_Desc(json& out_json);
	void					Load_Desc();

public:
	static CUI_Video* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag); //! 원형객체 생성
	virtual CGameObject* Clone(void* pArg) override; //! 사본객체 생성
	virtual CGameObject* Pool() override;
	virtual void			Free() override;
};

