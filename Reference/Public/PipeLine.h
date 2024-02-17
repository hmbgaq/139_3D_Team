#pragma once

/* 인게임 내 객체들을 그릴때 공통으로 사용하기위한 뷰, 투영행렬을 보관한다. */
#include "Base.h"

BEGIN(Engine)

class CPipeLine final : public CBase
{
public:
	enum D3DTRANSFORMSTATE { D3DTS_VIEW, D3DTS_PROJ, D3DTS_END };
private:
	CPipeLine();
	virtual ~CPipeLine() = default;

public:
	void Set_Transform(D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix);
	void Set_Transform(D3DTRANSFORMSTATE eState, _float4x4 TransformMatrix);

public:
	_matrix Get_TransformMatrix(D3DTRANSFORMSTATE eState);
	_float4x4 Get_TransformFloat4x4(D3DTRANSFORMSTATE eState);
	_matrix Get_TransformMatrixInverse(D3DTRANSFORMSTATE eState);
	_float4x4 Get_TransformFloat4x4Inverse(D3DTRANSFORMSTATE eState);

public: /* 카메라 관련 */
	_float4 Get_CamPosition();
	_float4 Get_CamSetting();

	_float	Get_CamFar() { return m_fCamFar; };

public:
	_float4x4 Get_PreViewMatrix();

public:
	HRESULT Initialize();
	void Tick();


private:
	_float4x4			m_Transform[D3DTS_END];
	_float4x4			m_Transform_Inverse[D3DTS_END];

	_float4				m_vCamPosition;


	_float4				m_vPreCamPosition;
	_float4				m_vPreCamQuaternion;
	_float4x4			m_PreViewMatrix;
	_float				m_fCamFar = 1000.f;

public:
	static CPipeLine* Create();
	virtual void Free() override;
};

END