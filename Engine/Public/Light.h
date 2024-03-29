#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CLight final : public CBase
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
	_float4				Get_LightDir() { return m_LightDesc.vDirection; }

	void				Set_LightDesc(LIGHT_DESC newDesc) {	m_LightDesc = newDesc; }
	void				Set_Diffuse(_float4 vDiffuse);
	void				Set_Specular(_float4 vSpecular);
	void				Set_Ambient(_float4 vAmbient);
	void				Set_LightIndex(_uint iLightIndex) { m_LightDesc.iLightIndex = iLightIndex;}
	void				Set_LightEnable(_bool bEnable) { m_LightDesc.bEnable = bEnable; }
	
	void				Set_LightType(LIGHT_DESC::TYPE eType) 
	{ 
		m_LightDesc.eType = eType;
	}


public:
	void				Decrease_GlobalIndex();

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