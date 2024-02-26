#pragma once

#include "Client_Defines.h"
#include "Effect_Void.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Particle_Point;
END

BEGIN(Client)

class CEffect_Particle final : public CEffect_Void
{
public:
	typedef struct tagParticleDesc : public CEffect_Void::EFFECTVOID_DESC
	{
		CVIBuffer_Particle_Point::TYPE_ACTION	eType_Action = { CVIBuffer_Particle_Point::SPHERE };
		CVIBuffer_Particle_Point::TYPE_FADE		eType_Fade = { CVIBuffer_Particle_Point::FADE_NONE };
		EASING_TYPE								eType_ColorLerp = { LINEAR };


		_bool		bLoop = { TRUE };
		_bool		bReverse = { FALSE };
		_bool		bSpriteAnim = { FALSE };


		/* 파티클 개수 */
		_uint		iCurNumInstance = { 100 };

		/* LifeTime */
		_float2		vMinMaxLifeTime = { 0.1f, 3.f };

		/* For.Position */
		_float2		vMinMaxRange = { 0.1f, 3.f };
		_float2		vMinMaxRangeLength = { 0.1f, 5.f };

		_float4		vCenterPosition = { 0.f, 0.f, 0.f, 1.f};

		/* For.Speed */
		_float2		vMinMaxSpeed = { 0.1f, 1.f };

		/* 가속도 */
		_float		fSpeedAcc = { 1.f };
		_float		fAccPosition = { 0.1f };

		/* For.Gravity */
		_bool		bUseGravity = { FALSE };
		_float		fGravityAcc = { -5.f };
		_float		fUseGravityPosition = { 0.1f };

		/* For.Rotation */
		_float2		vMinMaxRotationOffsetX = { 0.0f, 360.f };
		_float2		vMinMaxRotationOffsetY = { 0.0f, 360.f };
		_float2		vMinMaxRotationOffsetZ = { 0.0f, 360.f };
		_float3		vRotationOffset = {0.f, 0.f, 0.f};

		_float3     vCurrentRotation;
		_float3		vMinMaxRotationForce;

		/* For.Scale */
		_float2     vMinMaxScale;
		_float2     vAddScale;
		_float2		vCurrentScale;

		/* For.Color */
		_float2     vMinMaxRed = { 0.f, 1.f };
		_float2     vMinMaxGreen = { 0.f, 1.f };
		_float2     vMinMaxBlue = { 0.f, 1.f };
		_float2     vMinMaxAlpha = { 0.f, 1.f };

		_float4     vCurrentColor;

	}PARTICLE_DESC;


private:
	CEffect_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CEffect_Particle(const CEffect_Particle& rhs);
	virtual ~CEffect_Particle() = default;

public:
	virtual HRESULT Initialize_Prototype()				override;
	virtual HRESULT Initialize(void* pArg)				override;
	virtual void	Priority_Tick(_float fTimeDelta)	override;
	virtual void	Tick(_float fTimeDelta)				override;
	virtual void	Late_Tick(_float fTimeDelta)		override;
	virtual HRESULT Render()							override;

public:
	virtual void	ReSet_Effect()	override;
	virtual void	End_Effect()	override;

public:
	virtual _bool Write_Json(json& Out_Json)		 override;
	virtual void  Load_FromJson(const json& In_Json) override;


	/* For.Desc */
public:
	PARTICLE_DESC*	Get_Desc() { return &m_tParticleDesc; }
	void* Get_BufferDesc();

public:
	CVIBuffer_Particle_Point* Get_VIBufferCom() { return m_pVIBufferCom; }


private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	PARTICLE_DESC				m_tParticleDesc = {};
	UVSPRITE_DESC				m_tSpriteDesc = {};
	//CVIBuffer_Particle_Point::PARTICLE_POINT_DESC 

	//CModel*							 m_pModelCom = { nullptr };
	//CVIBuffer_Effect_Model_Instance* m_pVIBufferCom_Model = { nullptr };


private:
	CShader*					m_pShaderCom = { nullptr };
	CTexture*					m_pTextureCom[TEXTURE_END] = { nullptr };
	CVIBuffer_Particle_Point*	m_pVIBufferCom = { nullptr };


public:
	/* 원형객체를 생성한다. */
	static CEffect_Particle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;

	virtual void Free() override;
};

END