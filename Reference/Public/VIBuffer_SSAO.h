#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_SSAO final : public CVIBuffer
{
private:
	CVIBuffer_SSAO(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CVIBuffer_SSAO(const CVIBuffer_SSAO& rhs);
	virtual ~CVIBuffer_SSAO() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

private:
	_uint m_iNumPrimitives = 0;
	_uint m_iNumIndicesofPrimitive = 0;
	_uint m_iIndexSizeofPrimitive = 0;

public:
	static CVIBuffer_SSAO* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free();
};

END