#pragma once

#include "Imgui_Window.h"

BEGIN(Client)
class CEffect;
class CEffect_Void;
class CEffect_Particle;
class CEffect_Rect;
class CEffect_Instance;

class CWindow_EffectTool final : public CImgui_Window
{
public:
	struct Window_EffectTool_DESC : public ImGuiDESC
	{
	};

protected:
	CWindow_EffectTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CWindow_EffectTool() = default;

public:
	virtual	HRESULT Initialize()			override;
	virtual	void	Tick(_float fTimeDelta) override;
	virtual void	Render()				override;

public:
	HRESULT Ready_Sky();
	void	Set_SkyTexture();

/* For.Save&Load */
public:
	void	Update_SaveLoad();
	virtual	HRESULT		Save_Function(string strPath, string strFileName) override;
	virtual HRESULT		Load_Function(string strPath, string strFileName) override;


// Refactoring Start =====================================================================================================

public:
	void	Update_EffectList();

	void	Update_PlayBarArea();

	void	Update_ParticleTab();
	void	Update_RectTab();
	void	Update_MeshTab();
	void	Update_TrailTab();


	HRESULT Create_EffectObject(const wstring& strLayerTag, CGameObject* pOwner = nullptr);
	HRESULT Add_Part_Particle();
	HRESULT Add_Part_Rect();
	HRESULT Add_Part_Mesh(wstring strModelTag);
	HRESULT Add_Part_Trail();

	void	Update_CurMembers(wstring strName);

	void	Update_CurParameters();
	void	Update_CurParameters_Parts();

public:
	void	Select_EasingType(EASING_TYPE* eType);

private:
	map<const wstring, class CEffect*>		m_pEffects;
	map<const wstring, class CGameObject*>	m_CurPartObjects;

	char**			m_szEffectNames = { nullptr };
	char*			m_cCurEffectName = { nullptr };
	_int			m_iCurEffectIndex = { 0 };
	class CEffect*	m_pCurEffect = { nullptr };


	char**	m_szPartNames		= { nullptr };
	char*	m_cCurPartName		= { nullptr };
	_int	m_iCurPartIndex		= { 0 };
	CEffect_Void* m_pCurPartEffect = { nullptr };

	/* Sky */
private:
	_int m_iSkyTextureIndex = { 3 };

/* Desc */
private:
	CEffect::EFFECT_DESC* m_pCurEffectDesc = { nullptr };

	CEffect_Particle::PARTICLE_DESC*				m_pParticleDesc = {};
	CVIBuffer_Particle::PARTICLE_BUFFER_DESC*		m_pParticleBufferDesc = {};
	CEffect_Void::UVSPRITE_DESC*					m_pSpriteDesc_Particle = {};

	CEffect_Rect::EFFECT_RECT_DESC*					m_pRectDesc			= {};


	CEffect_Instance::EFFECT_INSTANCE_DESC*							m_pInstanceDesc		= {};
	CVIBuffer_Effect_Model_Instance::EFFECT_MODEL_INSTANCE_DESC*	m_pMeshBufferDesc = {};

private:
	_int m_iRenderGroup_Particle		= { 9 };
	_int m_iShaderPassIndex_Particle	= { 0 };
	_int m_iMaxShaderPassIndex_Particle = { 2 };
	_int m_iTexIndex_Particle[CEffect_Void::TEXTURE_END] = { };
	_int m_iMaxTexIndex_Particle[CEffect_Void::TEXTURE_END] = { 15, 17, 5, 16 };


	_int m_iRenderGroup_Rect = { 9 };
	_int m_iShaderPassIndex_Rect = { 0 };
	_int m_iMaxShaderPassIndex_Rect = { 2 };
	_int m_iTexIndex_Rect[CEffect_Void::TEXTURE_END] = { };
	_int m_iMaxTexIndex_Rect[CEffect_Void::TEXTURE_END] = { 4, 17, 5, 16 };


	_int m_iRenderGroup_Mesh			= { 9 };
	_int m_iShaderPassIndex_Mesh		= { 0 };
	_int m_iMaxShaderPassIndex_Mesh		= { 8 };
	_int m_iTexIndex_Mesh[CEffect_Void::TEXTURE_END] = { };
	_int m_iMaxTexIndex_Mesh[CEffect_Void::TEXTURE_END] = { 15, 17, 5, 16 };


	// Refactoring end   =====================================================================================================

private:
	_float	m_vTimeAccs_Effect[3]	= { 0.f, 0.f, 0.f };
	_float	m_vTimeAccs_Part[3]		= { 0.f, 0.f, 0.f };

private:
	_int	m_iLoop = { 0 };

	//_int	m_iNumInstance		= { 200 };
	//_int	m_iMaxNumInstance	= { 500 };

	_float	m_vTimes_Effect[3]	= { 0.f, 5.f, 0.f };	// Wait, LifeTime, Remain
	_float	m_vTimes_Part[3]	= { 0.f, 5.f, 0.f };	// Wait, LifeTime, Remain

	_float	m_vWorldPosition_Effect[3]	 = { 0.f, 0.f, 0.f };
	_float	m_vWorldPosition_Part[3]	 = { 0.f, 0.f, 0.f };

	_float	m_vScale_Effect[3] = { 0.f, 0.f, 0.f };
	_float	m_vScale_Part[3] = { 0.f, 0.f, 0.f };

