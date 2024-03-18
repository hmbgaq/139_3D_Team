#pragma once

#include "Base.h"

BEGIN(Engine)

class CLight_Manager final : public CBase
{
private:
	CLight_Manager();
	virtual ~CLight_Manager() = default;

public:
	HRESULT			Initialize();
	HRESULT			Add_Light(const LIGHT_DESC& LightDesc, _int& outLightIndex);
	class CLight*	Add_Light_AndGet(const LIGHT_DESC& LightDesc, _uint& outLightIndex);
	_bool			Remove_Light(const _uint& iIndex);
	_bool			Remove_AllLight();
	HRESULT			Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

	/* For. ShaderTool */
	class CLight*	Find_Light(const _int iIndex);
	class CLight*   Get_DirectionLight();
	void			Change_Light_Desc(const _int iIndex, LIGHT_DESC newDesc);

	/* For. Shadow */
	HRESULT			Set_ShadowLight(_uint iLevelIndex, _float4 vEye, _float4 vAt, _float4 vUp);
	HRESULT			Add_ShadowLight_View(_uint iLevelIndex, _vector vEye, _vector vAt, _vector vUp);
	HRESULT			Add_ShadowLight_Proj(_uint iLevelIndex, _float fFovAngleY, _float fAspectRatio, _float fNearZ, _float fFarZ);
	_float4x4		Get_ShadowLightViewMatrix(_uint iLevelIndex);
	_float4x4		Get_ShadowLightViewMatrix_Inverse(_uint iLevelIndex);
	_float4x4		Get_ShadowLightProjMatrix(_uint iLevelIndex);
	_float			Get_ShadowLightFar(_uint iLevelIndex);
	_float4			Get_ShadowLightPos(_uint iLevelIndex);

private:
	_float4x4					Identity_Matrix;
	list<class CLight*>			m_Lights;

	map<_uint, _float4x4>		m_ShadowLight_ViewMatrix;
	map<_uint, _float4x4>		m_ShadowLight_ProjMatrix;
	map<_uint, _float>			m_ShadowLight_Far;
	map<_uint, _float4>			m_ShadowLight_Pos;
	

public:
	static CLight_Manager* Create();
	virtual void Free() override;
};

END