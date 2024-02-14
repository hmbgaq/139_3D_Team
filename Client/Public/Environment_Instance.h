#pragma once

#include "Client_Defines.h"
#include "LandObject.h"

BEGIN(Engine)
class CShader;
class CCollider;
class CModel;
class CVIBuffer_Model_Instance;
END

BEGIN(Client)

class CEnvironment_Instance final : public CLandObject
{
public:
	typedef struct tagEnvironment_Desc : public CLandObject::LANDOBJECT_DESC
	{
		wstring strModelTag = {};
		_uint	iNumInstance = { 0 };
		_uint	iShaderPassIndex = { 1 };
	}ENVIRONMENT_INSTANCE_DESC;

private:
	CEnvironment_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEnvironment_Instance(const CEnvironment_Instance& rhs);
	virtual ~CEnvironment_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*					m_pShaderCom = { nullptr };	
	CModel*						m_pModelCom = { nullptr };
	CVIBuffer_Model_Instance*	m_pInstanceModelCom = { nullptr };

	ENVIRONMENT_INSTANCE_DESC	m_tInstanceDesc = {};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();


public:
	/* 원형객체를 생성한다. */
	static CEnvironment_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;

	virtual void Free() override;
};

END