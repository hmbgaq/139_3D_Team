#pragma once

#include "Imgui_Window.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)

class CWindow_UITool final : public CImgui_Window
{
public: /* ==================== Struct ==================== */
	struct WINDOW_UITOOL_DESC : public ImGuiDESC
	{
	};

	typedef struct tagImageInfo
	{
		_int						iImage_Width = 100;
		_int						iImage_Height = 100;

		_int						iTextureNum = -1;

		ID3D11ShaderResourceView* SRV_Texture = NULL;
	}IMAGEINFO;

	typedef struct tagPathInfo
	{
		_int		iPathNum = 0;
		WCHAR		cFileName[MAX_PATH] = TEXT("");
		wstring		strFilePath = TEXT("");
	}PATHINFO;

public:
	typedef struct tagUIBaseDesc
	{
		//			위치 X,Y / 사이즈 X,Y
		_float		fX, fY, fSizeX, fSizeY;

		_float		fTexSizeX = 0.f, fTexSizeY = 0.f;

		_float		fOffsetX = 0.f, fOffsetY = 0.f;

		_bool		bWorldUI = false;	// 월드상의 UI인지
		_bool		bFrame = false;		// Frame
		_bool		bEnable = true;		// Enable 여부
	}UI_DESC;

protected: /* ============ Create/Extinction ============= */
	CWindow_UITool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CWindow_UITool() = default;


public: /* =================== Basic =====================*/
	virtual	HRESULT				Initialize() override;
	virtual	void				Tick(_float fTimeDelta) override;
	virtual void				Render() override;

public: /* ==================== UI ===================== */
	void						UI_List(_float fTimeDelta);
	void						UI_ToolTip(_float fTimeDelta);
	std::vector<unsigned char>	UI_LoadImage(const std::string& filename, int& width, int& height, int& channels);
	void						ShowImagePreview(const std::vector<unsigned char>& imageData, int width, int height);
	bool						LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height);

public: /* Function */
	std::vector<const char*> ConvertStringVectorToCharArray(const std::vector<std::string>& stringVector);
	std::wstring ConvertToWideString(const std::string& ansiString);
	//	wstring을 char로 변경
	char* ConverWStringtoC(const wstring& wstr);
	//	char를 wchar_t로 변경
	wchar_t* ConverCtoWC(char* str);

public:
	// UI 설정
	void UI2D_Setting(_float fTimeDelta);
	HRESULT UI2D_Create(_float fTimeDelta);
	void UI2D_Delete(_float fTimeDelta);
	void Save_Desc();
	void Load_Desc();

public: /* Image */
	// 이미지 로드 함수
	void LoadImg(const _tchar* folderPath);
	void ImagePreview(_float fTimeDelta);

private: /* Member */
	UI_DESC						m_tUI_Info;


private: /* Image_Member */
	_int						m_My_Image_Width = 100;
	_int						m_My_Image_Height = 100;
	_int						m_iTextureNum = 0;
	_int						m_iTestNum = 0;
	vector<IMAGEINFO*>			m_vecTexture;	// 이미지 미리보기
	vector<PATHINFO*>			m_vecPaths;		// 경로, 파일이름 (프로토타입 파싱가능)
	IMAGEINFO*					m_tTexture;

	// 문자열 벡터를 const char* 배열로 변환
	std::vector<const char*>	m_vecImagePaths;
	_int						m_iSelectedPathIndex = 0; // 선택된 이미지 경로 인덱스

private: /* 2D */
	_float						m_fPosition[2] = { 0.f, 0.f };
	_float						m_fScale[2] = { 0.f, 0.f };
	vector<CGameObject*>		m_vecUIObject;
public:
	static CWindow_UITool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END

