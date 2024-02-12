#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTransform;
class CCollider;
class CShader;
class CModel;
class CBone;
END

BEGIN(Client)

class CBody_Player final : public CGameObject
{
public:
	typedef struct tagBodyDesc
	{
		class shared_ptr<CTransform>		m_pParentTransform = { nullptr };
	}BODY_DESC;
public:
	CBody_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Player(const CBody_Player& rhs);
	virtual ~CBody_Player() = default;

public:
	class CBone* Get_BonePtr(const _char* pBoneName);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

public:
	void SetUp_Animation(_uint iAnimIndex);

private:
	shared_ptr<CShader>		m_pShaderCom = { nullptr };
	shared_ptr<CModel>		m_pModelCom = { nullptr };
	shared_ptr<CCollider>	m_pColliderCom = { nullptr };

private:
	shared_ptr<CTransform>	m_pParentTransform = { nullptr };
	_float4x4			m_WorldMatrix = {};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();


public:
	/* 원형객체를 생성한다. */
	static shared_ptr<CBody_Player> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	/* 사본객체를 생성한다. */
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;

	virtual void Free() override;
};

END