#pragma once

#include "Imgui_Window.h"

BEGIN(Client)
class CEffect;
class CEffect_Void;
class CEffect_Particle;
//class CEffect_Rect;
class CEffect_Instance;
class CEffect_Trail;

class CSky;
class CProjectile;

class CWindow_EffectTool final : public CImgui_Window
{
public:
	enum TYPE_EDIT	{EDIT_EFFECT, EDIT_TRAIL, EDIT_END };
	enum TYPE_FILE { FILE_EFFECT, FILE_PART_PARTICLE, FILE_PART_MESH, FILE_TRAIL, TYPE_FILE_END };

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
	void	Update_SaveLoad_Menu();			// 저장 불러오기 메뉴 업데이트
	void	Update_SaveLoad_Trail_Menu();	// 저장 불러오기 메뉴(트레일) 업데이트
	void	Update_SaveLoad_Part_Menu();	// 저장 불러오기 메뉴(파트이펙트) 업데이트

	virtual	HRESULT		Save_Function(string strPath, string strFileName) override;
	virtual HRESULT		Load_Function(string strPath, string strFileName) override;


	HRESULT		Save_Temp(string strPath);
	HRESULT		Load_Temp(string strPath, TYPE_FILE eType_File);


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

	void	Attach_Tool(string strBoneTag);

/* For.Window Update (창 업데이트) */
public:
	void	Update_LevelSetting_Window();	// 레벨(환경) 세팅 창(카메라, 스카이박스, 크기비교용 모델 등...)

	void	Update_SaveSlot_Window();		// 임시 저장 슬롯 창

	void	Update_EffectList_Window();			// 이펙트 리스트박스 창
	void	Update_EffectTrail_Window();		// 트레일 (왼쪽 창)

	void	Update_EffectTransform_Window();		// 이펙트 트랜스폼 창

	void	Update_Timeline_Window();		// 타임라인 창
	void	Update_NeoSequencer_Window();	// 시퀀서 창

	void	Update_ImageList_Window();		// 텍스처 이미지 미리보기, 리스트

	void	Update_ParticleTab();			// 파티클 탭
	//void	Update_RectTab();				// 렉트 탭
	void	Update_MeshTab();				// 메쉬 탭
	void	Update_TrailTab(_float fTimeDelta);				// 트레일 탭


/* For.Create & Add (이펙트 생성 관련) */
	HRESULT Create_EffectObject(const wstring& strLayerTag, CGameObject* pOwner = nullptr);		// 파트 이펙트를 담을 이펙트 오브젝트 생성 함수
	HRESULT Add_Part_Particle();						// 파트:2D파티클 추가
	//HRESULT Add_Part_Rect();							// 파트:Rect 추가
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

	void	Update_CurParameters_Trail();			// 현재 트레일의 정보 업데이트

public:
	void	Select_EasingType(EASING_TYPE* eType);	// 이징 타입(러프관련) 선택

private:
	TYPE_EDIT	m_eEdit = { EDIT_EFFECT };
	TYPE_FILE	m_eFile = { TYPE_FILE_END };


private:
	map<const wstring, class CEffect*>		m_pEffects;
	map<const wstring, class CGameObject*>	m_CurPartObjects;

	char**			m_szEffectNames		= { nullptr };
	char*			m_cCurEffectName	= { nullptr };
	_int			m_iCurEffectIndex	= { 0 };
	class CEffect*	m_pCurEffect		= { nullptr };


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

	///* Rect Desc */
	//CEffect_Rect::EFFECT_RECT_DESC*					m_pRectDesc			= {};		// Rect만의 Desc


