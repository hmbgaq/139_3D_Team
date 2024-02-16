#include "stdafx.h"
#include "Window_UITool.h"
#include "UI_Anything.h"
#include "GameInstance.h"

#include "Json_Utility.h"

/* error 외부참조 기호 : define 걸어줘야함 */
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

CWindow_UITool::CWindow_UITool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CImgui_Window(pDevice, pContext)
{
}


HRESULT CWindow_UITool::Initialize()
{
	//! 현재는 특별한 기능없음. 추후 필요할 것 같아서 셋팅.
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	/* 해당 경로안에 있는 모든 이미지들을 불러온다. */
	LoadImg(ConverCtoWC(ConverWStringtoC(TEXT("../Bin/Resources/Textures/UI/Textures/PlayerHUD"))));
	//LoadImg(ConverCtoWC(ConverWStringtoC(TEXT("../Bin/Resources/Textures/UI/Textures"))));

	// 이미지 로드 Test
	_int iSize = m_vecPaths.size();
	for (_int i = 0; i < iSize; i++)
	{
		IMAGEINFO* tTexture = new IMAGEINFO;

		_bool bRet = LoadTextureFromFile(ConverWStringtoC(ConvertToWideString(m_vecPaths[i]->strFilePath)), &tTexture->SRV_Texture, &tTexture->iImage_Width, &tTexture->iImage_Height);
		IM_ASSERT(bRet);

		/* Texture 크기를 임의로 조절하고 싶다면, 여기서 강제로 덮어씌우자. 값을 안주면 원래 텍스처 크기대로 나온다. 추 후 원본 크기를 이용해 비율만 줄여서 출력해도 좋을 것 같다. */
		//tTexture->iImage_Width = 100;
		//tTexture->iImage_Height = 100;
		tTexture->iTextureNum = i;
		m_vecTexture.push_back(tTexture);

	}

	for (auto& iter : m_vecPaths)
	{
		string strFileNameWithoutExtension = GetFileName(iter->strFilePath);
		string strFileName = RemoveExtension(strFileNameWithoutExtension);

		/* 경로 잘라서 넣기 */
		m_vecImagePaths.push_back(ConverWStringtoC(ConvertToWideString(strFileName)));
	}

#pragma region 경로저장
	//char filePath[MAX_PATH] = "../Bin/DataFiles/Data_UI/Texture_Info/Texture_Info";

	//json Out_Json;
	//_ushort iIndex = 0;

	//for (PATHINFO* iter : m_vecPaths)
	//{
	//	json object;
	//	object["PathNum"] = iter->iPathNum;
	//	object["FileName"] = iter->strFileName;
	//	object["FilePath"] = iter->strFilePath;
	//	Out_Json.emplace(to_string(iIndex++), object);
	//}

	//CJson_Utility::Save_Json(filePath, Out_Json);

	//CJson_Utility::Load_Json(filePath, Out_Json);
#pragma endregion 

	return S_OK;
}

void CWindow_UITool::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	__super::Begin();
	ImGui::Text("UI_Tool");

	/* List */
	UI_List(fTimeDelta);

	/* UI_2D 세팅 */
	UI2D_Setting(fTimeDelta);

	/* 이미지 선택 및 미리보기 */
	ImagePreview(fTimeDelta);

	ImGui::Dummy(ImVec2(0, 5.f)); // 공백

	/* Default : LastNumber */
	UI_ToolTip(fTimeDelta); // Tip : 툴팁은 오버레이시 모든 출력중 가장 마지막에 호출되어야한다. (안그러면 다른 녀석들이 툴팁에 밀려서 출력됨)
	__super::End();
}

void CWindow_UITool::Render()
{

}

