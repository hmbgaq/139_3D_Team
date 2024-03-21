#pragma once

#include "Imgui_Window.h"

BEGIN(Client)
class CEffect;
class CEffect_Void;
class CEffect_Particle;
class CEffect_Rect;
class CEffect_Instance;
class CEffect_Trail;

class CSky;

class CWindow_EffectTool final : public CImgui_Window
{
public:
	enum TYPE_FILE { FILE_EFFECT, FILE_TRAIL, TYPE_FILE_END };

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


/* For.Save&Load */
public:
	void	Update_SaveLoad_Menu();	// 저장 불러오기 메뉴 업데이트
	virtual	HRESULT		Save_Function(string strPath, string strFileName) override;
	virtual HRESULT		Load_Function(string strPath, string strFileName) override;


/* For.Level Setting (환경 세팅) */
public:
	void	Show_ImGui_WindowSize();	// ImGui 윈도우 창 크기 표시
	void	Show_MousePos();			// 마우스 위치 표시

	void	Show_CameraInfo();	// 카메라 정보 표시
	void	ReSet_CameraPos();	// 카메라 위치 리셋

	HRESULT Ready_Grid();		// 그리드 생성

	HRESULT Ready_Sky();		// 스카이박스 준비
	void	Set_SkyTexture();	// 스카이박스 텍스처 변경

	HRESULT Ready_Model_Preview(wstring strModelTag);	// 크기비교용 모델 생성


/* For.Window Update (창 업데이트) */
public:
	void	Update_LevelSetting_Window();	// 레벨(환경) 세팅 창(카메라, 스카이박스, 크기비교용 모델 등...)

	void	Update_EffectList_Window();		// 이펙트 리스트박스 창

	void	Update_Timeline_Window();		// 타임라인 창
	void	Update_NeoSequencer_Window();	// 시퀀서 창

	void	Update_ImageList_Window();		// 텍스처 이미지 미리보기, 리스트

	void	Update_ParticleTab();			// 파티클 탭
	void	Update_RectTab();				// 렉트 탭
	void	Update_MeshTab();				// 메쉬 탭
	void	Update_TrailTab();				// 트레일 탭


/* For.Create & Add (이펙트 생성 관련) */
	HRESULT Create_EffectObject(const wstring& strLayerTag, CGameObject* pOwner = nullptr);		// 파트 이펙트를 담을 이펙트 오브젝트 생성 함수
	HRESULT Add_Part_Particle();						// 파트:2D파티클 추가
	HRESULT Add_Part_Rect();							// 파트:Rect 추가
	HRESULT Add_Part_Mesh(wstring strModelTag);			// 파트:메시 이펙트 추가
	HRESULT Add_Part_Mesh_Morph(wstring strModelTag1, wstring strModelTag2);	// 파트:메시(모프) 이펙트 추가
	
	HRESULT Create_Trail();								// 트레일 생성
	HRESULT Delete_Trail();								// 트레일 삭제

	void	Delete_CurEffectObject();					// 현재 선택된 이펙트 삭제
	void	Delete_CurPart();							// 현재 선택된 파트 이펙트 삭제		

	void	Copy_CurPart();								// 현재 선택된 파트 이펙트 복사

/* For.CurObject (현재 선택된 이펙트 관련 정보 업데이트) */
	void	Update_CurMembers(wstring strName);		// 현재 이펙트의 정보 업데이트
	void	Update_CurParameters();

	void	Update_CurParameters_Parts();			// 현재 파트 이펙트의 정보 업데이트

public:
	void	Select_EasingType(EASING_TYPE* eType);	// 이징 타입(러프관련) 선택

private:
	TYPE_FILE	m_eFile = { TYPE_FILE_END };


private:
	map<const wstring, class CEffect*>		m_pEffects;
	map<const wstring, class CGameObject*>	m_CurPartObjects;

	char**			m_szEffectNames = { nullptr };
	char*			m_cCurEffectName = { nullptr };
	_int			m_iCurEffectIndex = { 0 };
	class CEffect*	m_pCurEffect = { nullptr };


	char**		  m_szPartNames			= { nullptr };
	char*		  m_cCurPartName		= { nullptr };
	_int		  m_iCurPartIndex		= { 0 };
	CEffect_Void* m_pCurPartEffect	    = { nullptr };



/* Desc */
private:
	CEffect::EFFECT_DESC*							m_pCurEffectDesc = { nullptr };	// Effect Desc (공용)

	CEffect_Void::EFFECTVOID_DESC*					m_pCurVoidDesc = {}; // Effect_Void Desc (공용)

