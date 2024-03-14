#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END

BEGIN(Client)

class CEnvironment_Interact final : public CGameObject
{
public:
	enum INTERACT_TYPE { INTERACT_JUMP100, INTERACT_JUMP200, INTERACT_JUMP300, INTERACT_VAULT100, INTERACT_VAULT200, INTERACT_END };
	enum INTERACT_STATE { INTERACTSTATE_LOOP, INTERACTSTATE_ONCE, INTERACTSTATE_END };

public:
	typedef struct tagEnvironmentInteractObjectDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_float4			vPos = { 0.f, 0.f, 0.f, 0.f };
		_uint			iShaderPassIndex = { 0 };
		wstring			strModelTag;
		_float4x4		WorldMatrix;

		INTERACT_TYPE	eInteractType = INTERACT_END;
		INTERACT_STATE  eInteractState = INTERACTSTATE_END;
		_float3			vColliderSize = {};
		_float3			vColliderCenter = {};
		_int			iPlayAnimationIndex = { 0 };

		_bool			bAnimModel = { false };
		_bool			bPreview = true; //! 미리보기용 오브젝트인지 확인
	}ENVIRONMENT_INTERACTOBJECT_DESC;

private:
	CEnvironment_Interact(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CEnvironment_Interact(const CEnvironment_Interact& rhs);
	virtual ~CEnvironment_Interact() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void		Priority_Tick(_float fTimeDelta) override;
	virtual void		Tick(_float fTimeDelta) override;
	virtual void		Late_Tick(_float fTimeDelta) override;
	virtual HRESULT		Render() override;
	virtual HRESULT		Render_Shadow() override;

public:
	virtual _bool		Write_Json(json& Out_Json) override;
	virtual void		Load_FromJson(const json& In_Json) override;

public:
	ENVIRONMENT_INTERACTOBJECT_DESC*	Get_EnvironmentDesc() { return &m_tEnvironmentDesc; }
	wstring&							Get_ModelTag() { return m_tEnvironmentDesc.strModelTag; }
	_bool								Is_AnimModel() { return m_tEnvironmentDesc.bAnimModel; }
	
#ifdef _DEBUG
	void								Set_ColliderSize(_float3 vColliderSize);
	void								Set_ColliderCenter(_float3 vColliderCenter);
#endif // _DEBUG


#ifdef _DEBUG
public: //! For.Tool
	virtual _bool				Picking(_Out_ _float3* vPickedPos) override;

#endif 

public:
	void						Start_Environment_Animation() { m_bPlay = true; }

public:
	void								Interact();


private:
	CShader*							m_pShaderCom = { nullptr };	
	CModel*								m_pModelCom = { nullptr };
	CCollider*							m_pColliderCom = { nullptr };

	_int								m_iCurrentLevelIndex = -1;

private:
	ENVIRONMENT_INTERACTOBJECT_DESC		m_tEnvironmentDesc = {};
	_bool								m_bPlay = false;
	_bool								m_bInteract = false;
	
	_bool								m_bFindPlayer = false;

private:
	CPlayer*						    m_pPlayer = { nullptr };

private:
	HRESULT						Ready_Components();
	HRESULT						Ready_InteractCollider(INTERACT_TYPE eInteractType);
	HRESULT						Bind_ShaderResources();


public:
	/* 원형객체를 생성한다. */
	static CEnvironment_Interact* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;

	virtual void Free() override;

};

END