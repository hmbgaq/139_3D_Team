#pragma once

#include "Base.h"

BEGIN(Engine)

class CLight final : public CBase
{
private:
	CLight();
	virtual ~CLight() = default;

public:
	HRESULT Initialize(const LIGHT_DESC& LightDesc);
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

public:
	const LIGHT_DESC&	Get_LightDesc() const { return m_LightDesc; }
	_float4				Get_Pos() { return m_LightDesc.vPosition; }					/* vPosition */
	_float				Get_CurrentRange() { return m_LightDesc.fRange; }			/* fRange */
	_uint				Get_LightIndex() { return m_LightDesc.iLightIndex; }

	void				Set_LightDesc(LIGHT_DESC newDesc) {	m_LightDesc = newDesc; }

public:
	void Decrease_GlobalIndex();

private:
	class CGameInstance*	m_pGameInstance;
	LIGHT_DESC				m_LightDesc;
	static _uint			g_iLightIndex;
	_float					g_fCamFar = {};

public:
	static CLight* Create(const LIGHT_DESC& LightDesc);
	virtual void Free() override; 
};

END