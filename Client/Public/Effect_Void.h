#pragma once

#include "Client_Defines.h"

#include "AlphaObject.h"
#include "Easing_Utillity.h"

BEGIN(Client)

class CEffect_Void abstract : public CAlphaObject
{
public:
	enum TYPE_EFFECT { PARTICLE, RECT, MESH, TRAIL, TYPE_EFFECT_END };
	enum TEXTURE	 { TEXTURE_DIFFUSE, TEXTURE_MASK, TEXTURE_NOISE, TEXTURE_SPRITE, TEXTURE_END };


	typedef struct tagEffectVoidDesc : public CGameObject::GAMEOBJECT_DESC
	{
		wstring		strProtoTag = { TEXT("") };
		wstring		strPartTag = { TEXT("") };

		// Texture
		wstring		strModelTag = TEXT("");
		wstring		strTextureTag[TEXTURE_END];
		_int		iTextureIndex[TEXTURE_END] = { 0 };

		// Render Group
		_int		iRenderGroup = { 9 };	//! 밖에서 렌더러의 렌더그룹을 인트로 형변환해서 던져주자 이펙트는 9번

		// Shader
		_uint		iShaderPassIndex = { 0 };
		// Shader Variables
		_bool		bBillBoard		= { TRUE };
		_bool		bDissolve		= { TRUE };
		_float2		vUV_Offset		= { 0.f, 0.f };
		_float2		vUV_Scale		= { 0.f, 0.f };

		_float		fUV_RotDegree	= { 0.f };

		_bool		bUV_Wave		= { FALSE };
		_float		fUV_WaveSpeed	= { 1.f };

		_float4		vColor_Offset	= { 0.f, 0.f, 0.f, 0.f };
		_float4		vColor_Clip		= { 0.f, 0.f, 0.f, 0.f };
		_float4		vColor_Mul		= { 1.f, 1.f, 1.f, 1.f };

		_float		fBloom			= { 0.f };
		_float		fRadial			= { 0.f };
		_float		fDissolveAmount = { 0.f };
		_float		padding			= { 0.f };

		// States
		_bool		bActive_Tool	= { TRUE };
		_bool		bPlay			= { TRUE };
		_bool		bLoop			= { TRUE };
		_bool		bReverse		= { FALSE };
		_bool		bRender			= { FALSE };
		_bool		bUseSpriteAnim	= { FALSE };
		_bool		bUseRigidBody	= { FALSE };

		// Times
		EASING_TYPE		eType_Easing = { LINEAR };

		// 주인
		_bool		 bParentPivot	 = { TRUE };
		_float4x4	 matPivot		 = {}; /* XMStoreFloat4x4(&m_matPivot, XMMatrixIdentity()) */
		_float4x4	 matOffset		 = {};

		// Variables
		_float3	vPosition_Start = _float3(0.f, 0.f, 0.f);
		_float3	vPosition_End	= _float3(0.f, 0.f, 0.f);
		_bool	bPosition_Lerp	= { TRUE };

		_float3	vRotation_Start = _float3(0.f, 0.f, 0.f);
		_float3	vRotation_End	= _float3(0.f, 0.f, 0.f);
		_bool	bRotation_Lerp	= { TRUE };

		_float3	vScaling_Start	= _float3(1.f, 1.f, 1.f);
		_float3	vScaling_End	= _float3(1.f, 1.f, 1.f);
		_bool	bScaling_Lerp	= { TRUE };

		_float3	vVelocity_Start = _float3(0.f, 0.f, 0.f);
		_float3	vVelocity_End	= _float3(0.f, 0.f, 0.f);
		_float3	vVelocity_Cur	= _float3(0.f, 0.f, 0.f);
		_bool	bVelocity_Lerp	= { TRUE };

		_float4	vColor_Start	= _float4(1.f, 1.f, 1.f, 1.f);
		_float4	vColor_End		= _float4(1.f, 1.f, 1.f, 1.f);
		_float4	vColor_Cur		= _float4(1.f, 1.f, 1.f, 1.f);
		_bool	bColor_Lerp		= { TRUE };

		void Reset_Desc()
		{
			ZeroMemory(this, sizeof(EFFECTVOID_DESC));
			bActive_Tool	= { TRUE };
			bPlay			= { TRUE };
			bReverse		= { FALSE };
			bLoop			= { TRUE };
		}

	}EFFECTVOID_DESC;

	typedef struct tagUvSpriteDesc
	{
		_bool	bSpriteFinish   = { FALSE };
		_float	fSequenceTerm	= { 0.05f };

		_float2 vTextureSize	 = { 1792.f, 1792.f };  // fSpriteSizeX, fSpriteSizeY
		_float2 vTileSize		 = { 256.f, 256.f };	// fAnimationSizeX, fAnimationSizeY

		_float2	vUV_CurTileIndex = { 0, 0 }; // iCurrentHor, iCurrentVer
		_float2	vUV_MinTileCount = { 0, 0 }; // iMinHor, iMinVer
		_float2	vUV_MaxTileCount = { 7, 7 }; // iMaxHor, iMaxVer

	}UVSPRITE_DESC;