	/* Instance(Mesh) Desc */
	CEffect_Instance::EFFECT_INSTANCE_DESC*							m_pInstanceDesc		= {};	// Instance(Mesh)만의 Desc
	CVIBuffer_Effect_Model_Instance::EFFECT_MODEL_INSTANCE_DESC*	m_pMeshBufferDesc	= {};	// Instance(Mesh) 버퍼 Desc
	CEffect_Void::UVSPRITE_DESC*									m_pSpriteDesc_Mesh = {};	// 메시가 사용할 Void의 스프라이트 Desc


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
	_int m_iMaxShaderPassIndex_Particle						= { 14 };
	_int m_iTexIndex_Particle[CEffect_Void::TEXTURE_END]	= {};
	_int m_iMaxTexIndex_Particle[CEffect_Void::TEXTURE_END] = { 29, 9, 175, 244, 24 };


	_int m_iRenderGroup_Rect								= { ECast(CRenderer::RENDER_EFFECT) };
	_int m_iShaderPassIndex_Rect							= { 0 };
	_int m_iMaxShaderPassIndex_Rect							= { 5 };
	_int m_iTexIndex_Rect[CEffect_Void::TEXTURE_END]		= {};
	_int m_iMaxTexIndex_Rect[CEffect_Void::TEXTURE_END]		= { 29, 9, 175, 244, 24 };


	_int m_iRenderGroup_Mesh								= { ECast(CRenderer::RENDER_EFFECT) };
	_int m_iShaderPassIndex_Mesh							= { 0 };
	_int m_iMaxShaderPassIndex_Mesh							= { 7 };
	_int m_iTexIndex_Mesh[CEffect_Void::TEXTURE_END]		= {};
	_int m_iMaxTexIndex_Mesh[CEffect_Void::TEXTURE_END]		= { 29, 9, 175, 244, 24 };


	_int m_iRenderGroup_Trail								= { ECast(CRenderer::RENDER_EFFECT) };
	_int m_iShaderPassIndex_Trail							= { 0 };
	_int m_iMaxShaderPassIndex_Trail						= { 5 };
	_int m_iTexIndex_Trail[CEffect_Void::TEXTURE_END]		= {};
	_int m_iMaxTexIndex_Trail[CEffect_Void::TEXTURE_END]	= { 29, 9, 175, 244, 24 };


private:
	_float	m_vTimeAccs_Effect[3]	= { 0.f, 0.f, 0.f };
	_float	m_vTimeAccs_Part[3]		= { 0.f, 0.f, 0.f };

private:
	_int	m_iLoop = { 0 };
	_int	m_iType_Dead = { 0 };
	_int	m_iParentPivot = { 0 };

	_float	m_vTimes_Effect[3]	= { 0.f, 5.f, 0.f };	// Wait, LifeTime, Remain
	_float	m_vTimes_Part[3]	= { 0.f, 5.f, 0.f };	// Wait, LifeTime, Remain

	_float	m_vWorldPosition_Effect[3]	 = { 0.f, 0.f, 0.f };
	_float	m_vWorldPosition_Part[3]	 = { 0.f, 0.f, 0.f };

	_float	m_vScale_Effect[3] = { 0.f, 0.f, 0.f };
	_float	m_vScale_Part[3] = { 0.f, 0.f, 0.f };

	_float	m_vRotate_Effect[3] = { 0.f, 0.f, 0.f };
	_float	m_vRotate_Part[3] = { 0.f, 0.f, 0.f };

	_float m_vColor_Clip_Part[4] = { 0.f, 0.f, 0.f, 0.f };


	_int	m_iLoop_Part = { 0 };


	// 마스크 UV
	_int	m_iUV_Wave = { 0 };	// 1이 FALSE
	_float m_fUV_WaveSpeed[2] = { 0.f, 0.f };
	_float m_fUV_Offset_Mask[2] = { 0.f, 0.f };
	_float m_fUV_Scale_Mask[2]	= { 1.f, 1.f };


#pragma region Particle 옵션 시작 =====================================================
private:
	_int	m_iNumInstance_Particle		= { 200 };
	_int	m_iMaxNumInstance_Particle	= { 1000 };

