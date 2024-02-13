#include "stdafx.h"
#include "Window_UITool.h"
#include "UI_Anything.h"

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

	// 이미지 로드 Test
	bool ret = LoadTextureFromFile("../Bin/Resources/Textures/UI/Textures/EnemyHUD/Small/ui_enemy_hp_big_0.png", &my_texture, &my_image_width, &my_image_height);
	IM_ASSERT(ret);

	return S_OK;
}

void CWindow_UITool::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	/* Test */
	// 윈도우 창 크기 변경시 변경된 값을 마우스위치값을 이용해 창 사이즈를 바꿔줘야 할 것 같다.
	//ImGui::IsMouseDown();
	//m_tImGuiDESC.vWindowSize; 
	
	__super::Begin();


	//// 이미지 미리보기 호출
	//if (!imageData.empty())
	//{
	//	ShowImagePreview(imageData, iWidth, iHeight);
	//}

	// Test
	UI_List(fTimeDelta);

	ImGui::Text("UI_Test");



	/* Default : LastNumber */
	UI_ToolTip(fTimeDelta); // Tip : 툴팁은 오버레이시 모든 출력중 가장 마지막에 호출되어야한다. (안그러면 다른 녀석들이 툴팁에 밀려서 출력됨)
	__super::End();
}

void CWindow_UITool::Render()
{

}

void CWindow_UITool::UI_List(_float fTimeDelta)
{
	/* Test Value */
	tUI_Info.strName = "Test UI List";
	tUI_Info.iNum = 1;
	tUI_Info.fNum = 1.5f;

	// 정보를 목록으로 표시
	if (ImGui::TreeNode(tUI_Info.strName.c_str())) {
		ImGui::Text(u8"값1 : %d", tUI_Info.iNum);
		ImGui::Text(u8"값2 : %.2f", tUI_Info.fNum);

		// 버튼
		if (ImGui::Button(u8"버튼")) 
		{

		}

		ImGui::TreePop();
	}
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

	// 마우스가 해당 위치 위에 있는 경우에만 툴팁 표시 (마우스 오버)
	if (ImGui::IsMouseHoveringRect(
		ImVec2(fTestX - fHoverRangeX, fTestY - fHoverRangeY),
		ImVec2(fTestX + fHoverRangeX, fTestY + fHoverRangeY)))
	{

		// 표시할 정보
		ImGui::SetCursorScreenPos(ImVec2(fTestX + fScreenPosX, fTestY - fScreenPosY));
		ImGui::BeginTooltip();
		ImGui::Text("pointer = %p", my_texture);
		ImGui::Text("size = %d x %d", my_image_width, my_image_height);
		ImGui::Image((void*)my_texture, ImVec2(my_image_width, my_image_height));

		ImGui::EndTooltip(); // 툴팁 종료
	}
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