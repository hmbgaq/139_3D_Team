#pragma once

#include "Client_Defines.h"
#include "AlphaObject.h"

BEGIN(Client)

class CEffect_Object abstract : public CAlphaObject
{
	
protected:
	CEffect_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
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
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END