	_int	m_iBillBoard				= { 0 };
	_int	m_iSoft						= { 1 };
	_int	m_iUseMask					= { 0 };	// 0이 true	

	/* 파티클만의 속성 */
	_int	m_iRecycle_Particle			= { 0 };
	_int	m_iReverse_Particle			= { 0 };
	_int	m_iType_Action_Particle		= { 0 };
	_int	m_iType_Fade_Particle		= { 0 };
	_int	m_iType_Fade_Takes_Particle = { 0 };

	_float	m_vMinMaxLifeTime_Particle[2] = { 0.f, 0.f };	// 라이프타임

	/* Emitter */
	_float	m_fEmissionTime_Particle = { 0.f };			// 방출 시간 텀
	_int	m_iAddEmitCount_Particle = { 0 };			// 한번 방출 할 때 몇개씩 추가로 방출할건지


	/* RigidBody ============================================== */
	_int	m_iUseRigidBody_Particle	= { 0 };
	_int	m_iKinetic_Particle			= { 0 };
	_int	m_iUseGravity_Particle		= { 0 };

	_float	m_fGravity_Particle = { -9.8f };		// 중력 가속도
	_float	m_fGravity_X_Particle = { 0.f };		// 중력 가속도 X
	_float	m_fGravity_Z_Particle = { 0.f };		// 중력 가속도 Z
	 
	//_float m_fFriction_Particle = { 0.1f };		// 마찰 계수
	_float	m_vFrictionLerp_Pos_Particle[2] = { 0.f, 0.f };		// 어디서부터 러프를 시작하고, 끝낼건지
	_float	m_vStartEnd_Friction_Particle[2] = { 0.1f, 0.1f };


	_float	m_fSleepThreshold_Particle = { 0.05f };	// 슬립 한계점(1이하로)

	_float	m_vMinMaxPower_Particle[2]	= { 0.1f, 250.f };
	_float	m_vMinMaxMass_Particle[2]	= { 10.f, 10.f };

	/* RigidBody ============================================== */

	_float	m_vMinMaxSpeed_Particle[2] = { 1.f, 1.f };
	_float	m_vMinMaxTornadoSpeed_Particle[2] = { 1.f, 1.f };


	/* For.Position */
	_int	m_iType_Dir_Particle = { 0 };

	_float	m_vMinCenterOffsetPos_Particle[3] = { 0.f, 0.f, 0.f };
	_float	m_vMaxCenterOffsetPos_Particle[3] = { 0.f, 0.f, 0.f };

	_float	m_vMinMaxRange_Particle[2] = { 0.f, 0.f };
	_float	m_fMinMaxAddRange_Particle[2] = { 0.f, 0.f };

	_float	m_vMinMaxPosY_Particle[2] = { 0.1f, 3.f };		// 파티클이 올라갈 최고 높이
	_float	m_vMinMaxTheta_Particle[2] = { 0.f, 6.28f };	// 3.14f * 2.f


	/* For.Rotation */
	_int	m_iUseRotAcc_Particle = { 1 };

	//_float	m_vRadian_Particle[3] = { 0.f, 0.f, 0.f };
	_float	m_vMinRadian_Particle[3] = { 0.f, 0.f, 0.f };
	_float	m_vMaxRadian_Particle[3] = { 0.f, 0.f, 0.f };


	_float	m_vMinMaxRadianSpeed_X_Particle[2] = { 0.f, 0.f };
	_float	m_vMinMaxRadianSpeed_Y_Particle[2] = { 0.f, 0.f };
	_float	m_vMinMaxRadianSpeed_Z_Particle[2] = { 0.f, 0.f };

	_float	m_vRotationOffsetX_Particle[2] = { 0.f, 0.f };
	_float	m_vRotationOffsetY_Particle[2] = { 0.f, 0.f };
	_float	m_vRotationOffsetZ_Particle[2] = { 0.f, 0.f };


