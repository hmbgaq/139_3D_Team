#include "stdafx.h"
#include "Level_Logo.h"
#include "GameInstance.h"
#include "BackGround.h"
#include "Level_Loading.h"
#include "UI_Manager.h"

//#include <mfapi.h>
//#include <mfreadwrite.h>

CLevel_Logo::CLevel_Logo(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Logo::Initialize()
{
	FAILED_CHECK(Ready_Layer_BackGround(TEXT("Layer_BackGround")));

	//{

	//	IMFSourceReader* pSourceReader = NULL;
	//	IMFMediaType* pMediaType = NULL;

	//	HRESULT hr = MFCreateSourceReaderFromURL(TEXT("C:\\Users\\hmbga\\OneDrive\\3D_Personal\\Client\\Bin\\Resources\\Video\\starcraft.mp4"), NULL, &pSourceReader);
	//	
	//	FAILED_CHECK(hr);

	//	hr = pSourceReader->GetCurrentMediaType(0, &pMediaType);
	//	FAILED_CHECK(hr);

	//	// 비디오 텍스처 생성
	//	ID3D11Texture2D* pVideoTexture = NULL;

	//	D3D11_TEXTURE2D_DESC textureDesc;
	//	textureDesc.Width = MF_MT_FRAME_SIZE.Data1;
	//	textureDesc.Height = MF_MT_FRAME_SIZE.Data2;
	//	textureDesc.MipLevels = 1;
	//	textureDesc.ArraySize = 1;
	//	textureDesc.Format = DXGI_FORMAT_NV12;

	//	//pSourceReader->Release();
	//}

	Set_Filter();
	

	return S_OK;
}

void CLevel_Logo::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_TAB))
	{
		_int iCheckPoint = 0;
		iCheckPoint = MessageBox(g_hWnd, L"확인 선택 시 스테이지, 취소 선택 시 맵 툴이 실행됩니다.", L"실행 조건 확인", MB_OKCANCEL);

		// 확인 버튼을 눌렀을 때
		if (iCheckPoint == IDOK)
		{
			//FAILED_CHECK_RETURN(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY)), );
			FAILED_CHECK_RETURN(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_SNOWMOUNTAIN)),);
			//FAILED_CHECK_RETURN(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_SNOWMOUNTAINBOSS)), );
			//FAILED_CHECK_RETURN(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_INTRO_BOSS)),);

			/* Test 이거 맵 터짐 아무 쓸모 없는거 같은데 ㅋㅋ 쓰는 사람 있니? */
			/* 나 쓴다. 난안터짐 */
			//FAILED_CHECK_RETURN(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_INTRO)),);
		}
		else if (iCheckPoint == IDCANCEL)
		{
			FAILED_CHECK_RETURN(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_TOOL)),);
		}
	}
}

HRESULT CLevel_Logo::Render()
{
	SetWindowText(g_hWnd, TEXT("로고레벨입니다."));

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_BackGround(const wstring & strLayerTag)
{
	CBackGround::BACKGROUND_DESC		BackGroundDesc = {};

	BackGroundDesc.fX = g_iWinSizeX >> 1;
	BackGroundDesc.fY = g_iWinSizeY >> 1;
	BackGroundDesc.fSizeX = g_iWinSizeX;
	BackGroundDesc.fSizeY = g_iWinSizeY;
	BackGroundDesc.fRotationPerSec = XMConvertToRadians(90.f);
	BackGroundDesc.fSpeedPerSec = 10.f;

	FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_LOGO, strLayerTag, TEXT("Prototype_GameObject_BackGround"), &BackGroundDesc));

	return S_OK;
}

void CLevel_Logo::Set_Filter()
{
	m_pGameInstance->Check_Group((_uint)COLLISION_LAYER::MONSTER_ATTACK, (_uint)COLLISION_LAYER::PLAYER);
	m_pGameInstance->Check_Group((_uint)COLLISION_LAYER::PLAYER_ATTACK, (_uint)COLLISION_LAYER::MONSTER);
	m_pGameInstance->Check_Group((_uint)COLLISION_LAYER::PLAYER_ATTACK, (_uint)COLLISION_LAYER::WEAKNESS);
	m_pGameInstance->Check_Group((_uint)COLLISION_LAYER::PLAYER, (_uint)COLLISION_LAYER::MONSTER);
	m_pGameInstance->Check_Group((_uint)COLLISION_LAYER::MONSTER, (_uint)COLLISION_LAYER::MONSTER);
	m_pGameInstance->Check_Group((_uint)COLLISION_LAYER::PLAYER_PARRYING, (_uint)COLLISION_LAYER::MONSTER_ATTACK);

	//m_pGameInstance->Check_PhysXFilterGroup((_uint)PHYSX_COLLISION_LAYER::PLAYER, (_uint)PHYSX_COLLISION_LAYER::GROUND);
}

CLevel_Logo * CLevel_Logo::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_Logo*		pInstance = new CLevel_Logo(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Logo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Logo::Free()
{
	__super::Free();
}
