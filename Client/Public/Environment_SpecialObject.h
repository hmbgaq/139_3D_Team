#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "UI_Weakness.h"

BEGIN(Engine)
class CShader;
class CModel;
class CTexture;
END

BEGIN(Client)
class CEnvironment_LightObject;
//class CUI_Weakness;

class CEnvironment_SpecialObject final : public CGameObject
{

public:
enum SPECIALTYPE { SPECIAL_SIGNAL, SPECIAL_TRACKLEVER, SPECIAL_END };

public:
	typedef struct tagEnvironmentSpecialObjectDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_uint		iShaderPassIndex = { 8 };
		wstring		strModelTag;
		_float4x4	WorldMatrix;

		SPECIALTYPE eSpecialType = SPECIAL_END;
		_bool		bAnimModel = { false };
		_int		iPlayAnimationIndex = { 0 };
		_bool		bPreview = true; //! 미리보기용 오브젝트인지 확인
		
		wstring		strDiffuseTextureTag = L"";
		wstring		strMaskTextureTag = L"";
		wstring		strNoiseTextureTag = L"";

		//wstring		strDiffuseTextureTag = L"Prototype_Component_Texture_RaidPoolGround_Diffuse1";
		//wstring		strMaskTextureTag = L"Prototype_Component_Texture_RaidPoolLight1";
		//wstring		strNoiseTextureTag = L"Prototype_Component_Texture_RaidPoolLight3";

		//!FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO_BOSS, TEXT("Prototype_Component_Texture_RaidPoolGround_Diffuse1"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Map/RaidPool/T_BloodGround_ORM.dds"))));
		//!FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO_BOSS, TEXT("Prototype_Component_Texture_RaidPoolLight1"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Map/RaidPool/T_RaidBloodlight_01_BC.dds"))));
		//!FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO_BOSS, TEXT("Prototype_Component_Texture_RaidPoolLight2"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Map/RaidPool/T_RaidBloodlight_02_BC.dds"))));
		//!FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_INTRO_BOSS, TEXT("Prototype_Component_Texture_RaidPoolLight3"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Map/RaidPool/T_RaidBloodlight_03_BC.dds"))));
		_int		iSpecialGroupIndex = -1;
		_int		iBloomMeshIndex = 0;
	}ENVIRONMENT_SPECIALOBJECT_DESC;



private:
	CEnvironment_SpecialObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CEnvironment_SpecialObject(const CEnvironment_SpecialObject& rhs);
	virtual ~CEnvironment_SpecialObject() = default;

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
	ENVIRONMENT_SPECIALOBJECT_DESC* Get_EnvironmentDesc() { return &m_tEnvironmentDesc; }
	wstring&						Get_ModelTag() { return m_tEnvironmentDesc.strModelTag; }
	_bool							Is_AnimModel() { return m_tEnvironmentDesc.bAnimModel; }

	CModel*							Get_ModelCom() { return m_pModelCom; }

public:
	void				Set_ColliderRender(_bool bColliderRender) { m_bColliderRender = bColliderRender; }
	void				Set_ShaderPassIndex(_int iShaderPassIndex) { m_tEnvironmentDesc.iShaderPassIndex = iShaderPassIndex;}

	_int				Get_SpecialGroupIndex() { return m_tEnvironmentDesc.iSpecialGroupIndex; }
	void				Set_SpecialGroupIndex(_int iSpecialGroupIndex) { m_tEnvironmentDesc.iSpecialGroupIndex = iSpecialGroupIndex;}

	void				Set_BloonMeshIndex(_int iMeshIndex) { m_tEnvironmentDesc.iBloomMeshIndex = iMeshIndex;}

	SPECIALTYPE			Get_SpecialType() { return m_tEnvironmentDesc.eSpecialType;}
	void				Set_SpecialType(SPECIALTYPE eSpecialType) { m_tEnvironmentDesc.eSpecialType = eSpecialType;}

#ifdef _DEBUG
public: //! For.Tool
	virtual _bool		Picking(_Out_ _float3* vPickedPos) override;
	_bool				Picking_VerJSY(RAY* pRay, _Out_ _float3* vPickedPos);

#endif 

public:
	void				Start_Environment_Animation() { m_bPlay = true; }


public:
	void				SignalInit();
	void				SignalFunction(const _float fTimeDelta);
	void				Set_SignalChange(_bool bChange);
	
public:
	HRESULT				TrackLeverInit();
	void				TrackLeverFunction();
	CUI_Weakness*		Get_LeverWeakUI() { return m_pLeverWeaknessUI; }

	

private:
	CShader*			m_pShaderCom = { nullptr };
	CModel*				m_pModelCom = { nullptr };
	CCollider*			m_pPickingCollider = nullptr;
	 
	CTexture*			m_pDiffuseTexture = { nullptr };
	CTexture*			m_pMaskTexture = { nullptr };
	CTexture*			m_pNoiseTexture = { nullptr };
	

private:
	ENVIRONMENT_SPECIALOBJECT_DESC	m_tEnvironmentDesc = {};
	_bool							m_bPlay = false;
	_bool							m_bColliderRender = false;
	_float							m_fTimeAcc = 0.f;

private: //!For. Signal
	_bool							m_bSignalChange = false;
	_float4							m_vSignalColor = {};
	_float4							m_vRedSignal = { 1.f, 0.f, 0.f, 1.f };
	_float4							m_vGreenSignal = { 0.f, 1.f, 0.f, 1.f};
	_int							m_iSignalMeshShaderPass = 9;
	_float							m_fSignalBloomPower = 5.f;
	_bool							m_bChangeLerp = false;

private: //! For. TrackLever
	CEnvironment_SpecialObject*		m_pSignalObject = { nullptr }; 
	CUI_Weakness*					m_pLeverWeaknessUI = { nullptr }; 
	CEnvironment_LightObject*		m_pLightObject = { nullptr };
	_bool							m_bLeverOn = false;
	

private:
	HRESULT				Ready_Components();
	HRESULT				Bind_ShaderResources();


public:
	/* 원형객체를 생성한다. */
	static CEnvironment_SpecialObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;

	virtual void Free() override;

};

END