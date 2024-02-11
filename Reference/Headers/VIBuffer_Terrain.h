#pragma once

#include "VIBuffer.h"

/* »ç°¢Çü. */

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
private:
	CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs);
	virtual ~CVIBuffer_Terrain() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring& strHeightMapFilePath);
	virtual HRESULT Initialize(void* pArg) override;

public:
	_float Compute_Height(_fvector vLocalPos);
	void Culling(_fmatrix WorldMatrix);

private:
	_uint				m_iNumVerticesX = { 0 };
	_uint				m_iNumVerticesZ = { 0 };

	class CQuadTree*	m_pQuadTree = { nullptr };

public:
	static CVIBuffer_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strHeightMapFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END