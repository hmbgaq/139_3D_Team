#pragma once

#include "Client_Defines.h"
#include "Body.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
class CBone;
END

BEGIN(Client)

class CBody_Player final : public CBody
{
public:
// 	typedef struct tagBodyDesc
// 	{
// 		class CTransform*		m_pParentTransform = { nullptr };
// 	}BODY_DESC;
private:
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

// private:
// 	CShader*			m_pShaderCom = { nullptr };	
// 	CModel*				m_pModelCom = { nullptr };
// 	CCollider*			m_pColliderCom = { nullptr };
// 
// private:
// 	class CTransform*	m_pParentTransform = { nullptr };
// 	_float4x4			m_WorldMatrix = {};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();


public:
	/* 원형객체를 생성한다. */
	static CBody_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;

	virtual void Free() override;
};

END