	/* For.Scale */
	//_float	m_vLerpScale_Pos_Particle[2]	= { 0.f, 1.f };
	_int	m_iUseScaleLerp_Particle		= { 0 };
	_int	m_iScaleRatio_Particle			= { 0 }; // 크기 정비율
	_float	m_vScaleLerp_Up_Pos[2]			= { 0.f, 0.3f };
	_float	m_vScaleLerp_Down_Pos[2]		= { 1.f, 1.f };
	_float	m_vMinMaxWidth_Particle[2]		= { 0.f, 1.f };
	_float	m_vMinMaxHeight_Particle[2]		= { 0.f, 1.f };
	//_float m_vScaleSpeed_Particle[2]		= { 0.005f, 0.05f };

	/* For.Color */
	_int	m_iDynamic_Color_Particle = { 0 };
	_float	m_fColor_Min_Particle[3] = { 1.f, 1.f, 1.f };
	_float	m_fColor_Max_Particle[3] = { 1.f, 1.f, 1.f };
	_float	m_fColor_Cur_Particle[3] = { 1.f, 1.f, 1.f };

	_float	m_fMinMaxAlpha_Particle[2] = { 1.f, 1.f };

	_int	m_iColor_Mode_Particle = { 0 };
	//_float	m_fColor_Mul_Particle[4] = { 1.f, 1.f, 1.f, 1.f };


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

	CEffect_Void::TEXTURE m_eType_Sprite_Particle = { CEffect_Void::TEXTURE_END };
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



#pragma endregion Particle 옵션 끝 =====================================================



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
	_int	m_iNumInstance_Mesh		= { 50 };
	_int	m_iMaxNumInstance_Mesh	= { 1000 };


	_int	m_iType_Mode_Mesh = { 1 }; // MODE_STATIC, MODE_PARTICLE, MODE_END

	_int	m_iType_Action_Mesh = { 0 };
	_int	m_iRecycle_Mesh = { 0 };
	_int	m_iReverse_Mesh = { 0 };
	_int	m_iType_Fade_Mesh = { 0 };
	_int	m_iType_Fade_Takes_Mesh = { 0 };


	_float	m_vMinMaxLifeTime_Mesh[2] = { 0.f, 0.f };	// 라이프타임
	_float	m_vMinMaxSpeed_Mesh[2] = { 1.f, 1.f };
	_float	m_vMinMaxTornadoSpeed_Mesh[2] = { 1.f, 1.f };

	/* Emitter */
	_float	m_fEmissionTime_Mesh = { 0.f };			// 방출 시간 텀
	_int	m_iAddEmitCount_Mesh= { 0 };			// 한번 방출 할 때 몇개씩 추가로 방출할건지


	/* Morph */
	_float m_fMorphTimeTerm = { 0.05f };


	/* RigidBody ============================================== */
	_int	m_iUseRigidBody_Mesh	= { 0 };
	_int	m_iKinetic_Mesh			= { 0 };
	_int	m_iUseGravity_Mesh		= { 0 };

	_float	m_fGravity_Mesh				= { -9.8f };	// 중력 가속도
	_float	m_fGravity_X_Mesh = { 0.f };		// 중력 가속도 X
	_float	m_fGravity_Z_Mesh = { 0.f };		// 중력 가속도 Z

	_float	m_fMinMaxFriction_Mesh[2]	= { 0.1f, 0.1f };		// 마찰 계수
	_float	m_fSleepThreshold_Mesh	= { 0.05f };	// 슬립 한계점(1이하로)

	_float	m_vMinMaxPower_Mesh[2]	= { 0.1f, 250.f };
	_float	m_vMinMaxMass_Mesh[2] = { 10.f, 10.f };
	/* RigidBody ============================================== */


