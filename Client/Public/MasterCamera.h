#pragma once

#include "Client_Defines.h"
#include "Camera.h"

#include "SpringCamera.h"

BEGIN(Client)

class CMasterCamera : public CGameObject
{
public:
	enum CameraType
	{
		DynamicCamera,//툴 및 테스트용 카메라 
		SpringCamera,//플레이어 에 붙어 있는 카메라 
		CutSceneCamera, // 맵및 보스 전에서 보일 카메라 

		CameraType_End
	};

private:
	CMasterCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CMasterCamera(const CMasterCamera& rhs);
	virtual ~CMasterCamera() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

public:
	void Set_CameraType(CameraType _eCameraType) { m_eCameraType = _eCameraType; }
	CameraType Get_CameraType() { return m_eCameraType; }
	vector<CCamera*> Get_vectorCamera() { return m_Cameras; }

	class CCamera_Dynamic* Get_DynamicCamera();
	class CSpringCamera* Get_SpringCamera();

private:
	vector<CCamera*>	m_Cameras;
	CameraType			m_eCameraType = { CameraType::CameraType_End };

	class CPlayer*		m_pPlayer = { nullptr };
private:
	_bool				m_bfirstCheck = true;
	_bool				m_bTickCamera = true;
public:
	static CMasterCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;
	virtual void Free() override;
	
};

END