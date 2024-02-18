#pragma once

#include "Component.h"
//#include "AsTypes.h"

BEGIN(Engine)

class ENGINE_DLL CConstantBuffer final : public CBase
{
	using Super = CBase;
protected:
	CConstantBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CConstantBuffer(const CConstantBuffer& rhs) = delete;
	virtual ~CConstantBuffer() = default;

public:
	ID3D11Buffer* GetBuffer() { return m_pConstantBuffer; }

	HRESULT Initialize(const _uint & iSize);
	void CopyData(const void* data);

private:
	ID3D11Buffer* m_pConstantBuffer = nullptr;
	_uint					m_iSize = 0;

	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;

public:
	static CConstantBuffer* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _uint & iSize);

	virtual void Free();
};

END