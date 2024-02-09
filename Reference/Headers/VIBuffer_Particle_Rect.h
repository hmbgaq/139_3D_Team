#pragma once

#include "VIBuffer_Instancing.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Particle_Rect final : public CVIBuffer_Instancing
{
public:
	typedef struct : public CVIBuffer_Instancing::INSTANCING_DESC
	{
		
	}PARTICLE_RECT_DESC;

private:
	CVIBuffer_Particle_Rect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Particle_Rect(const CVIBuffer_Particle_Rect& rhs);
	virtual ~CVIBuffer_Particle_Rect() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iNumInstance);
	virtual HRESULT Initialize(void* pArg) override;	
	// virtual void Update(_float fTimeDelta) override;


public:
	static CVIBuffer_Particle_Rect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumInstance);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END