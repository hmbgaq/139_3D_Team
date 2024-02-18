#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CCollider;
class CModel;
class CVIBuffer_Environment_Model_Instance;
END

BEGIN(Client)

class CEnvironment_Instance final : public CGameObject
{
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

public:
	virtual _bool	Write_Json(json& Out_Json) override;
	virtual void	Load_FromJson(const json& In_Json) override;

public:
	void		Update(INSTANCE_INFO_DESC InstanceDesc, _int iIndex);

public:
	MAPTOOL_INSTANCE_DESC&					Get_InstanceDesc() { return m_tInstanceDesc; }
	wstring									Get_ModelTag() { return m_tInstanceDesc.strModelTag;}

	vector<INSTANCE_INFO_DESC>*				Get_InstanceInfoDesc() { return &m_tInstanceDesc.vecInstanceInfoDesc; }

	INSTANCE_INFO_DESC*						Get_InstanceInfo(_uint iIndex) { return &m_tInstanceDesc.vecInstanceInfoDesc[iIndex]; }

private:
	CShader*								m_pShaderCom = { nullptr };	
	CModel*									m_pModelCom = { nullptr };
	CVIBuffer_Environment_Model_Instance*	m_pInstanceModelCom = { nullptr };

	MAPTOOL_INSTANCE_DESC					m_tInstanceDesc;

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