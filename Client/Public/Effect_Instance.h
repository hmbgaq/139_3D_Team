#pragma once

#include "Client_Defines.h"
#include "Effect_Void.h"

BEGIN(Engine)
class CShader;
class CCollider;
class CModel;
class CVIBuffer_Effect_Model_Instance;
END

BEGIN(Client)

class CEffect_Instance final : public CEffect_Void
{
public:
	typedef struct tagEffectInstanceDesc
	{
		_bool		bUseCustomTex	 = { TRUE };


		/* Bloom */
		_float4		vBloomColor = { 1.f, 1.f, 1.f, 1.f };
		_float3		vBloomPower = { 1.f, 1.f, 1.f };

		/* Rim */
		_float4		vRimColor = { 1.f, 1.f, 1.f, 1.f };
		_float		fRimPower = { 1.f };


	}EFFECT_INSTANCE_DESC;


private:
	CEffect_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CEffect_Instance(const CEffect_Instance& rhs);
	virtual ~CEffect_Instance() = default;

public:
	virtual HRESULT				Initialize_Prototype()				override;
	virtual HRESULT				Initialize(void* pArg)				override;
	virtual void				Priority_Tick(_float fTimeDelta)	override;
	virtual void				Tick(_float fTimeDelta)				override;
	virtual void				Late_Tick(_float fTimeDelta)		override;
	virtual HRESULT				Render()							override;

public:
	virtual _bool Write_Json(json& Out_Json)		 override;
	virtual void  Load_FromJson(const json& In_Json) override;

public:
	virtual void	ReSet_Effect()	override;
	virtual void	End_Effect()	override;


/* For.Desc */
public:
	EFFECT_INSTANCE_DESC*	Get_InstanceDesc() { return &m_tInstanceDesc; }
	DISTORTION_DESC*		Get_Distortion_Desc() { return &m_tDistortionDesc; }

	CVIBuffer_Effect_Model_Instance* Get_VIBufferCom() { return m_pVIBufferCom; }

private:
	CShader*							m_pShaderCom				= { nullptr };	
	CModel*								m_pModelCom[CVIBuffer_Effect_Model_Instance::MORPH_END]	= { nullptr };
	CTexture*							m_pTextureCom[TEXTURE_END]	= { nullptr };
	CVIBuffer_Effect_Model_Instance*	m_pVIBufferCom				= { nullptr };

	EFFECT_INSTANCE_DESC				m_tInstanceDesc = {};
	DISTORTION_DESC						m_tDistortionDesc = {};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();


public:
	/* 원형객체를 생성한다. */
	static CEffect_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool();

	virtual void Free() override;

};

END