	/* Particle Desc */
	CVIBuffer_Particle::PARTICLE_BUFFER_DESC*		m_pParticleBufferDesc	= {};	// 파티클 버퍼 Desc
	CEffect_Void::UVSPRITE_DESC*					m_pSpriteDesc_Particle	= {};	// 파티클이 사용할 Void의 스프라이트 Desc

	/* Rect Desc */
	CEffect_Rect::EFFECT_RECT_DESC*					m_pRectDesc			= {};		// Rect만의 Desc


	/* Instance(Mesh) Desc */
	CEffect_Instance::EFFECT_INSTANCE_DESC*							m_pInstanceDesc		= {};	// Instance(Mesh)만의 Desc
	CVIBuffer_Effect_Model_Instance::EFFECT_MODEL_INSTANCE_DESC*	m_pMeshBufferDesc	= {};	// Instance(Mesh) 버퍼 Desc


/* Trail */
private:
	CEffect_Trail* m_pTrail = { nullptr };

	/* Trail Desc */
	CEffect_Void::EFFECTVOID_DESC*		m_pVoidTrailDesc = {};		// Effect_Void Desc (트레일 전용)
	CEffect_Trail::TRAIL_DESC*			m_pTrailDesc = {};			// Trail만의 Desc
	CVIBuffer_Trail::TRAIL_BUFFER_DESC* m_pTrailBufferDesc = {};	// Trail 버퍼 Desc


private:
	_int m_iRenderGroup_Particle							= { ECast(CRenderer::RENDER_EFFECT) };
	_int m_iShaderPassIndex_Particle						= { 0 };
	_int m_iMaxShaderPassIndex_Particle						= { 4 };
	_int m_iTexIndex_Particle[CEffect_Void::TEXTURE_END]	= {};
	_int m_iMaxTexIndex_Particle[CEffect_Void::TEXTURE_END] = { 12, 0, 44, 24, 20 };


	_int m_iRenderGroup_Rect								= { ECast(CRenderer::RENDER_EFFECT) };
	_int m_iShaderPassIndex_Rect							= { 0 };
	_int m_iMaxShaderPassIndex_Rect							= { 5 };
	_int m_iTexIndex_Rect[CEffect_Void::TEXTURE_END]		= {};
	_int m_iMaxTexIndex_Rect[CEffect_Void::TEXTURE_END]		= { 12, 0, 44, 24, 20 };


	_int m_iRenderGroup_Mesh								= { ECast(CRenderer::RENDER_EFFECT) };
	_int m_iShaderPassIndex_Mesh							= { 0 };
	_int m_iMaxShaderPassIndex_Mesh							= { 6 };
	_int m_iTexIndex_Mesh[CEffect_Void::TEXTURE_END]		= {};
	_int m_iMaxTexIndex_Mesh[CEffect_Void::TEXTURE_END]		= { 12, 0, 44, 24, 20 };


	_int m_iRenderGroup_Trail								= { ECast(CRenderer::RENDER_EFFECT) };
	_int m_iShaderPassIndex_Trail							= { 0 };
	_int m_iMaxShaderPassIndex_Trail						= { 5 };
	_int m_iTexIndex_Trail[CEffect_Void::TEXTURE_END]		= {};
	_int m_iMaxTexIndex_Trail[CEffect_Void::TEXTURE_END]	= { 12, 0, 44, 24, 20 };


private:
	_float	m_vTimeAccs_Effect[3]	= { 0.f, 0.f, 0.f };
	_float	m_vTimeAccs_Part[3]		= { 0.f, 0.f, 0.f };

private:
	_int	m_iLoop = { 0 };
	_int	m_iType_Dead = { 0 };

	_float	m_vTimes_Effect[3]	= { 0.f, 5.f, 0.f };	// Wait, LifeTime, Remain
	_float	m_vTimes_Part[3]	= { 0.f, 5.f, 0.f };	// Wait, LifeTime, Remain

	_float	m_vWorldPosition_Effect[3]	 = { 0.f, 0.f, 0.f };
	_float	m_vWorldPosition_Part[3]	 = { 0.f, 0.f, 0.f };

	_float	m_vScale_Effect[3] = { 0.f, 0.f, 0.f };
	_float	m_vScale_Part[3] = { 0.f, 0.f, 0.f };

	_float	m_vRotate_Effect[3] = { 0.f, 0.f, 0.f };
	_float	m_vRotate_Part[3] = { 0.f, 0.f, 0.f };

	_float m_vColor_Clip_Part[4] = { 0.f, 0.f, 0.f, 0.f };


#pragma region Particle 옵션 시작
private:
	_int	m_iNumInstance_Particle		= { 200 };
	_int	m_iMaxNumInstance_Particle	= { 500 };

