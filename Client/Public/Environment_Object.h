#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CEnvironment_Object final : public CGameObject
{
public:
	typedef struct tagEnvironmentObjectDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_float4		vPos = { 0.f, 0.f, 0.f, 0.f };
		_uint		iShaderPassIndex = { 0 };
		wstring		strModelTag;
		_float4x4	WorldMatrix;

		_bool		bAnimModel = { false };
		_int		iPlayAnimationIndex = { 0 };
		
		_bool		bPreview = true; //! 미리보기용 오브젝트인지 확인

		_bool		bLightObject = false;
		LIGHT_DESC	tLightDesc = {};
		_int		iLightIndex = -1;

	}ENVIRONMENT_OBJECT_DESC;



private:
	CEnvironment_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CEnvironment_Object(const CEnvironment_Object& rhs);
	virtual ~CEnvironment_Object() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void		Priority_Tick(_float fTimeDelta) override;
	virtual void		Tick(_float fTimeDelta) override;
	virtual void		Late_Tick(_float fTimeDelta) override;
	virtual HRESULT		Render() override;
	virtual HRESULT		Render_Shadow() override;
	virtual HRESULT		Render_Ice() override;

public:
	virtual _bool		Write_Json(json& Out_Json) override;
	virtual void		Load_FromJson(const json& In_Json) override;

public:
	ENVIRONMENT_OBJECT_DESC* Get_EnvironmentDesc() { return &m_tEnvironmentDesc; }
	wstring&			Get_ModelTag() { return m_tEnvironmentDesc.strModelTag; }
	_bool				Is_AnimModel() { return m_tEnvironmentDesc.bAnimModel; }
	
	CModel*				Get_ModelCom() { return m_pModelCom; }
	
public:
	_int				Get_AnimationIndex() { return m_tEnvironmentDesc.iPlayAnimationIndex; }
	void				Set_AnimationIndex(_uint iAnimIndex);
	void				Set_ColliderRender(_bool bColliderRender) { m_bColliderRender = bColliderRender;}
	void				Set_ShaderPassIndex(_int iShaderPassIndex) { m_tEnvironmentDesc.iShaderPassIndex = iShaderPassIndex;}

#ifdef _DEBUG
public: //! For.Tool
	virtual _bool		Picking(_Out_ _float3* vPickedPos) override;
	_bool				Picking_VerJSY(RAY* pRay, _Out_ _float3* vPickedPos);

#endif 

public:
	void					Start_Environment_Animation() { m_bPlay = true; }

private:
	CShader*					m_pShaderCom = { nullptr };	
	CModel*						m_pModelCom = { nullptr };
	CCollider*					m_pPickingCollider = nullptr;

private:
	ENVIRONMENT_OBJECT_DESC		m_tEnvironmentDesc = {};
	_bool						m_bPlay = false;
	_bool						m_bColliderRender = false;

	_uint						m_iCurrentLevel = LEVEL_TOOL;

private:
	HRESULT				Ready_Components();
	HRESULT				Bind_ShaderResources();

	/* 고드름용 - 소영 작업중 */
	_int		iCheckMeshNum = 0;
	_int		m_iIceMeshNumber = 0;
	_float4		m_vCamPosition = {};
	_float		m_fCamFar = {};

	_bool		bRenderIce = { false };
	_bool		bIcarusTexture = { false };
	CTexture*	m_pIceNoise = { nullptr };
	CTexture*	m_pIceDiffuse = { nullptr };
	CTexture*	m_pRADTexture = { nullptr };

	/* 렌더링용 */
private:

public:
	/* 원형객체를 생성한다. */
	static CEnvironment_Object* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;

	virtual void Free() override;

};

END