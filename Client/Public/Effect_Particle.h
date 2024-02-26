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
	enum PROJTYPE { TYPE_PERSPECTIVE, TYPE_ORTHOGRAPHIC, TYPE_END };
	enum TYPE { SINGLE, SPRITE, TEXTURE_TYPE_END };
	//typedef struct tagParticleDesc : public CEffect_Void::EFFECTVOID_DESC
	//{
	//	TYPE	eType = { SINGLE };


	//}PARTICLE_DESC;

	typedef struct tagParticleDesc : public CEffect_Void::EFFECTVOID_DESC
	{
		// 파티클 타입
		PROJTYPE eType_Proj = { TYPE_PERSPECTIVE };
		_bool bSortZ		= { false };

		// 분포 범위
		_float3 fRange = _float3(0.f, 0.f, 0.f);
		_float2 fRangeDistance = _float2(0.f, 0.f);

#pragma region 크기
		_bool	bScaleSameRate = { false }; // 정비율
		_float2 fScaleStart = _float2(2.f, 4.f);

		_bool	bScaleChange = { false };
		_float2 fScaleChangeStartDelay = _float2(0.f, 0.f);

		_bool	bScaleChangeRandom = { false };
		_float2 fScaleChangeTime = _float2(1.f, 5.f);

		_bool bScaleAdd = { false };
		_bool bScaleLoop = { false };
		_bool bScaleLoopStart = { false };

		_float2 fScaleMin = _float2(1.f, 1.f);
		_float2 fScaleMax = _float2(10.f, 10.f);
		_float2 fScaleSpeed = _float2(1.f, 5.f);
#pragma endregion

#pragma region 이동
		_float2 fVelocitySpeed = _float2(1.f, 1.f);

		_float3 vVelocityMinStart = _float3(0.f, 0.f, 0.f);
		_float3 vVelocityMaxStart = _float3(0.f, 0.f, 0.f);

		_bool bVelocityChange = { false };
		_float2 fVelocityChangeStartDelay = _float2(0.f, 0.f);

		_bool bVelocityChangeRandom = { false };
		_float2 fVelocityChangeTime = _float2(1.f, 5.f);

		_bool bVelocityLoop = { false };
		_uint iVelocityCountCur = { 0 };
		_uint iVelocityCountMax = { 2 };

		_uint iVelocityUse = { 10 }; // 사용하는 Velocity 포인터 변수 개수
		_float3* pVelocityMin = { nullptr };
		_float3* pVelocityMax = { nullptr };
		_float2* pVelocityTime = { nullptr };
#pragma endregion

#pragma region 회전
		_bool bRandomAxis = { false };
		_float4 vAxis = _float4(0.f, 1.f, 0.f, 0.f);

		_bool bRandomAngle = { false };
		_float fAngle = 0.f;

		_bool bRotationChange = { false };
		_float2 fRotationChangeStartDelay = _float2(0.f, 0.f);

		_float2 fRotationSpeed = _float2(30.f, 30.f);

		_bool bRotationChangeRandom = { false };
		_float2 fRotationChangeTime = _float2(1.f, 1.f);

		_bool bRotationAdd = { false };
#pragma endregion

		// 지속 시간
		_float2	vLifeTime = _float2(5.f, 10.f);

		_bool   bUseBox = { false };
		_float3 fBoxMin;
		_float3 fBoxMax;

#pragma region 텍스처
		_bool	bRandomStartIndex = { true };

		_float2 fUVIndex = _float2(0.f, 0.f);
		_float2 fUVMaxCount = _float2(1.f, 1.f);

#pragma endregion

#pragma region 애니메이션
		_bool bAnimation = { false };
		_bool bAnimationLoop = { false };
		_float2 fAnimationSpeed = _float2(1.f, 1.f);
#pragma endregion

#pragma region 알파
		_float2 fStartAlpha = _float2(0.f, 0.f);

		_bool bFadeCreate = { false };
		_bool bFadeDelete = { false };
		_float2 fFadeSpeed = _float2(1.f, 1.f);

		_bool   bFadeChange = { false };
		_bool   bFadeIn = { false }; // 점점 뚜렷해지다.
		_float2 fFadeChangeStartDelay = _float2(0.f, 0.f);
#pragma endregion

#pragma region 색상
		_bool  bColorRandom = { false };

		_bool bColorChange = { false };

		_bool	bColorChangeRandom = { false };
		_float2 fColorChangeRandomTime = _float2(4.f, 4.f);

		_bool	bColorLoop = { false };
		_float2 fColorChangeStartDelay = _float2(2.f, 2.f);

		_float2 fColorChangeStartM = _float2(4.f, 4.f);
		_float4 fColorMid = _float4(0.f, 1.f, 0.f, 0.f);

		_float2 vColorChangeStartE = _float2(6.f, 6.f);

		// 보간 시간
		_float2 fColorDuration = _float2(2.f, 2.f);
#pragma endregion

#pragma region 블러
		_bool   bBloomPowerRandom = { false };
		_float4 fBloomPower = _float4(1.f, 0.f, 0.f, 1.f);

		_bool   bBlurPowerRandom = { false };
		_float  fBlurPower = 0.5f;
#pragma endregion

	}PARTICLE_DESC;

	typedef struct tagParticleRigidbodyDesc
	{
		_bool bRigidbody	= { false };
		_bool bGravity		= { false };
		_bool bStopZero		= { false };
		_bool bStopStartY	= { false };
		_bool bGroundSlide	= { false };

		_bool bStartJump	= { false };
		_float4 vStartMinVelocity = _float4(0.f, 0.f, 0.f, 0.f);
		_float4 vStartMaxVelocity = _float4(0.f, 0.f, 0.f, 0.f);

		_float4 vMaxVelocity = _float4(10.f, 30.f, 10.f, 0.f); // 최대 속력 = 이동 속도
		_float fMass	  = { 1.f }; // 질량
		_float fFricCoeff = { 5.f }; // 마찰 계수

	}PARTICLE_RIGIDBODY_DESC;

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
	PARTICLE_DESC* Get_Desc() { return &m_tParticleDesc; }

	void Set_ParticleDesc(const PARTICLE_DESC& tDesc);
	void Set_RigidbodyDesc(const PARTICLE_RIGIDBODY_DESC& tDesc);

	void Add_Velocity(_float4 _vMinVelocity, _float4 _vMaxVelocity);

	void Set_Texture_Diffuse();

