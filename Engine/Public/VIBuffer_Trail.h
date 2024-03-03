#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class CGameObject;
class CTransform;
class CBone;


class ENGINE_DLL CVIBuffer_Trail : public CVIBuffer
{
public:
	typedef struct tagTrailBufferDesc
	{
		_bool		bTrailOn = { FALSE };

		_float3		vPos_0;
		_float3		vPos_1;
		_uint		iPass;

		_uint		iNumVertices;
		_uint		iMaxCnt;
		_uint		iVtxCnt;

		_float4		vLocalSwordLow;
		_float4		vLocalSwordHigh;

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