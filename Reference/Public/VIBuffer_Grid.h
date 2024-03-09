#pragma once

#include "VIBuffer.h"

/* ±×¸®µå. */

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Grid final : public CVIBuffer
{
public:
	typedef struct tagGridBufferDesc
	{
		_float		fHeight = { 0.f };

	}GRID_BUFFER_DESC;

private:
	CVIBuffer_Grid(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CVIBuffer_Grid(const CVIBuffer_Grid& rhs);
	virtual ~CVIBuffer_Grid() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;


public:
	GRID_BUFFER_DESC* Get_Desc() { return &m_tDesc; }


public:
	_float Compute_Height(_fvector vLocalPos);


private:
	GRID_BUFFER_DESC	m_tDesc = {};

	_uint				m_iNumVerticesX = { 0 };
	_uint				m_iNumVerticesZ = { 0 };


public:
	static CVIBuffer_Grid* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END