public:
	CVIBuffer_Particle_Point*	Get_VIBufferCom_Point() { return m_pVIBufferCom_Point; }
	CVIBuffer_Particle*			Get_VIBufferCom() { return m_pVIBufferCom; }
	void* Get_BufferDesc();

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	PARTICLE_DESC				m_tParticleDesc = {};
	PARTICLE_RIGIDBODY_DESC		m_tRigidbodyDesc;
	//CVIBuffer_Particle_Point::PARTICLE_POINT_DESC 
	//UVSPRITE_DESC				m_tSpriteDesc = {};

	_bool m_bParticleDelete = { true };
	_bool m_bParticleDie	= { false };

	_float4x4 m_ViewMatrix;
	_float4x4 m_ProjMatrix;


	_float3 m_vLocalPos			= _float3(0.f, 0.f, 0.f);
	_float3 m_vLocalScale		= _float3(0.f, 0.f, 0.f);
	_float3 m_vLocalRotation	= _float3(0.f, 0.f, 0.f);
	_float4 m_vAddOffsetPos		= _float4(0.f, 0.f, 0.f, 1.f);

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom[TEXTURE_END] = { nullptr };
	
	CVIBuffer_Particle_Point*	m_pVIBufferCom_Point = { nullptr };
	CVIBuffer_Particle*			m_pVIBufferCom = { nullptr };

	CModel* m_pModelCom = { nullptr };
	CVIBuffer_Effect_Model_Instance* m_pVIBufferCom_Model = { nullptr };


public:
	/* 원형객체를 생성한다. */
	static CEffect_Particle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;

	virtual void Free() override;
};

END