#pragma once

#include "Imgui_Window.h"


BEGIN(Client)

class CWindow_EffectTool final : public CImgui_Window
{
public:
	enum EFFECTTYPE {EFFECT_PARTICLE, EFFECT_TEXTURE, EFFECT_MESH, EFFECT_TRAIL, EFFECT_END };

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

	void	Update_PartEffects();


	void	Update_EffectList();
	HRESULT Create_EffectObject(const wstring& strLayerTag, CGameObject* pOwner = nullptr);
	HRESULT Add_Part_Particle();


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

	// Refactoring end   =====================================================================================================


private:
	_int m_iLoop = { 0 };

	_int m_iNumInstance = { 200 };
	_int m_iMaxNumInstance = { 500 };

	_float	m_vTimes_Effect[3] = { 0.f, 10.f, 0.f }; // Wait, LifeTime, Remain
	_float	m_vTimes_Particle[3] = { 1.f, 4.f, 0.f }; // Wait, LifeTime, Remain

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