	_int	m_iBillBoard				= { 0 };

	/* 파티클만의 속성 */
	_int	m_iRecycle_Particle			= { 0 };
	_int	m_iReverse_Particle			= { 0 };
	_int	m_iType_Emit_Particle		= { 0 };
	_int	m_iType_Action_Particle		= { 0 };
	_int	m_iType_Fade_Particle		= { 0 };


	_float	m_vMinMaxLifeTime_Particle[2] = { 0.f, 0.f };	// 라이프타임

	/* RigidBody ============================================== */
	_int	m_iUseRigidBody_Particle	= { 0 };
	_int	m_iKinetic_Particle			= { 0 };
	_int	m_iUseGravity_Particle		= { 0 };

	_float	m_fGravity_Particle = { -9.8f };		// 중력 가속도
	
	//_float	m_fFriction_Particle = { 0.1f };		// 마찰 계수
	_float		m_vFrictionLerp_Pos_Particle[2] = { 0.f, 0.f };		// 어디서부터 러프를 시작하고, 끝낼건지
	_float		m_vStartEnd_Friction_Particle[2] = { 0.1f, 0.1f };


	_float	m_fSleepThreshold_Particle = { 0.05f };	// 슬립 한계점(1이하로)

	_float	m_vMinMaxPower_Particle[2]	= { 0.1f, 250.f };
	_float	m_vMinMaxMass_Particle[2]	= { 10.f, 10.f };
	_float	m_vMinMaxSpeed_Particle[2] = { 1.f, 1.f };
	/* RigidBody ============================================== */


	/* For.Position */
	_float	m_vMinCenterOffsetPos_Particle[3] = { 0.f, 0.f, 0.f };
	_float	m_vMaxCenterOffsetPos_Particle[3] = { 0.f, 0.f, 0.f };
	_float	m_vMinMaxRange_Particle[2] = { 0.f, 0.f };

	/* For.Rotation */
	_float	m_vRadian_Particle[3] = { 0.f, 0.f, 0.f };
	_float	m_vRotationOffsetX_Particle[2] = { 0.f, 0.f };
	_float	m_vRotationOffsetY_Particle[2] = { 0.f, 0.f };
	_float	m_vRotationOffsetZ_Particle[2] = { 0.f, 0.f };


	/* For.Scale */
	//_float	m_vLerpScale_Pos_Particle[2]	= { 0.f, 1.f };

	_int	m_iUseScaleLerp					= { 0 };
	_float	m_vScaleLerp_Up_Pos[2]			= { 0.f, 0.3f };
	_float	m_vScaleLerp_Down_Pos[2]		= { 1.f, 1.f };
	_float	m_vMinMaxWidth_Particle[2]		= { 0.f, 1.f };
	_float	m_vMinMaxHeight_Particle[2]		= { 0.f, 1.f };
	//_float m_vScaleSpeed_Particle[2]		= { 0.005f, 0.05f };

	/* For.Color */
	_int	m_iDynamic_Color_Particle = { 0 };
	_float	m_fColor_Min_Particle[4] = { 1.f, 1.f, 1.f, 1.f };
	_float	m_fColor_Max_Particle[4] = { 1.f, 1.f, 1.f, 1.f };
	_float	m_fColor_Cur_Particle[4] = { 1.f, 1.f, 1.f, 1.f };

	_int	m_iColor_Mode_Particle = { 0 };
	_float	m_fColor_Mul_Particle[4] = { 1.f, 1.f, 1.f, 1.f };


	/* UV Option_Particle */
	_float  m_vUV_Offset_Particle[2]	= { 0.f, 0.f };
	_float  m_vUV_Scale_Particle[2]		= { 1.f, 1.f };
	_float	m_fUV_RotDegree_Particle	= { 0.f };			// UV회전

	_float	m_fAddScale					= { 0.f };
	_float	m_vAddScale[2]				= { 0.f, 0.f };


	_int	m_iSortZ = { 0 };

	/* For.Sprite ======================================== */
	_int	m_iSprite_Particle				= { 0 };	// 1이 True
	_int	m_iSpriteLoop					= { 0 };
	_int	m_vUV_MaxTileCount_Particle[2]	= { 7, 7 };
	_float	m_fSequenceTerm_Particle		= { 0.05f };
	/* For.Sprite ======================================== */



	/* Distortion ============================================== */
	_float m_fSequenceTerm_Distortion_Particle = { 1.f };
	_int	m_iType_Scroll_Particle = { 0 };

