#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CCollider;
class CModel;
END

BEGIN(Client)

class CMonster final : public CGameObject
{
public:
	typedef struct tagMonsterDesc : public CGameObject::tagGameObjectDesc
	{
		_float4x4	WorldMatrix = XMMatrixIdentity();
		_bool		bPreview = false;
		wstring		strProtoTypeTag = L""; //! 파싱용
	}MONSTER_DESC;

private:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;

public:
	MONSTER_DESC*	Get_MonsterDesc() { return &m_tMonsterDesc;}


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void	OnCollisionEnter(CCollider* other)	override;
	virtual void	OnCollisionStay(CCollider* other)	override;
	virtual void	OnCollisionExit(CCollider* other)	override;

public:
	virtual _bool		Write_Json(json& Out_Json) override;
	virtual void		Load_FromJson(const json& In_Json) override;

private:
	CModel*				m_pModelCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };	
	CCollider*			m_pColliderCom = { nullptr };
	CTexture*			m_pDissolveTexCom = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	_int					m_iRenderPass = {};
	_float					m_fTimeDelta;
	MONSTER_DESC			m_tMonsterDesc = {};
public:
	/* 원형객체를 생성한다. */
	static CMonster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;

	virtual CGameObject* Pool() override;

	virtual void Free() override;

	
};

END