void CWindow_UITool::UI_List(_float fTimeDelta)
{
	//if (m_vecUIObject.empty())
	//	return;

	static int	Object_idx = 0;
	static int	Texture_idx = 0;
	static int	Layer_idx = 0;

	if (ImGui::BeginListBox("LayerList"))
	{
		for (_int i = 0; i < 3; i++)
		{
			const bool is_selected = (Layer_idx == i);
			if (ImGui::Selectable(m_strItems[i].c_str(), is_selected))
				Layer_idx = i;
			m_iLayerNum = i;
			
			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}

		ImGui::EndListBox();
	}

	ImGui::Spacing();

	_int		iTextureTagSize = (_int)m_vecImagePaths.size();
	if (ImGui::BeginListBox("TextureList"))
	{
		for (_int i = 0; i < iTextureTagSize; i++)
		{
			const bool is_selected = (m_iSelectedPathIndex == i);
			if (ImGui::Selectable(m_vecImagePaths[i], is_selected))
			{			
				m_iSelectedPathIndex = i;
			}
			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}

	//ImGui::Text("pointer = %p", m_vecTexture[m_iSelectedPathIndex]->SRV_Texture);
	//ImGui::Text("size = %f x %f", m_vecTexture[m_iSelectedPathIndex]->iImage_Width, m_vecTexture[m_iSelectedPathIndex]->iImage_Height);
	ImGui::Image((void*)m_vecTexture[m_iSelectedPathIndex]->SRV_Texture, ImVec2(m_vecTexture[m_iSelectedPathIndex]->iImage_Width, m_vecTexture[m_iSelectedPathIndex]->iImage_Height));

	/* 테그 */
	m_tUI_Desc.strProtoTag = m_vecImagePaths[m_iSelectedPathIndex];

	ImGui::Spacing();
	_int		ObjectTagSize = (_int)m_vecObjectName.size();
	if (ImGui::BeginListBox("ObjectList"))
	{
		for (_int i = 0; i < ObjectTagSize; i++)
		{
			const bool is_selected = (Object_idx == i);
			if (ImGui::Selectable(ConverWStringtoC(ConvertToWideString(m_vecObjectName[i])), is_selected))
			{
				Object_idx = i;
			}
			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}

	if (!m_vecUIObject.empty())
	{
		_vector vPosition = m_vecUIObject[Object_idx]->Get_Transform()->Get_State(CTransform::STATE_POSITION);

		ImGui::InputFloat("PositionX", &vPosition.m128_f32[0]);
		ImGui::InputFloat("PositionY", &vPosition.m128_f32[1]);
		ImGui::InputFloat("PositionZ", &vPosition.m128_f32[2]);
	}


	///* Test Value */
	//m_tUI_Info.strName = "Test UI List";
	//m_tUI_Info.iNum = 1;
	//m_tUI_Info.fNum = 1.5f;

	//// 정보를 목록으로 표시
	//if (ImGui::TreeNode(m_tUI_Info.strName.c_str())) {
	//	ImGui::Text(u8"값1 : %d", m_tUI_Info.iNum);
	//	ImGui::Text(u8"값2 : %.2f", m_tUI_Info.fNum);

	//	// 버튼
	//	if (ImGui::Button(u8"버튼"))
	//	{

	//	}

	//	ImGui::TreePop();
	//}
}

void CWindow_UITool::UI_ToolTip(_float fTimeDelta)
{
	/* 테스트 값 */
	_int	iTestNum = 1;
	_float	fTestValue1 = 33.f;
	_float	fTestValue2 = 44.f;

	/* 마우스 오버할 위치 */
	_float	fTestX = 50.f;
	_float	fTestY = 500.f;

	/* 마우스 오버할 범위 */
	_float	fHoverRangeX = 200.f;
	_float	fHoverRangeY = 200.f;

	/* 마우스 오버시 나오는 정보창의 크기 */
	_float	fScreenPosX = 20.f;
	_float	fScreenPosY = 40.f;

	//// 마우스가 해당 위치 위에 있는 경우에만 툴팁 표시 (마우스 오버)
	//if (ImGui::IsMouseHoveringRect(
	//	ImVec2(fTestX - fHoverRangeX, fTestY - fHoverRangeY),
	//	ImVec2(fTestX + fHoverRangeX, fTestY + fHoverRangeY)))
	//{

	//	// 표시할 정보
	//	ImGui::SetCursorScreenPos(ImVec2(fTestX + fScreenPosX, fTestY - fScreenPosY));
	//	ImGui::BeginTooltip();

	//	ImGui::EndTooltip(); // 툴팁 종료
	//}
}

// 이미지를 메모리에 로드하는 함수
std::vector<unsigned char> CWindow_UITool::UI_LoadImage(const std::string& filename, int& width, int& height, int& channels)
{
	// 파일 내용 읽기 (미리보기로 최대 100바이트까지만 출력)
	std::ifstream file(filename, std::ios::binary);
	if (file.is_open()) {
		char buffer[101]; // 100바이트 + 널 문자
		if (file.read(buffer, 100)) {
			buffer[file.gcount()] = '\0'; // 널 문자 추가
			std::cout << "Preview: " << buffer << std::endl;
		}
		file.close();
	}
	else {
		std::cerr << "Failed to open file: " << filename << std::endl;
	}

	// 파일 크기 확인
	file.seekg(0, std::ios::end);
	std::streamsize fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	// 파일 내용을 저장할 벡터 생성
	std::vector<unsigned char> imageData(fileSize);
	if (!file.read(reinterpret_cast<char*>(imageData.data()), fileSize)) {
		std::cerr << "Failed to read file: " << filename << std::endl;
		return {};
	}

	// 이미지 너비, 높이, 채널 수 설정 (예시로 고정 값 사용)
	width = 256;
	height = 256;
	channels = 4; // RGBA

	return imageData;
}

// Simple helper function to load an image into a DX11 texture with common settings
bool CWindow_UITool::LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height)
{
	// Load from disk into a raw RGBA buffer
	int image_width = 0;
	int image_height = 0;
	unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
	if (image_data == NULL)
		return false;

	// Create texture
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = image_width;
	desc.Height = image_height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;

	ID3D11Texture2D* pTexture = NULL;
	D3D11_SUBRESOURCE_DATA subResource;
	subResource.pSysMem = image_data;
	subResource.SysMemPitch = desc.Width * 4;
	subResource.SysMemSlicePitch = 0;
	m_pDevice->CreateTexture2D(&desc, &subResource, &pTexture);

	// Create texture view
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	m_pDevice->CreateShaderResourceView(pTexture, &srvDesc, out_srv);
	pTexture->Release();

	*out_width = image_width;
	*out_height = image_height;
	stbi_image_free(image_data);

	return true;
}

// std::string 벡터를 const char* 배열로 변환하는 함수
std::vector<const char*> CWindow_UITool::ConvertStringVectorToCharArray(const std::vector<std::string>& stringVector)
{
	std::vector<const char*> charArray;
	charArray.reserve(stringVector.size());
	for (const std::string& str : stringVector)
	{
		charArray.push_back(str.c_str());
	}
	return charArray;
}

std::wstring CWindow_UITool::ConvertToWideString(const std::string& ansiString)
{
	int wideStrLen = MultiByteToWideChar(CP_ACP, 0, ansiString.c_str(), -1, nullptr, 0);
	if (wideStrLen == 0)
	{
		// 변환 실패 처리
		return L"Fail";
	}

	std::wstring wideString(wideStrLen, L'\0');
	MultiByteToWideChar(CP_ACP, 0, ansiString.c_str(), -1, &wideString[0], wideStrLen);

	return wideString;
}

std::string CWindow_UITool::WStringToString(const std::wstring& wstr)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
	return converter.to_bytes(wstr);
}

