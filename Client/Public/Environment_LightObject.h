#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CEnvironment_LightObject final : public CGameObject
{
public:
	typedef struct tagEnvironmentLightObjectDesc : public CGameObject::GAMEOBJECT_DESC 
	{

		_float4		vPos = { 0.f, 0.f, 0.f, 0.f };
		_uint		iShaderPassIndex = { 0 };
		wstring		strModelTag;
		_float4x4	WorldMatrix;

		_bool		bAnimModel = { false };
		_int		iPlayAnimationIndex = { 0 };
		
		LIGHT_DESC  LightDesc = {};
		_int		iLightIndex = { 0 };
		_bool		bPreview = true; //! 미리보기용 오브젝트인지 확인
	}ENVIRONMENT_LIGHTOBJECT_DESC;



private:
	CEnvironment_LightObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CEnvironment_LightObject(const CEnvironment_LightObject& rhs);
	virtual ~CEnvironment_LightObject() = default;

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
	ENVIRONMENT_LIGHTOBJECT_DESC* Get_EnvironmentDesc() { return &m_tEnvironmentDesc; }
	wstring&			Get_ModelTag() { return m_tEnvironmentDesc.strModelTag; }
	_bool				Is_AnimModel() { return m_tEnvironmentDesc.bAnimModel; }
	
	CModel*				Get_ModelCom() { return m_pModelCom; }
	
public:
	void				Set_ColliderRender(_bool bColliderRender) { m_bColliderRender = bColliderRender;}

#ifdef _DEBUG
public: //! For.Tool
	virtual _bool		Picking(_Out_ _float3* vPickedPos) override;
	_bool				Picking_VerJSY(RAY* pRay, _Out_ _float3* vPickedPos);

#endif 

public:
	void				Start_Environment_Animation() { m_bPlay = true; }

private:
	CShader*			m_pShaderCom = { nullptr };	
	CModel*				m_pModelCom = { nullptr };
	CCollider*			m_pPickingCollider = nullptr;

private:
	ENVIRONMENT_LIGHTOBJECT_DESC	m_tEnvironmentDesc = {};
	_bool							m_bPlay = false;
	_bool							m_bColliderRender = false;


	_uint					m_iCurrentLevel = LEVEL_TOOL;

private:
	HRESULT				Ready_Components();
	HRESULT				Bind_ShaderResources();


public:
	/* 원형객체를 생성한다. */
	static CEnvironment_LightObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;

	virtual void Free() override;

};

END