	_float	m_vRotate_Effect[3] = { 0.f, 0.f, 0.f };
	_float	m_vRotate_Part[3] = { 0.f, 0.f, 0.f };

	_float m_vColor_Clip_Part[4] = { 0.f, 0.f, 0.f, 0.f };
	_float m_vColor_Clip_Rect[4] = { 0.f, 0.f, 0.f, 0.f };


#pragma region 파티클 옵션 시작
private:
	_int	m_iNumInstance_Particle		= { 200 };
	_int	m_iMaxNumInstance_Particle	= { 500 };

	_int	m_iBillBoard				= { 0 };

	_float	m_vMinMaxLifeTime_Particle[2] = { 0.f, 0.f };	// 라이프타임

	/* RigidBody ============================================== */
	_int	m_iUseRigidBody_Particle	= { 0 };
	_int	m_iKinetic_Particle			= { 0 };
	_int	m_iUseGravity_Particle		= { 0 };

	_float	m_fGravity_Particle = { -9.8f };		// 중력 가속도
	_float	m_fFriction_Particle = { 0.1f };		// 마찰 계수
	_float	m_fSleepThreshold_Particle = { 0.05f };	// 슬립 한계점(1이하로)

	_float	m_vMinMaxPower_Particle[2]	= { 0.1f, 250.f };
	_float	m_vMinMaxMass_Particle[2]	= { 10.f, 10.f };
	/* RigidBody ============================================== */


	/* For.Position */
	_float	m_vMinMaxRange_Particle[2] = { 0.f, 0.f };

	/* For.Rotation */
	_float	m_vRotationOffsetX_Particle[2] = { 0.f, 0.f };
	_float	m_vRotationOffsetY_Particle[2] = { 0.f, 0.f };
	_float	m_vRotationOffsetZ_Particle[2] = { 0.f, 0.f };


	/* For.Color */
	_int	m_iDynamic_Color_Particle = { 0 };
	_float	m_fColor_Min_Particle[4] = { 1.f, 1.f, 1.f, 1.f };
	_float	m_fColor_Max_Particle[4] = { 1.f, 1.f, 1.f, 1.f };
	_float	m_fColor_Cur_Particle[4] = { 1.f, 1.f, 1.f, 1.f };


	_float	m_fUV_RotDegree				= { 0.f };	// UV회전

	_float	m_fAddScale					= { 0.f };
	_float	m_vAddScale[2]				= { 0.f, 0.f };


	_int	m_iSortZ = { 0 };

	/* For.Sprite ======================================== */
	_int	m_iSprite_Particle				= { 0 };	// 1이 True
	_int	m_vUV_MaxTileCount_Particle[2]	= { 7, 7 };
	_float	m_fSequenceTerm_Particle		= { 0.05f };
	/* For.Sprite ======================================== */

#pragma endregion 파티클 옵션 끝



#pragma region Rect_Option
	_int	m_vUV_MaxTileCount[2] = { 7, 7 };
	_float	m_fSequenceTerm_RectSprite = { 0.05f };

	/* Distortion */
	_float m_fSequenceTerm_Distortion = { 1.f };

	_float	m_vScrollSpeeds[3]			= { 1.f, 1.f, 0.f };
	_float	m_vScales_Distortion[3]		= { 1.f, 1.f, 1.f };

	_float	m_vDistortion1[2] = { 0.1f, 0.1f };
	_float	m_vDistortion2[2] = { 0.0f, 0.0f };
	_float	m_vDistortion3[2] = { 0.0f, 0.1f };

	_float	m_fDistortionScale = { 1.f };
	_float	m_fDistortionBias = { 1.f };

#pragma endregion



#pragma region Mesh_Option
private:
	_int m_iNumInstance_Mesh = { 50 };
	_int m_iMaxNumInstance_Mesh = { 100 };

	_float  m_fUV_Offset[2] = { 0.f, 0.f };
	_float  m_vUV_Scale[2]	= { 1.f, 1.f };

	_float	m_DissolveAmount = { 0.f };

	_float	m_fUV_RotDegree_Mesh = { 0.f };

	_float	m_fRimColor_Mesh[4] = { 1.f, 1.f, 1.f, 1.f };
	_float	m_fRimPower_Mesh = { 1.f };


	_float	m_vBloomColor_Mesh[4] = { 1.f, 1.f, 1.f, 1.f };
	_float	m_vBloomPower_Mesh[3] = { 1.f, 1.f, 1.f };

	_float	m_vMinMaxPower_Mesh[2] = { 0.1f, 500.f };

	_float	m_vMinMaxRange_Mesh[2] = { 0.f, 0.f };
	_float	m_vRotationOffsetX_Mesh[2] = { 0.f, 0.f };
	_float	m_vRotationOffsetY_Mesh[2] = { 0.f, 0.f };
	_float	m_vRotationOffsetZ_Mesh[2] = { 0.f, 0.f };

#pragma endregion

public:
	HRESULT Ready_Layer_Greed(const wstring& strLayerTag);


public:
	void	Update_Demo_Sequencer();

#pragma region Sequencer
private:
	int32_t m_iCurSequenceTimePos = { 0 };
#pragma endregion


public:
	static CWindow_EffectTool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END

