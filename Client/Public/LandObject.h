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
		CVIBuffer_Terrain*		pTerrainBuffer;
		CTransform*				pTerrainTransform;
	}LANDOBJECT_DESC;

protected:
	CLandObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
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
	class CVIBuffer_Terrain*			m_pVIBuffer_Terrain = { nullptr };
	class CTransform*					m_pTransform_Terrain = { nullptr };

protected:
	HRESULT SetUp_OnTerrain(CTransform* pTargetTransform);
	
public:
	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END