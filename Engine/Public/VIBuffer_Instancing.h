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
		_float2		vAddScale;

		_float2		vRotX = { 0.0f, 360.f };
		_float2		vRotY = { 0.0f, 360.f };
		_float2		vRotZ = { 0.0f, 360.f };

		_float4		vColor;

		_float2		vLifeTime;

		_float		fAcceleration = { 1.f };
		_float		fAccPosition = { 0.1f };

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

public:
	INSTANCING_DESC* Get_Desc() { return &m_InstancingDesc; }

	_float			 Get_TimePosition() { return m_fTimeAcc; }
	void			 Set_TimePosition(_float fTimePosition) { m_fTimeAcc = fTimePosition; }

	_bool			 Is_Loop() { return m_bLoop; }
	void			 Set_Loop(_bool bLoop) { m_bLoop = bLoop; }

	_bool			 Is_Play() { return m_bIsPlay; }
	void			 Set_Play(_bool bPlay) { m_bIsPlay = bPlay; }

	_bool			 Is_ReversePlay() { return m_bReverse; }
	void			 Set_ReversePlay(_bool bReverse) { m_bReverse = bReverse; }

	_float2			Get_AddScale() { return m_InstancingDesc.vAddScale; }
	void			Set_AddScale(_float fX, _float fY) { m_InstancingDesc.vAddScale = _float2(fX, fY); }

	_float			Get_RotX() { return m_InstancingDesc.vRotX.y; }
	void			Set_RotX(_float fRotX) { m_InstancingDesc.vRotX.y = fRotX; }

	_float			Get_RotY() { return  m_InstancingDesc.vRotY.y; }
	void			Set_RotY(_float fRotY) { m_InstancingDesc.vRotY.y = fRotY; }

	_float			Get_RotZ() { return m_InstancingDesc.vRotZ.y; }
	void			Set_RotZ(_float fRotZ) { m_InstancingDesc.vRotZ.y = fRotZ; }
	void			Set_RotZ_start(_float fRotZ) { m_InstancingDesc.vRotZ.x = fRotZ; }

	_float			Get_Acceleration() { return m_InstancingDesc.fAcceleration; }
	void			Set_Acceleration(_float fAcceleration) { m_InstancingDesc.fAcceleration = fAcceleration; }

	_float			Get_AccPosition() { return m_InstancingDesc.fAccPosition; }
	void			Set_AccPosition(_float fAccPosition) { m_InstancingDesc.fAccPosition = fAccPosition; }


protected:
	ID3D11Buffer*				m_pVBInstance = { nullptr };
	_uint						m_iInstanceStride = { 0 };
	_uint						m_iNumInstance = { 0 };	
	_uint						m_iIndexCountPerInstance = { 0 };

protected:
	random_device				m_RandomDevice;
	mt19937_64					m_RandomNumber;

protected:
	_float*						m_pSpeeds		= { nullptr };
	_float*						m_pLifeTimes	= { nullptr };
	INSTANCING_DESC				m_InstancingDesc;

	_float						m_fTimeAcc	= { 0.0f };

	_bool						m_bLoop		= { TRUE };
	_bool						m_bIsPlay	= { TRUE };
	_bool						m_bReverse	= { FALSE };

protected:
	_float3						m_vRotation;


public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END