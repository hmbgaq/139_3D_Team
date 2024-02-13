#include "stdafx.h"
#include "Window_AnimTool.h"

CWindow_AnimTool::CWindow_AnimTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CImgui_Window(pDevice, pContext)
{
}


HRESULT CWindow_AnimTool::Initialize()
{
	//! 현재는 특별한 기능없음. 추후 필요할 것 같아서 셋팅.
	if(FAILED(__super::Initialize()))
		return E_FAIL;


	return S_OK;
}

void CWindow_AnimTool::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	__super::Begin();

	ImGui::PushItemWidth(ImGui::GetFontSize() * -12);


	


	__super::End();
}

void CWindow_AnimTool::Render()
{

}


CWindow_AnimTool* CWindow_AnimTool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWindow_AnimTool* pInstance = new CWindow_AnimTool(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CWindow_AnimTool");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWindow_AnimTool::Free()
{
	__super::Free();
}