	_float	m_vScrollSpeeds_Particle[3] = { 1.f, 1.f, 0.f };
	_float	m_vScales_Distortion_Particle[3] = { 1.f, 1.f, 1.f };

	_float	m_vDistortion1_Particle[2] = { 0.1f, 0.1f };
	_float	m_vDistortion2_Particle[2] = { 0.0f, 0.0f };
	_float	m_vDistortion3_Particle[2] = { 0.0f, 0.1f };

	_float	m_fDistortionScale_Particle = { 1.f };
	_float	m_fDistortionBias_Particle = { 1.f };
	/* Distortion ============================================== */



	/* 파티클 Rim & Bloom */
	_float	m_fRimColor_Particle[4] = { 1.f, 1.f, 1.f, 1.f };
	_float	m_vBloomPower_Particle[3] = { 1.f, 1.f, 1.f };

#pragma endregion Particle 옵션 끝



#pragma region Rect 옵션 시작

	/* For.Sprite ======================================== */
	_int	m_iSprite_Rect = { 0 };					// 1이 True
	_int	m_iSpriteLoop_Rect = { 0 };
	_int	m_vUV_MaxTileCount_Rect[2] = { 7, 7 };
	_float	m_fSequenceTerm_Rect = { 0.05f };
	/* For.Sprite ======================================== */


	_int	m_iBillBoard_Rect = { 0 };

	_float	m_fColor_Mul_Rect[4] = { 1.f, 1.f, 1.f, 1.f };	// 쉐이더에서 곱해줄 색
	_float	m_vColor_Clip_Rect[4] = { 0.f, 0.f, 0.f, 0.f };	// 쉐이더에서 discard할 값


	/* Distortion */
	_int	m_iType_Scroll_Rect = { 0 };

	_float	m_vScrollSpeeds[3]			= { 1.f, 1.f, 0.f };
	_float	m_vScales_Distortion[3]		= { 1.f, 1.f, 1.f };

	_float	m_vDistortion1[2] = { 0.1f, 0.1f };
	_float	m_vDistortion2[2] = { 0.0f, 0.0f };
	_float	m_vDistortion3[2] = { 0.0f, 0.1f };

	_float	m_fDistortionScale = { 1.f };
	_float	m_fDistortionBias = { 1.f };


	/* UV Option_Rect */
	_float  m_fUV_Offset_Rect[2] = { 0.f, 0.f };
	_float  m_vUV_Scale_Rect[2] = { 1.f, 1.f };
	_float	m_fUV_RotDegree_Rect = { 0.f };	// UV회전


	/* 렉트 Rim & Bloom */
	_float	m_fRimColor_Rect[4] = { 1.f, 1.f, 1.f, 1.f };
	_float	m_vBloomPower_Rect[3] = { 1.f, 1.f, 1.f };

#pragma endregion Rect 옵션 끝



#pragma region Instance(Mesh) 옵션 시작
private:
	_int m_iNumInstance_Mesh	= { 20 };
	_int m_iMaxNumInstance_Mesh = { 20 };

	_int	m_iRecycle_Mesh = { 0 };

	_float	m_vMinMaxLifeTime_Mesh[2] = { 0.f, 0.f };	// 라이프타임

	/* Morph */
	_float m_fMorphTimeTerm = { 0.05f };


	/* RigidBody ============================================== */
	_int	m_iUseRigidBody_Mesh	= { 0 };
	_int	m_iKinetic_Mesh			= { 0 };
	_int	m_iUseGravity_Mesh		= { 0 };

	_float	m_fGravity_Mesh			= { -9.8f };	// 중력 가속도
	_float	m_fFriction_Mesh		= { 0.1f };		// 마찰 계수
	_float	m_fSleepThreshold_Mesh	= { 0.05f };	// 슬립 한계점(1이하로)

	_float	m_vMinMaxPower_Mesh[2]	= { 0.1f, 250.f };
	/* RigidBody ============================================== */


	/* Color */
	_int	m_iColor_Mode_Mesh = { 0 };
	_float	m_fColor_Mul_Mesh[4] = { 1.f, 1.f, 1.f, 1.f };


	/* Position_Mesh Particle */
	_float	m_vMinCenterOffsetPos_Mesh[3] = { 0.f, 0.f, 0.f };
	_float	m_vMaxCenterOffsetPos_Mesh[3] = { 0.f, 0.f, 0.f };

	_float	m_vMinMaxRange_Mesh[2] = { 0.f, 0.f };


	_float	m_vRotationOffsetX_Mesh[2] = { 0.f, 0.f };
	_float	m_vRotationOffsetY_Mesh[2] = { 0.f, 0.f };
	_float	m_vRotationOffsetZ_Mesh[2] = { 0.f, 0.f };


