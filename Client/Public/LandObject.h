#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CVIBuffer_Terrain;
class CTransform;
END
BEGIN(Client)

class CLandObject abstract : public CGameObject
{
public:
	typedef struct tagLandObjectDesc: public CGameObject::GAMEOBJECT_DESC
	{
		shared_ptr<CVIBuffer_Terrain>		pTerrainBuffer;
		shared_ptr<CTransform>				pTerrainTransform;
	}LANDOBJECT_DESC;

public:
	CLandObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLandObject(const CLandObject& rhs);
	virtual ~CLandObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	shared_ptr<CVIBuffer_Terrain>			m_pVIBuffer_Terrain = { nullptr };
	shared_ptr<CTransform>					m_pTransform_Terrain = { nullptr };

protected:
	HRESULT SetUp_OnTerrain(shared_ptr<CTransform> pTargetTransform);
	
public:
	/* 사본객체를 생성한다. */
	virtual shared_ptr<CGameObject> Clone(void* pArg) = 0;
	virtual void Free() override;
};

END