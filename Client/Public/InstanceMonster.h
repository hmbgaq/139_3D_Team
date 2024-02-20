#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
class CTexture;
class CVIBuffer_AnimModel_Instance;
END


BEGIN(Client)

class CInstanceMonster final : public CGameObject
{


private:
	CInstanceMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInstanceMonster(const CInstanceMonster& rhs);
	virtual ~CInstanceMonster() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void		Priority_Tick(_float fTimeDelta) override;
	virtual void		Tick(_float fTimeDelta) override;
	virtual void		Late_Tick(_float fTimeDelta) override;
	virtual HRESULT		Render() override;

private:
	HRESULT				Ready_Render(_uint iSize);
	HRESULT				Render_Instance(_uint iSize);

private:
	HRESULT				Ready_Components();
	HRESULT				Bind_ShaderResources();

	HRESULT				Ready_Instance();
		HRESULT				Create_AnimationTexture();
	

	void				Add_InstanceData(_uint iSize, _uint& iIndex);

private:
	_int					m_iRenderPass = { 0 };
	_float					m_fTimeDelta;

private:
	CModel*							m_pModelCom = { nullptr };
	CShader*						m_pShaderCom = { nullptr };
	CCollider*						m_pColliderCom { nullptr };
	CTexture*						m_pDissolveTexCom = { nullptr };
	CVIBuffer_AnimModel_Instance* 	m_pInstanceModelCom = { nullptr };

	
	ANIMMODEL_INSTANCE_DESC m_tInstanceDesc = {};

public:
	static CInstanceMonster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END

