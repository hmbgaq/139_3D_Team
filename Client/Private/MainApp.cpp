#include "stdafx.h"
#include "MainApp.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Json_Utility.h"


CMainApp::CMainApp()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize()
{
	GRAPHIC_DESC		GraphicDesc = {};

	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.eWinMode = GRAPHIC_DESC::WINMODE_WIN;
	GraphicDesc.iBackBufferSizeX = g_iWinSizeX;
	GraphicDesc.iBackBufferSizeY = g_iWinSizeY;

	FAILED_CHECK(m_pGameInstance->Initialize_Engine(LEVEL_END, g_hInst, GraphicDesc, &m_pDevice, &m_pContext));

	FAILED_CHECK(Ready_Font());

	// FAILED_CHECK(Ready_Gara());

	FAILED_CHECK(Ready_Prototype_Component_ForStaticLevel());

	FAILED_CHECK(Open_Level(LEVEL_LOGO));

	return S_OK;
}

void CMainApp::Tick(_float fTimeDelta)
{
	m_pGameInstance->Tick_Engine(fTimeDelta);

	m_fTimeAcc += fTimeDelta;
}

HRESULT CMainApp::Render()
{
	m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 1.f, 1.f));
	m_pGameInstance->Clear_DepthStencil_View();

	m_pGameInstance->Render_Engine();

	++m_iNumRender;

	if (1.f <= m_fTimeAcc)
	{
		wsprintf(m_szFPS, TEXT("FPS:%d"), m_iNumRender);
		m_iNumRender = 0;
		m_fTimeAcc = 0.f;
	}
	
	// MakeSpriteFont "넥슨lv1고딕 Bold" /FontSize:30 /FastPack /CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 140.spritefont
	m_pGameInstance->Render_Font(TEXT("Font_Default"), m_szFPS, _float2(1100.f, 20.f), XMVectorSet(1.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Present();

	return S_OK;
}

HRESULT CMainApp::Ready_Font()
{
	FAILED_CHECK(m_pGameInstance->Add_Font(TEXT("Font_Default"), TEXT("../Bin/Resources/Fonts/139ex.spritefont")));
	FAILED_CHECK(m_pGameInstance->Add_Font(TEXT("Font_Arial"), TEXT("../Bin/Resources/Fonts/Arial.spritefont")));

	return S_OK;
}

HRESULT CMainApp::Ready_UITexture()
{
	json json_in;
	char filePath[MAX_PATH] = "../Bin/DataFiles/Data_UI/Texture_Info/Texture_Info";

	_int		iPathNum = 0;
	string		strFileName;
	string		strFilePath;

	CJson_Utility::Load_Json(filePath, json_in);

	for (auto& item : json_in.items())
	{
		json object = item.value();

		iPathNum = object["PathNum"];
		strFileName = object["FileName"];
		strFilePath = object["FilePath"];

		wstring wstrPrototag;
		m_pGameInstance->String_To_WString(strFileName, wstrPrototag);

		wstring wstrFilePath;
		m_pGameInstance->String_To_WString(strFilePath, wstrFilePath);

		FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_STATIC, wstrPrototag, CTexture::Create(m_pDevice, m_pContext, wstrFilePath)));
	}

	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVEL eStartLevelID)
{
	NULL_CHECK_RETURN(m_pGameInstance, E_FAIL);

	CLevel*		pLevel = CLevel_Loading::Create(m_pDevice, m_pContext, eStartLevelID);
	NULL_CHECK_RETURN(pLevel, E_FAIL);

	return m_pGameInstance->Open_Level(LEVEL_LOADING, pLevel);
}