	typedef struct tagDistortionDesc
	{
		_float	fSequenceTerm	= { 0.05f };

		_float3 vScrollSpeeds	= { 0.f, 0.f, 0.f };
		_float3 vScales			= { 0.f, 0.f, 0.f };

		_float2 vDistortion1	= { 0.f, 0.f };
		_float2 vDistortion2	= { 0.f, 0.f };
		_float2 vDistortion3	= { 0.f, 0.f };

		_float fDistortionScale = { 0.f };
		_float fDistortionBias	= { 0.f };

	}DISTORTION_DESC;


protected:
	CEffect_Void(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CEffect_Void(const CEffect_Void& rhs);
	virtual ~CEffect_Void() = default;

public:
	virtual HRESULT Initialize_Prototype()				override;
	virtual HRESULT Initialize(void* pArg)				override;
	virtual void	Priority_Tick(_float fTimeDelta)	override;
	virtual void	Tick(_float fTimeDelta)				override;
	virtual void	Late_Tick(_float fTimeDelta)		override;
	virtual HRESULT Render()							override;


public:
	virtual _bool Write_Json(json& Out_Json)		override;
	virtual void Load_FromJson(const json& In_Json)	override;

	void	Write_VoidDesc(json& Out_Json, void* pArg);
	void*	Load_VoidDesc(const json& In_Json);

public:
	virtual void	ReSet_Effect();
	virtual void	End_Effect();


public:
	virtual void	Set_Owner(CGameObject* pOwner) { m_pOwner = pOwner; }

	TYPE_EFFECT Get_EffectType() { return m_eType_Effect; }
	void		Set_EffectType(TYPE_EFFECT eType) { m_eType_Effect = eType; }

	_float		Get_WaitingAcc() { return m_fWaitingAcc; }
	void		Set_WaitingAcc(_float fTime) { m_fWaitingAcc = fTime; }

	_float		Get_TimeAcc() { return m_fTimeAcc; }
	void		Set_TimeAcc(_float fTime) { m_fTimeAcc = fTime; }

	_float		Get_RemainAcc() { return m_fRemainAcc; }
	void		Set_RemainAcc(_float fTime) { m_fRemainAcc = fTime; }

	_float		Get_SequenceAcc() { return m_fSequenceAcc; }
	void		Set_SequenceAcc(_float fTime) { m_fSequenceAcc = fTime; }

	_float		Get_SpriteTimeAcc() { return m_fSpriteTimeAcc; }
	void		Set_SpriteTimeAcc(_float fTime) { m_fSpriteTimeAcc = fTime; }

	_float		Get_LifeTimeRatio() { return m_fLifeTimeRatio; }

	_float		Get_WaitingTime() { return m_fWaitingTime; }
	void		Set_WaitingTime(_float fTime) { m_fWaitingTime = fTime; }
	
	_float		Get_LifeTime() { return m_fLifeTime; }
	void		Set_LifeTime(_float fLifeTime) { m_fLifeTime = fLifeTime; }

	_float		Get_RemainTime() { return m_fRemainTime; }
	void		Set_RemainTime(_float fTime) { m_fRemainTime = fTime; }

	_float		Get_SequenceTime() { return m_fSequenceTime; }
	void		Set_SequenceTime(_float fSequenceTime) { m_fSequenceTime = fSequenceTime; }

protected:
	TYPE_EFFECT	m_eType_Effect = { TYPE_EFFECT_END };

	_float		m_fWaitingAcc    = { 0.f };	/* 시작 딜레이 시간 누적 */
	_float		m_fTimeAcc	    = { 0.f };	/* 시간 누적 */
	_float		m_fRemainAcc    = { 0.f };
	_float		m_fSequenceAcc  = { 0.f };	/* 시퀀스 시간 누적 */
	_float		m_fSpriteTimeAcc = { 0.f };	/* 스프라이트 시간 누적 */

	_float		m_fLifeTimeRatio = { 0.f };	/* 라이프타임을 0~1로 보간한 값 */

	_float		m_fWaitingTime   = { 0.f };	/* 이 값이 넘어가야 m_fTimeAcc가 누적되기 시작한다. */
	_float		m_fLifeTime		 = { 5.f };
	_float		m_fRemainTime	 = { 0.f };	/* 라이프타임이 지나고, 이 시간이 넘어가야 이펙트 종료. */
	_float		m_fSequenceTime	 = { 0.f };	/* 총 시퀀스 시간(fWaitingTime + fLifeTime + fRemainTime) */


	/* 주인 */
	CGameObject* m_pOwner = { nullptr };

public:
	virtual CGameObject* Clone(void* pArg)	override = 0;
	virtual CGameObject* Pool()				override = 0;

	virtual void Free() override;
};

END