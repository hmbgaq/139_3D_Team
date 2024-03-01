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
	enum TYPE_MESH	{ MESH_PARTICLE, MESH_STATIC, TYPE_MESH_END };


public:
	typedef struct tagEffect_Desc : public CEffect_Void::EFFECTVOID_DESC
	{
		TYPE_MESH		eType_Mesh = { MESH_PARTICLE };
		_bool			bUseCustomTex = { TRUE };

		_uint			iCurNumInstance = { 1 };

		_float4			vBloomColor = { 1.f, 1.f, 1.f, 1.f };
		_float3			vBloomPower = { 1.f, 1.f, 1.f };

		_float4			vRimColor = { 1.f, 1.f, 1.f, 1.f };
		_float			fRimPower = { 1.f };

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
	EFFECT_INSTANCE_DESC* Get_Desc() { return &m_tInstanceDesc; }
	void* Get_BufferDesc();

	CVIBuffer_Effect_Model_Instance* Get_VIBufferCom() { return m_pVIBufferCom; }

private:
	CShader*							m_pShaderCom				= { nullptr };	
	CModel*								m_pModelCom					= { nullptr };
	CTexture*							m_pTextureCom[TEXTURE_END]	= { nullptr };
	CVIBuffer_Effect_Model_Instance*	m_pVIBufferCom				= { nullptr };

	EFFECT_INSTANCE_DESC				m_tInstanceDesc = {};

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