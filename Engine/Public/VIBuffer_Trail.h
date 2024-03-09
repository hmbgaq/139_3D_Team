#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

//class CGameObject;
//class CTransform;


class ENGINE_DLL CVIBuffer_Trail : public CVIBuffer
{
public:
	typedef struct tagTrailBufferDesc
	{
		_float3		vPos_0	= { 0.f, 0.f, 0.f };
		_float3		vPos_1	= { 0.f, 0.f, 1.f };
		_uint		iMaxCnt = { 16 };

		_uint		iNumVertices	= { 0 };
		_uint		iVtxCnt			= { 0 };

		_uint		iLerpPointNum = { 12 };
		_int		iCatMullRomIndex[4];

	}TRAIL_BUFFER_DESC;


private:
	CVIBuffer_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Trail(const CVIBuffer_Trail& rhs);
	virtual ~CVIBuffer_Trail() = default;

protected:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	void Update(_float _fTimeDelta, _matrix _ParentMatrix);
	void Tick(_float fTimeDelta);

public:
	void Reset_Points(_matrix _ParentMatrix);

public:
	virtual _bool	Write_Json(json& Out_Json)			override;
	virtual void	Load_FromJson(const json& In_Json)	override;


/* For.Desc */
public:
	TRAIL_BUFFER_DESC* Get_Desc() { return &m_tBufferDesc; }

private:
	TRAIL_BUFFER_DESC	m_tBufferDesc = {};



public:
	static CVIBuffer_Trail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	void Free();
};

END