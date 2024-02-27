#pragma once

#include "VIBuffer_Instancing.h"

/* Á¡. */

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Point final : public CVIBuffer_Instancing
{
private:
	CVIBuffer_Point(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Point(const CVIBuffer_Point& rhs);
	virtual ~CVIBuffer_Point() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iNumInstance);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Update(_float fTimeDelta);

public:
	virtual void ReSet();

public:
	virtual _bool Write_Json(json & Out_Json)			override;
	virtual void  Load_FromJson(const json & In_Json)	override;


public:
	static CVIBuffer_Point* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumInstance);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END