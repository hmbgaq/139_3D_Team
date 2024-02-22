#pragma once

#include "Imgui_Window.h"

BEGIN(Client)
class CEffect;
class CEffect_Void;
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


// Refactoring Start =====================================================================================================

public:
	void	Update_PlayBarArea();

	void	Update_Tab_Common(CEffect_Void::TYPE_EFFECT eType);
	void	Update_ParticleTab();
	void	Update_RectTab();
	void	Update_MeshTab();

	void	Update_EffectList();
	HRESULT Create_EffectObject(const wstring& strLayerTag, CGameObject* pOwner = nullptr);
	HRESULT Add_Part_Particle();
	HRESULT Add_Part_Rect();
	HRESULT Add_Part_Mesh();

	void	Update_CurMembers(wstring strName);

	void	Update_CurParameters();
	void	Update_CurParameters_Parts();

private:
	map<const wstring, class CEffect*>		m_pEffects;
	map<const wstring, class CGameObject*>	m_CurPartObjects;

	char** m_szEffectNames = { nullptr };
	char* m_cCurEffectName = { nullptr };
	_int			m_iCurEffectIndex = { 0 };
	class CEffect*	m_pCurEffect = { nullptr };


	char** m_szPartNames		= { nullptr };
	char* m_cCurPartName		= { nullptr };
	_int m_iCurPartIndex		= { 0 };
	class CEffect_Void* m_pCurPartEffect = { nullptr };


/* Desc */
private:
	CEffect::EFFECT_DESC* m_pCurEffectDesc = { nullptr };
	CEffect::EFFECT_DESC  m_tEffectDesc_Edit = {};

	CEffect_Particle::PARTICLE_DESC*				m_pParticleDesc = {};
	CVIBuffer_Particle_Point::PARTICLE_POINT_DESC*	m_pParticlePointDesc = {};

	CEffect_Rect::EFFECT_RECT_DESC*					m_pRectDesc = {};
	CEffect_Instance::EFFECT_INSTANCE_DESC*			m_pInstanceDesc = {};

private:
	_int m_iRenderGroup_Particle		= { 7 };
	_int m_iShaderPassIndex_Particle	= { 0 };

	_int  m_iTexIndex_Particle[CEffect_Void::TEXTURE_END] = { };
	_int  m_iMaxTexIndex_Particle[CEffect_Void::TEXTURE_END] = { 13, 19, 8 };

	// Refactoring end   =====================================================================================================


private:
	_int m_iLoop = { 0 };

	_int m_iNumInstance = { 200 };
	_int m_iMaxNumInstance = { 500 };

	_float	m_vTimes_Effect[3] = { 0.f, 8.f, 0.f }; // Wait, LifeTime, Remain
	_float	m_vTimes_Part[3] = { 1.f, 4.f, 0.f };	// Wait, LifeTime, Remain

#pragma region Particle_Option
private:
	CVIBuffer_Particle_Point::TYPE_ACTION	m_eParticleTypeAction = { CVIBuffer_Particle_Point::TYPE_ACTION::SPHERE };
	CVIBuffer_Particle_Point::TYPE_FADE		m_eParticleTypeFade = { CVIBuffer_Particle_Point::TYPE_FADE::FADE_NONE };

	_int	m_iBillBoard = { 0 };

	_float	m_fAddScale = { 0.f };
	_float	m_vAddScale[2] = { 0.f, 0.f };

	_float	m_vMinMaxLifeTime[2] = { 0.f, 0.f };

	_float	m_fRotateUvDegree = { 0.f };

	_float	m_vMinMaxRange[2] = { 0.f, 0.f };
	_float	m_fMaxLengthPosition = { 0.f };

	_float	m_vRotationOffsetX[2] = { 0.f, 0.f };
	_float	m_vRotationOffsetY[2] = { 0.f, 0.f };
	_float	m_vRotationOffsetZ[2] = { 0.f, 0.f };

	_float	m_fParticleAcceleration = { 1.f };
	_float	m_fParticleAccPosition = { 0.1f };

	_float	m_fUseGravityPosition = { 0.1f };
	_float  m_fGravityAcc = { -9.8f };

	_float	m_fColor_Start_Particle[4] = { 1.f, 1.f, 1.f, 1.f };
	_float	m_fColor_End_Particle[4] = { 1.f, 1.f, 1.f, 1.f };
	_float	m_fColor_Cur_Particle[4] = { 1.f, 1.f, 1.f, 1.f };

	_float	m_vWorldPosition[4] = { 0.f, 0.f, 0.f, 1.f };
#pragma endregion

/* For.Save&Load */
public:
	void	Update_SaveLoad();
	virtual	HRESULT		Save_Function(string strPath, string strFileName) override;
	virtual HRESULT		Load_Function(string strPath, string strFileName) override;


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

