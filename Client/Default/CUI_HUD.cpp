#include "stdafx.h"
#include "CUI_HUD.h"

HRESULT CUI_HUD::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    return S_OK;
}

HRESULT CUI_HUD::Initialize(void* pArg)
{
    __super::Initialize(pArg);

    return S_OK;
}

void CUI_HUD::Priority_Tick(_float fTimeDelta)
{
    __super::Priority_Tick(fTimeDelta);
}

void CUI_HUD::Tick(_float fTimeDelta)
{

    __super::Tick(fTimeDelta);
}

void CUI_HUD::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
}

HRESULT CUI_HUD::Render()
{
    __super::Render();

    return S_OK;
}
