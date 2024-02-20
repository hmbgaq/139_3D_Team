#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Field final : public CVIBuffer
{
public:
	typedef struct tagVertex_Dynamic_Info
	{
		float		fX;
		float		fY;
		float		fZ;

		vector<VTXFIELD> vecVertexInfo;
	}DINFO;

private:
	CVIBuffer_Field(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Field(const CVIBuffer_Field& rhs);
	virtual ~CVIBuffer_Field() = default;

public:
	_bool					Get_Vertex(_uint _iIndex, VTXFIELD* _pOut);
	_bool					Get_Indices(_uint _iIndex, _uint3 * _pOut);
	vector<VTXFIELD>*		Get_Infos() { return &m_VertexInfo; }

public:
	_bool					Compute_MousePos(RAY _Ray, _matrix _WorldMatrix, _float3 * pOut);

public:
	virtual HRESULT			Initialize_Prototype();
	virtual HRESULT			Initialize(void* pArg) override;

public:
	void					Update(_vector _vMousePos, _float _fRadious, _float _fPower, _uint _iMode);

private:
	_uint					m_iNumVerticesX = 0, m_iNumVerticesZ = 0;
	_uint					m_iNumPrimitive;

	_float					m_fInterval = { 0.f };

	vector<_uint3>			m_Indices;
	vector<VTXFIELD>		m_VertexInfo;

public:
	static	CVIBuffer_Field* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END