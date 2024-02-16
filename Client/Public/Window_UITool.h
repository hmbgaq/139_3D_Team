#pragma once

#include "Imgui_Window.h"
#include "UI_Anything.h"

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

public:
	// SH_Add
	typedef struct tagPathInfo
	{
		_int		iPathNum = 0;
		string		strFileName;
		string		strFilePath;
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

public: /* ================= Function ================= */
	// string타입을 받는 벡터 컨테이너를 char*타입을 받는 벡터 컨테이너로 변환 해주는 함수
	std::vector<const char*> ConvertStringVectorToCharArray(const std::vector<std::string>& stringVector);
	// string을 wstring으로 변환 해주는 함수
	std::wstring ConvertToWideString(const std::string& ansiString);
	// string을 wchar로 변환 해주는 함수
	WCHAR* StringTowchar(const std::string& str);
	// wstring을 string으로 변환 해주는 함수
	std::string WStringToString(const std::wstring& wstr);
	//	wstring을 char로 변환 해주는 함수
	char* ConverWStringtoC(const wstring& wstr);
	//	char를 wchar_t로 변환 해주는 함수
	wchar_t* ConverCtoWC(char* str);
	// WCHAR*를 string으로 변환 해주는 함수
	std::string WideStringToString(const wchar_t* wideStr);
	// 경로에서 파일이름과 확장자만 추출해주는 함수
	std::string GetFileName(const std::string& filePath);
	// 확장자를 제거해주는 함수
	std::string RemoveExtension(const std::string& filePath);


public:
	// UI 설정
	void UI2D_Setting(_float fTimeDelta);
	HRESULT UI2D_Create(_float fTimeDelta);
	void UI2D_Delete(_float fTimeDelta);

public: /* Save/Load */
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
	std::vector<string>			m_vecObjectName;
	_int						m_iSelectedPathIndex = 0; // 선택된 이미지 경로 인덱스
	_int						m_iSelectedObjectIndex = 0; // 선택된 이미지 경로 인덱스
	_int						m_iUINameNum = 0;
private: /* 2D */
	_float						m_fPosition[2] = { 0.f, 0.f };
	_float						m_fScale[2] = { 0.f, 0.f };
	vector<CGameObject*>		m_vecUIObject;
	string						m_strItems[3] = { "Layer_UI_Player", "Layer_UI_Monster", "Layer_UI_Inventory" };

	_int						m_iLayerNum = 0;
	CUI_Anything::UIANYTHING	m_tUI_Desc;

public:
	static CWindow_UITool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END

