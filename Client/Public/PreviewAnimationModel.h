#pragma once

#include "Client_Defines.h"
#include "Character.h"

BEGIN(Engine)
class CModel;
class CBone;
END

BEGIN(Client)

class Cweapon;

class CPreviewAnimationModel : public CCharacter
{
protected:
	CPreviewAnimationModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPreviewAnimationModel(const CPreviewAnimationModel& rhs);
	virtual ~CPreviewAnimationModel() = default;

public:
	CModel*		Get_CurrentModel();

protected:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)	override;
	virtual void Priority_Tick(_float fTimeDelta)	override;
	virtual void Tick(_float fTimeDelta)	override;
	virtual void Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT Render()	override;
	virtual HRESULT Render_Shadow();

public:
	void Init_EditPreviewAnimationModel(const string& In_szModelKey);
	void Change_AnimationFromIndex(const _uint& In_iAnimIndex);
	void Play_Animation(_float fTimeDelta, _bool bIsLoop);
	void Add_DebugWeapon(const string& In_szBoneName, const _float3& In_vOffset, const _float In_fSize);
	void Remove_DebugWeapon(const string& In_szBoneName);
	void Clear_DebugWeapon();
	void Clear_ModelWeapon();

	void Bind_KeyFrameEvent(const string& In_szKeyFrameEventFileName);
private:
	void Release_BeforeModel();

private:
	CBone*				m_pParentBone;
	_float4x4			m_TransformationMatrix;

public:
	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END