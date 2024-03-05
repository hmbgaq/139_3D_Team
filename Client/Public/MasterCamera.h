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
		SpringCamera,
		CutSceneCamera,

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


private:
	vector<CCamera*> m_Cameras;
	CameraType m_eCameraType = { CameraType::CameraType_End };

	class CPlayer* m_pPlayer = { nullptr };


public:
	static CMasterCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;
	virtual void Free() override;
	
};

END