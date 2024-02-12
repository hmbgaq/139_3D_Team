#pragma once

#include "Base.h"

BEGIN(Engine)

class CShader;
class CVIBuffer_Rect;

class CLight final : public CBase
{
private:
	CLight();
	virtual ~CLight() = default;

public:
	HRESULT Initialize(const LIGHT_DESC& LightDesc);
	HRESULT Render(shared_ptr<CShader> pShader, shared_ptr<CVIBuffer_Rect> pVIBuffer);

public:
	const LIGHT_DESC&	Get_LightDesc() const { return m_LightDesc; }
	_float4				Get_Pos() { return m_LightDesc.vPosition; }					/* vPosition */
	_float				Get_CurrentRange() { return m_LightDesc.fRange; }			/* fRange */
	_uint				Get_LightIndex() { return m_LightDesc.iLightIndex; }


private:
	LIGHT_DESC			m_LightDesc;
	static _uint		g_iLightIndex;

public:
	static CLight* Create(const LIGHT_DESC& LightDesc);
	virtual void Free() override; 
};

END