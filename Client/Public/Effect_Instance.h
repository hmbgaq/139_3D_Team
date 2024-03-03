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
	typedef struct tagEffectInstanceDesc : public CEffect_Void::EFFECTVOID_DESC
	{
		TYPE_MESH		eType_Mesh = { MESH_PARTICLE };
		_bool			bUseCustomTex = { TRUE };


#pragma region 버퍼 Desc초기화를 위한 값 (저장X : 버퍼가 저장)
		/* RigidBody */
		_bool		bKinetic = { TRUE };		// 키네틱, 즉 TRUE면 속도 계산 함)
		_bool		bUseGravity = { TRUE };
		FORCE_MODE	eForce_Mode = { FORCE_MODE::IMPULSE };

		_float		fGravity = { -9.8f };			// 중력 가속도
		_float		fFriction = { 0.1f };			// 마찰 계수
		_float		fSleepThreshold = { 0.05f };	// 슬립 한계점
		_byte		byFreezeAxis = { 0 };			// 축 고정 확인용 바이트

		_float2		vMinMaxPower = { 0.1f, 250.f };


		/* For.Position */
		_float4		vCenterPosition		= { 0.f, 0.f, 0.f, 1.f };
		_float2		vMinMaxRange		= { 0.1f, 3.f };

		/* For.Rotation */
		_float2		vMinMaxRotationOffsetX	= { 0.0f, 360.f };
		_float2		vMinMaxRotationOffsetY	= { 0.0f, 360.f };
		_float2		vMinMaxRotationOffsetZ	= { 0.0f, 360.f };
		_float3		vRotationOffset			= { 0.f, 0.f, 0.f };
#pragma endregion


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