	/* For.Color */
	_int	m_iDynamic_Color_Mesh = { 0 };
	_float	m_fColor_Min_Mesh[3] = { 1.f, 1.f, 1.f };
	_float	m_fColor_Max_Mesh[3] = { 1.f, 1.f, 1.f };
	_float	m_fColor_Cur_Mesh[3] = { 1.f, 1.f, 1.f };

	_float	m_fMinMaxAlpha_Mesh[2] = { 1.f, 1.f };

	_int	m_iColor_Mode_Mesh = { 0 };


	/* For.Scale_Mesh */
	_int	m_iUseScaleLerp_Mesh			= { 0 };
	_float	m_vScaleLerp_Up_Pos_Mesh[2]		= { 0.f, 0.3f };
	_float	m_vScaleLerp_Down_Pos_Mesh[2]	= { 1.f, 1.f };
	_int	m_iScaleRatio_Mesh				= { 0 }; // 크기 정비율?
	_float	m_vStartScale_Mesh[3]			= { 1.f, 1.f, 1.f };
	_float	m_vEndScale_Mesh[3]				= { 1.f, 1.f, 1.f };


	/* Position_Mesh Particle */
	_int	m_iType_Dir_Mesh = { 0 };

	_float	m_vMinCenterOffsetPos_Mesh[3] = { 0.f, 0.f, 0.f };
	_float	m_vMaxCenterOffsetPos_Mesh[3] = { 0.f, 0.f, 0.f };

	_float	m_vMinMaxRange_Mesh[2] = { 0.f, 0.f };
	_float	m_fMinMaxAddRange_Mesh[2] = { 0.f, 0.f };

	_float	m_vMinMaxPosY_Mesh[2] = { 0.1f, 3.f };		// 파티클이 올라갈 최고 높이
	_float	m_vMinMaxTheta_Mesh[2] = { 0.f, 6.28f };	// 3.14f * 2.f


	/* Rotation_Mesh */
	_int	m_iUseRotAcc_Mesh = { 1 }; // 1이 사용안함

	//_float	m_vRadian_Mesh[3] = { 0.f, 0.f, 0.f };
	_float	m_vMinRadian_Mesh[3] = { 0.f, 0.f, 0.f };
	_float	m_vMaxRadian_Mesh[3] = { 0.f, 0.f, 0.f };

	_float	m_vMinMaxRadianSpeed_X_Mesh[2] = { 0.f, 0.f };
	_float	m_vMinMaxRadianSpeed_Y_Mesh[2] = { 0.f, 0.f };
	_float	m_vMinMaxRadianSpeed_Z_Mesh[2] = { 0.f, 0.f };

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


	/* For.Sprite_Mesh ======================================== */
	_int	m_iSprite_Mesh = { 0 };	// 1이 True
	_int	m_iSpriteLoop_Mesh = { 0 };
	_int	m_vUV_MaxTileCount_Mesh[2] = { 7, 7 };
	_float	m_fSequenceTerm_Mesh = { 0.05f };

	CEffect_Void::TEXTURE m_eType_Sprite_Mesh = { CEffect_Void::TEXTURE_END };
	/* For.Sprite_Mesh ======================================== */




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
	_float m_vPos_0[3] = { 0.f, 0.f, 0.f };
	_float m_vPos_1[3] = { 0.f, 0.f, 0.f };

	_int	m_iMaxCnt_Trail = { 32 };
	_int	m_iLerpPointNum = { 12 };

	_int	m_iColor_Mode_Trail = { 0 };
	_float	m_vColor_Clip_Trail[4] = { 0.f, 0.f, 0.f, 0.f };	// 쉐이더에서 discard할 값
	_float	m_fColor_Mul_Trail[4] = { 1.f, 1.f, 1.f, 1.f };		// 쉐이더에서 추가로 곱해줄 색 값


	_float	m_vBloomPower_Trail[3]	= { 1.f, 1.f, 1.f };
	_float	m_fRimColor_Trail[4]	= { 1.f, 1.f, 1.f, 1.f };
	_float	m_fRimPower_Trail		= { 5.f };


