#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CCamera_Dynamic final : public CCamera
{
public:
	typedef struct tagDynamicCameraDesc : public tagCameraDesc
	{
		_float		fMouseSensor = 0.0f;
	} DYNAMIC_CAMERA_DESC;

private:
	CCamera_Dynamic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CCamera_Dynamic(const CCamera_Dynamic& rhs);
	virtual ~CCamera_Dynamic() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

public:
	virtual HRESULT Ready_Components();
private:
	_float				m_fMouseSensor = { 0.0f };

	/* 소영 추가 */
public:
	void Set_ActionSpeed(_float fSpeed) { fAction_Speed = fSpeed; }

private:
	HRESULT Ready_CameraAction(_float fTimeDelta);

	_float		fAction_X		= 0.9f;
	_float		fAction_Speed	= 0.2f;
	_float4x4	 m_vBookMark	= {};

public:
	static CCamera_Dynamic* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool();
	virtual void Free() override;

};

END