#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instancing abstract : public CVIBuffer
{
public:
	typedef struct
	{
		_float3		vCenter;
		_float		fRange;
		_float2		vSpeed;
		_float2		vScale;
		_float2		vLifeTime;
	}INSTANCING_DESC;

protected:
	CVIBuffer_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Instancing(const CVIBuffer_Instancing& rhs);
	virtual ~CVIBuffer_Instancing() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Bind_VIBuffers() override;
	virtual void Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	virtual void ReSet();

protected:
	ID3D11Buffer* m_pVBInstance = { nullptr };
	_uint						m_iInstanceStride = { 0 };
	_uint						m_iNumInstance = { 0 };
	_uint						m_iIndexCountPerInstance = { 0 };

protected:
	random_device				m_RandomDevice;
	mt19937_64					m_RandomNumber;

protected:
	_float* m_pSpeeds		= { nullptr };
	_float* m_pLifeTimes	= { nullptr };
	INSTANCING_DESC				m_InstancingDesc;
	_float						m_fTimeAcc = { 0.0f };


public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END