	/* UV Option */
	_float  m_fUV_Offset_Trail[2] = { 0.f, 0.f };
	_float  m_vUV_Scale_Trail[2] = { 1.f, 1.f };
	_float	m_fUV_RotDegree_Trail = { 0.f };


	/* 마스크 UV */
	_int	m_iUV_Wave_Trail = { 0 };	// 1이 FALSE
	_float m_fUV_WaveSpeed_Trail[2] = { 0.f, 0.f };
	_float m_fUV_Offset_Mask_Trail[2] = { 0.f, 0.f };
	_float m_fUV_Scale_Mask_Trail[2] = { 1.f, 1.f };


	/* Distortion ============================================== */
	_float m_fSequenceTerm_Distortion_Trail = { 1.f };
	_int	m_iType_Scroll_Trail = { 0 };

	_float	m_vScrollSpeeds_Trail[3] = { 1.f, 1.f, 0.f };
	_float	m_vScales_Distortion_Trail[3] = { 1.f, 1.f, 1.f };

	_float	m_vDistortion1_Trail[2] = { 0.1f, 0.1f };
	_float	m_vDistortion2_Trail[2] = { 0.0f, 0.0f };
	_float	m_vDistortion3_Trail[2] = { 0.0f, 0.1f };

	_float	m_fDistortionScale_Trail = { 1.f };
	_float	m_fDistortionBias_Trail = { 1.f };
	/* Distortion ============================================== */

#pragma endregion Trail 옵션 끝


#pragma region 시퀀서
private:
	int32_t m_iCurSequenceTimePos = { 0 };
#pragma endregion


#pragma region Level Setting
private:
	/* Camera */
	CCamera*	m_pCamera = { nullptr };
	_float		m_fCameraSpeed = { 60.f };
	_float3		m_Camera_ResetPos = { 0.f, 1.8f, -5.f };		// 카메라 리셋 위치
	_float4		m_Camera_ResetLookAt = { 0.f, 0.f, 0.f, 1.f };	// 카메라 리셋 LookAt

	/* Sky */
	CSky* m_pSky = { nullptr };
	_int m_iSkyTextureIndex = { 3 };		// 스카이박스 텍스처 인덱스

	/* Grid */
	CGrid*	m_pGrid	= { nullptr };
	_float	m_fColor_Grid[4] = { 0.5f, 0.5f, 0.5f, 1.f };	// 그리드(와이어프레임) 컬러
	_int	m_iShaderPass_Grid = { 2 };


	/* Test Floor */
	CModel_Preview* m_pFloor_Preview = { nullptr };					// 땅바닥
	_float			m_vWorldPosition_Floor[3] = { 0.f, 0.f, 0.f };	// 바닥 모델 월드 이동
	_float			m_vScale_Floor[3] = { 0.f, 0.f, 0.f };			// 바닥 모델 크기

	/* Model_Preview */
	CModel_Preview*	m_pModel_Preview = { nullptr };				// 크기 비교용 보기 모델
	CPart_Preview*	m_pPart_Preview = { nullptr };				// 모델의 파트(트레일 확인용)
	_float			m_vWorldPosition_Model[3] = { 0.f, 0.f, 0.f };	// 크기 비교용 모델 월드 이동
	_float			m_vWorldRotate_Model[3] = { 0.f, 0.f, 0.f };	// 크기 비교용 모델 월드 회전 
	_float			m_vScale_Model[3] = { 0.f, 0.f, 0.f };			// 크기 비교용 모델 크기
	_float			m_fModelRot = { 0.f };

	CEffect*		m_pTestEffect = { nullptr };

	CGameObject*	m_pTestProjectile = { nullptr };


	string m_strBoneTag = { "" };	// 툴에서 본에 붙이기용
#pragma endregion



public:
	static CWindow_EffectTool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END

