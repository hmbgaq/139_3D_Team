#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CEffect_Object final : public CGameObject
{
	
protected:
	CEffect_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Object(const CEffect_Object& rhs);
	virtual ~CEffect_Object() = default;

public:
	virtual HRESULT Initialize_Prototype()				override;
	virtual HRESULT Initialize(void* pArg)				override;
	virtual void	Priority_Tick(_float fTimeDelta)	override;
	virtual void	Tick(_float fTimeDelta)				override;
	virtual void	Late_Tick(_float fTimeDelta)		override;
	virtual HRESULT Render()							override;


public:
	/* 원형객체를 생성한다. */
	static CEffect_Object* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;

	virtual void Free() override;
};

END