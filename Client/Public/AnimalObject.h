#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)
class CUI_Weakness;

class CAnimalObject final : public CGameObject
{
public:
	enum ANIMALTYPE { ANIMAL_COW, ANIMAL_CHIKEN, ANIMAL_YOUNGCHIKEN, ANIMAL_HAWK, ANIMALTYPE_END };
	typedef struct tagAnimalObjectDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_float4		vPos = { 0.f, 0.f, 0.f, 0.f };
		_uint		iShaderPassIndex = { 0 };
		wstring		strModelTag;
		wstring		strProtoTypeTag;
		_float4x4	WorldMatrix;

		_int		iStartNaviIndex = -1;
		_int		iAnimalGroupIndex = -1;
		_int		iPlayAnimationIndex = { 0 };
		_bool		bPreview = true; //! 미리보기용 오브젝트인지 확인
		ANIMALTYPE  eAnimalType = ANIMALTYPE_END;

	}Animal_OBJECT_DESC;



private:
	CAnimalObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CAnimalObject(const CAnimalObject& rhs);
	virtual ~CAnimalObject() = default;

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
	Animal_OBJECT_DESC* Get_AnimalDesc() { return &m_tAnimalDesc; }
	wstring&			Get_ModelTag() { return m_tAnimalDesc.strModelTag; }
	_bool				Is_AnimModel() { return m_tAnimalDesc.bAnimModel; }
	
	CModel*				Get_ModelCom() { return m_pModelCom; }
	
public:
	_int				Get_AnimationIndex() { return m_tAnimalDesc.iPlayAnimationIndex; }
	void				Set_AnimationIndex(_uint iAnimIndex);
	void				Set_ColliderRender(_bool bColliderRender) { m_bColliderRender = bColliderRender;}
	void				Set_ShaderPassIndex(_int iShaderPassIndex) { m_tAnimalDesc.iShaderPassIndex = iShaderPassIndex;}
	void				Set_AnimalGroupIndex(_int iAnimalGroupIndex) { m_tAnimalDesc.iAnimalGroupIndex = iAnimalGroupIndex;}
	_int				Get_StartNaviIndex() { return m_tAnimalDesc.iStartNaviIndex;}
	void				Set_StartNaviIndex(_int iStartNaviIndex) { m_tAnimalDesc.iStartNaviIndex;}

	

#ifdef _DEBUG
public: //! For.Tool
	virtual _bool		Picking(_Out_ _float3* vPickedPos) override;
	_bool				Picking_VerJSY(RAY* pRay, _Out_ _float3* vPickedPos);

#endif 


private:
	CShader*					m_pShaderCom = { nullptr };	
	CModel*						m_pModelCom = { nullptr };
	CCollider*					m_pPickingCollider = nullptr;
	CUI_Weakness*				m_pWeaknessUI = { nullptr };

private:
	Animal_OBJECT_DESC			m_tAnimalDesc = {};
	_bool						m_bColliderRender = false;

	_uint						m_iCurrentLevel = LEVEL_TOOL;

private:
	HRESULT				Ready_Components();
	HRESULT				Bind_ShaderResources();

	/* 동물 전용 */
	class CEffect*		m_pEffect = { nullptr };
	_float4				m_vCamPosition = {};
	_float				m_fCamFar = {};

public:
	/* 원형객체를 생성한다. */
	static CAnimalObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;

	virtual void Free() override;

};

END