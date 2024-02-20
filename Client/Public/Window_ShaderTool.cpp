#include "stdafx.h"
#include "Window_ShaderTool.h"

CWindow_ShaderTool::CWindow_ShaderTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CImgui_Window(pDevice, pContext)
{
}

HRESULT CWindow_ShaderTool::Initialize()
{	
	//! 현재는 특별한 기능없음. 추후 필요할 것 같아서 셋팅.
	FAILED_CHECK(__super::Initialize());

	return S_OK;
}

void CWindow_ShaderTool::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	__super::Begin();

	__super::End();
}

void CWindow_ShaderTool::Render()
{
}

CWindow_ShaderTool* CWindow_ShaderTool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWindow_ShaderTool* pInstance = new CWindow_ShaderTool(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CWindow_ShaderTool");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWindow_ShaderTool::Free()
{
	__super::Free();
}