	/* UV Option */
	_float  m_fUV_Offset_Mesh[2] = { 0.f, 0.f };
	_float  m_vUV_Scale_Mesh[2] = { 1.f, 1.f };
	_float	m_fUV_RotDegree_Mesh = { 0.f };


	/* Rim Bloom */
	_float	m_vBloomPower_Mesh[3] = { 1.f, 1.f, 1.f };
	_float	m_fRimColor_Mesh[4] = { 1.f, 1.f, 1.f, 1.f };
	_float	m_fRimPower_Mesh = { 5.f };


	/* Dissolve */
	_float	m_DissolveAmount = { 0.f };


	/* Distortion ============================================== */
	_float m_fSequenceTerm_Distortion_Mesh = { 1.f };
	_int	m_iType_Scroll_Mesh = { 0 };

	_float	m_vScrollSpeeds_Mesh[3] = { 1.f, 1.f, 0.f };
	_float	m_vScales_Distortion_Mesh[3] = { 1.f, 1.f, 1.f };

	_float	m_vDistortion1_Mesh[2] = { 0.1f, 0.1f };
	_float	m_vDistortion2_Mesh[2] = { 0.0f, 0.0f };
	_float	m_vDistortion3_Mesh[2] = { 0.0f, 0.1f };

	_float	m_fDistortionScale_Mesh = { 1.f };
	_float	m_fDistortionBias_Mesh = { 1.f };
	/* Distortion ============================================== */
#pragma endregion Instance(Mesh) 옵션 끝


#pragma region Trail 옵션 시작
private:
	_float	m_vColor_Clip_Trail[4] = { 0.f, 0.f, 0.f, 0.f };	// 쉐이더에서 discard할 값
	_float	m_fColor_Mul_Trail[4] = { 1.f, 1.f, 1.f, 1.f };		// 쉐이더에서 추가로 곱해줄 색 값
	
	_float m_vPos_0[3] = { 0.f, 0.f, 0.f };	
	_float m_vPos_1[3] = { 0.f, 0.f, 0.f };

	_int	m_iMaxCnt_Trail = { 32 };
	_int	m_iLerpPointNum = { 12 };


	_float	m_vBloomPower_Trail[3]	= { 1.f, 1.f, 1.f };
	_float	m_fRimColor_Trail[4]	= { 1.f, 1.f, 1.f, 1.f };
	_float	m_fRimPower_Trail		= { 5.f };
#pragma endregion Trail 옵션 끝


#pragma region 시퀀서
private:
	int32_t m_iCurSequenceTimePos = { 0 };
#pragma endregion


#pragma region Level Setting
private:
	/* Camera */
	CCamera*	m_pCamera = { nullptr };
	_float3		m_Camera_ResetPos = { 0.f, 1.8f, -5.f };		// 카메라 리셋 위치
	_float4		m_Camera_ResetLookAt = { 0.f, 0.f, 0.f, 1.f };	// 카메라 리셋 LookAt

	/* Sky */
	CSky* m_pSky = { nullptr };
	_int m_iSkyTextureIndex = { 3 };		// 스카이박스 텍스처 인덱스

	/* Grid */
	CGrid*	m_pGrid	= { nullptr };
	_float	m_fColor_Grid[4] = { 0.3f, 0.3f, 0.3f, 1.f };	// 그리드(와이어프레임) 컬러
	_int	m_iShaderPass_Grid = { 2 };


	/* Test Floor */
	CModel_Preview* m_pFloor_Preview = { nullptr };				// 땅바닥
	_float			m_vWorldPosition_Floor[3] = { 0.f, 0.f, 0.f };	// 바닥 모델 월드 이동
	_float			m_vScale_Floor[3] = { 0.f, 0.f, 0.f };			// 바닥 모델 크기

	/* Model_Preview */
	CModel_Preview*	m_pModel_Preview = { nullptr };				// 크기 비교용 보기 모델
	CPart_Preview*	m_pPart_Preview = { nullptr };				// 모델의 파트(트레일 확인용)
	_float			m_vWorldPosition_Model[3] = { 0.f, 0.f, 0.f };	// 크기 비교용 모델 월드 이동
	_float			m_vWorldRotate_Model[3] = { 0.f, 0.f, 0.f };	// 크기 비교용 모델 월드 회전 
	_float			m_vScale_Model[3] = { 0.f, 0.f, 0.f };			// 크기 비교용 모델 크기
	_float			m_fModelRot = { 0.f };

	CEffect* m_pTestEffect = { nullptr };
#pragma endregion



public:
	static CWindow_EffectTool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END