char* CWindow_UITool::ConverWStringtoC(const wstring& wstr)
{
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
	char* result = new char[size_needed];
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, result, size_needed, NULL, NULL);
	return result;
}

wchar_t* CWindow_UITool::ConverCtoWC(char* str)
{
	_tchar* pStr;
	int strSize = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, NULL);
	pStr = new WCHAR[strSize];
	MultiByteToWideChar(CP_ACP, 0, str, (_int)strlen(str) + (size_t)1, pStr, strSize);

	return pStr;
}

std::string CWindow_UITool::WideStringToString(const wchar_t* wideStr) 
{
	// std::wstring으로부터 std::string으로 변환
	std::wstring wstr(wideStr);
	// std::string으로 변환
	return std::string(wstr.begin(), wstr.end());
}

// 파일 이름만 추출하는 함수
std::string CWindow_UITool::GetFileName(const std::string& filePath) 
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

std::string CWindow_UITool::RemoveExtension(const std::string& filePath)
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

WCHAR* CWindow_UITool::StringTowchar(const std::string& str)
{
	// std::wstring으로 변환
	std::wstring wstr(str.begin(), str.end());
	// c_str() 함수를 사용하여 WCHAR* 포인터로 변환
	return const_cast<WCHAR*>(wstr.c_str());
}

void CWindow_UITool::LoadImg(const _tchar* folderPath)
{
	// 찾은 이미지 데이터를 받을 변수
	WIN32_FIND_DATA findData;

	// 폴더 경로
	wstring wfolderPath = (wstring)folderPath + L"/*.*";

	// 폴더 경로 저장 및 핸들 반환
	HANDLE hFind = FindFirstFile(wfolderPath.c_str(), &findData);

	// 핸들이 있을 경우
	if (hFind != INVALID_HANDLE_VALUE)
	{
		// 하위 디렉토리
		vector<wstring> subDirectories;
		// 이미지 파일
		vector<wstring> imageFiles;
		do
		{
			//  파일의 속성 정보가 입력된다. (디렉토리인지 파일인지 등)
			if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (lstrcmp(findData.cFileName, L".") != 0 && lstrcmp(findData.cFileName, L"..") != 0)
				{
					// 폴더 경로 = 경로 + / + 파일명
					wstring subFolderPath = (wstring)folderPath + L"/" + findData.cFileName;
					subDirectories.push_back(subFolderPath);
					LoadImg(subFolderPath.c_str());
				}
			}
			else
			{
				// 파일인 경우, 이미지 파일인지 확인하고 로드
				wstring filePath = (wstring)folderPath + L"/" + findData.cFileName;

				// 파일 확장자 확인
				if (wcsstr(findData.cFileName, L".png") || wcsstr(findData.cFileName, L".jpg") ||
					/*wcsstr(findData.cFileName, L".bmp") || wcsstr(findData.cFileName, L".tga") ||*/
					wcsstr(findData.cFileName, L".dds"))
				{
					PATHINFO* pPathInfo = new PATHINFO;

					pPathInfo->strFilePath = WStringToString(filePath);
					//wcscpy(&pPathInfo->cFileName[MAX_PATH], StringTowchar(RemoveExtension(WideStringToString(findData.cFileName))));
					pPathInfo->strFileName = RemoveExtension(WStringToString(findData.cFileName));
					//pPathInfo->cFileName[MAX_PATH] = *StringTowchar(RemoveExtension(WideStringToString(findData.cFileName)));
					pPathInfo->iPathNum = m_iTextureNum;
					m_vecPaths.push_back(pPathInfo);

					m_iTextureNum++;
				}
			}
		} while (FindNextFile(hFind, &findData));

		
		FindClose(hFind);
	}
}

