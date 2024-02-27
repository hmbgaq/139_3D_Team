#pragma once

/* 클라이언트 개발자가 만들고자하는 카메라들의 공통적인 부모가 되기위한 클래스다. */
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	typedef struct tagCameraDesc : public tagGameObjectDesc
	{
		_float4		vEye, vAt;
		_float		fFovy, fAspect, fNear, fFar;
	}CAMERA_DESC;

protected:
	CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CCamera(const CCamera& rhs);
	virtual ~CCamera() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;	

public:
	void	Update_Cascade();

public:
	void	Set_Enable(_bool _bEnable) { m_bEnable = _bEnable; }


protected:
	_float	m_fFovy			= { 0.f };
	_float	m_fAspect		= { 0.0f };
	_float	m_fNear			= { 0.0f };
	_float	m_fFar			= { 0.0f };

	_bool	m_bEnable = false; //! true일 경우에만 뷰 투영 셋팅하게~ 씅승용


	_float						m_fCascadeEnd[4]; // 사다리꼴 절두체 끝부분 
public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END