HRESULT CMainApp::Ready_Prototype_Component_ForStaticLevel()
{
	/* For.Prototype_Component_VIBuffer_Rect*/
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), CVIBuffer_Rect::Create(m_pDevice, m_pContext)));

	/* For.Prototype_Component_Shader_VtxPosTex*/
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements)));
	//
	/* For.Prototype_Component_Shader_UI */ // + SH_Add
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_UI.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements)));
	//
	//
	/* For.Ready_UITexture */ // + SH_Add
	//if (FAILED(Ready_UITexture()))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Gara()
{
	//D3D11_BLEND_DESC			BlendDesc;
	//D3D11_DEPTH_STENCIL_DESC	DepthStencilDesc;
	//D3D11_RASTERIZER_DESC		RasterizerDesc;

	//RasterizerDesc.CullMode

	//ID3D11RasterizerState*		pRSState;
	//m_pDevice->CreateRasterizerState(RasterizerDesc, &pRSState);

	/*m_pContext->RSSetState();
	m_pContext->OMSetDepthStencilState();
	m_pContext->OMSetBlendState();*/

	/* 텍스쳐를 생성해보자. */
	ID3D11Texture2D*		pTexture2D = { nullptr };

	D3D11_TEXTURE2D_DESC	TextureDesc = {};

	TextureDesc.Width = 1024;
	TextureDesc.Height = 1024;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DYNAMIC;
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	TextureDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA		InitialData = {};

	_uint*		pPixels = new _uint[TextureDesc.Width * TextureDesc.Height];

	for (size_t i = 0; i < TextureDesc.Height; i++)
	{
		for (size_t j = 0; j < TextureDesc.Width; j++)
		{
			_uint		iIndex = (_uint)(i * TextureDesc.Width + j);

			pPixels[iIndex] = D3DCOLOR_ARGB(255, 0, 0, 0);
		}
	}

	InitialData.pSysMem = pPixels;
	InitialData.SysMemPitch = TextureDesc.Width * 4;


	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, &InitialData, &pTexture2D)))
		return E_FAIL;

	/*pPixels[0] = D3DCOLOR_ARGB(255, 255, 0, 0);*/

	for (size_t i = 0; i < TextureDesc.Height; i++)
	{
		for (size_t j = 0; j < TextureDesc.Width; j++)
		{
			_uint		iIndex = (_uint)i * TextureDesc.Width + (_uint)j;

			if(j < TextureDesc.Width * 0.5f)
				pPixels[iIndex] = D3DCOLOR_ARGB(255, 0, 0, 0);
			else
				pPixels[iIndex] = D3DCOLOR_ARGB(255, 255, 255, 255);
		}
	}

	/* 텍스쳐의 픽셀정보를 내 마음대로 조절해서 */
	D3D11_MAPPED_SUBRESOURCE		MappedSubResource = {};

	m_pContext->Map(pTexture2D, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubResource);

	memcpy(MappedSubResource.pData, pPixels, sizeof(_uint) * TextureDesc.Width * TextureDesc.Height);

	m_pContext->Unmap(pTexture2D, 0);


	/* 다시 파일로 저장하기위해서. */
	if (FAILED(SaveDDSTextureToFile(m_pContext, pTexture2D, TEXT("../Bin/Resources/Textures/Terrain/MyMask.dds"))))
		return E_FAIL;

	Safe_Delete_Array(pPixels);
	Safe_Release(pTexture2D);


	HANDLE		hFile = CreateFile(TEXT("../Bin/DataFiles/Navigation.dat"), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	_ulong		dwByte = { 0 };

	_float3		vPoints[3];

	vPoints[0] = _float3(0.f, 0.0f, 10.f);
	vPoints[1] = _float3(10.f, 0.0f, 0.0f);
	vPoints[2] = _float3(0.0f, 0.0f, 0.0f);
	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

	vPoints[0] = _float3(0.f, 0.0f, 10.f);
	vPoints[1] = _float3(10.f, 0.0f, 10.f);
	vPoints[2] = _float3(10.f, 0.0f, 0.0f);
	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

	vPoints[0] = _float3(0.f, 0.0f, 20.f);
	vPoints[1] = _float3(10.f, 0.0f, 10.f);
	vPoints[2] = _float3(0.0f, 0.0f, 10.f);
	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

	vPoints[0] = _float3(10.f, 0.0f, 10.f);
	vPoints[1] = _float3(20.f, 0.0f, 0.f);
	vPoints[2] = _float3(10.f, 0.0f, 0.0f);
	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

	CloseHandle(hFile);

	return S_OK;
}

CMainApp * CMainApp::Create()
{
	CMainApp*		pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CMainApp");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMainApp::Free()
{
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	/*  내 멤버를 정리하면. */
	Safe_Release(m_pGameInstance);

	CGameInstance::Release_Engine();

}