void CWindow_UITool::ImagePreview(_float fTimeDelta)
{

}

void CWindow_UITool::UI2D_Setting(_float fTimeDelta)
{
	ImGui::CollapsingHeader("2D_Setting");
	/* Scale */
	ImGui::SeparatorText(u8"크기 변경");
	ImGui::InputFloat("ScaleX", &m_tUI_Desc.fSizeX);
	ImGui::InputFloat("ScaleY", &m_tUI_Desc.fSizeY);

	/* Position*/
	ImGui::SeparatorText(u8"위치 변경");
	ImGui::InputFloat("PositionX", &m_tUI_Desc.fX);
	ImGui::InputFloat("PositionY", &m_tUI_Desc.fY);

	ImGui::Dummy(ImVec2(0, 2.5)); // 공백

	ImGui::Separator();

#pragma region Create/Delete
	if (ImGui::Button("Create"))
	{
		UI2D_Create(fTimeDelta);
	}

	ImGui::SameLine(70.f);

	if (ImGui::Button("Delete"))
	{
		UI2D_Delete(fTimeDelta);
	}
#pragma endregion End

#pragma region Save/Load
	if (ImGui::Button("Save"))
	{	
		Save_Desc();
	}

	ImGui::SameLine(70.f);

	if (ImGui::Button("Load"))
	{
		Load_Desc();
	}
#pragma endregion End
}

HRESULT CWindow_UITool::UI2D_Create(_float fTimeDelta)
{
	FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_STATIC, ConvertToWideString(m_strItems[m_iLayerNum]), TEXT("Prototype_GameObject_UI_Anything"), &m_tUI_Desc));

	m_vecObjectName.push_back(m_tUI_Desc.strProtoTag);
	m_pGameInstance->Get_CloneGameObjects(LEVEL_STATIC, &m_vecUIObject);
	m_iUINameNum++;
}

void CWindow_UITool::UI2D_Delete(_float fTimeDelta)
{

}

/* ex : Save */
void CWindow_UITool::Save_Desc()
{
	//string filePath = "Particle_Desc";
	char filePath[MAX_PATH] = "../Bin/DataFiles/Data_UI/UI_Info";

	json Out_Json;
	
	Out_Json["PostionX"] = m_tUI_Info.fX;
	Out_Json["PostionY"] = m_tUI_Info.fY;
	Out_Json["SizeX"] = m_tUI_Info.fSizeX;
	Out_Json["SizeY"] = m_tUI_Info.fSizeY;

	CJson_Utility::Save_Json(filePath, Out_Json);
}

/* ex : Load */
void CWindow_UITool::Load_Desc()
{
	json json_in;
	char filePath[MAX_PATH] = "../Bin/DataFiles/Data_UI/UI_Info";

	CJson_Utility::Load_Json(filePath, json_in);

	m_tUI_Info.fX = json_in["PostionX"];
	m_tUI_Info.fY = json_in["PostionY"];
	m_tUI_Info.fSizeX = json_in["SizeX"];
	m_tUI_Info.fSizeY = json_in["SizeY"];

}

// ImGui를 사용하여 이미지를 표시하는 함수
void CWindow_UITool::ShowImagePreview(const std::vector<unsigned char>& imageData, int width, int height)
{
	ImVec2 imageSize(width, height);
	ImGui::Image(reinterpret_cast<void*>(const_cast<unsigned char*>(imageData.data())), imageSize);
}

CWindow_UITool* CWindow_UITool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWindow_UITool* pInstance = new CWindow_UITool(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CWindow_UITool");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWindow_UITool::Free()
{
	__super::Free();
}