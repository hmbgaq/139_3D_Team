#pragma once

#include "Imgui_Window.h"

BEGIN(Engine)
class CEffect_Object;
//class CParticle_Custom;
END

BEGIN(Client)

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
	void	Update_ParticleTab();
	void	Update_Demo_Particle();
	void	Update_ListArea_Particle();
	void	Update_PlayArea_Particle();
	void	Update_AppearArea_Particle();
	void	Update_ActionArea_Particle();

	void	Update_CurParameters();

	/* For.Tool Util */
public:
	HRESULT Ready_Layer_Greed(const wstring& strLayerTag);

	/* For.Particle */
public:
	HRESULT Create_NewParticle();
	HRESULT Ready_Layer_Particle(const wstring& strLayerTag);

public:
	wstring Make_NameWithPin(const wstring& strFrontName);

private:
	map<const wstring, class CParticle_Custom*>	m_pParticles;

private:
	char** m_szParticleNames = { nullptr };
	_int	m_iCurParticleIndex = { 0 };
	_float	m_fLifeTimePositionEdit = { 0.f };
	class CParticle_Custom* m_pCurParticle = { nullptr };

	_int	m_iPlayType = { 0 };
	_int	m_iLoop = { 0 };

	_float m_vRotationX[2] = { 0.f, 0.f };
	_float m_vRotationY[2] = { 0.f, 0.f };
	_float m_vRotationZ[2] = { 0.f, 0.f };
	_float m_fParticleAcceleration = { 1.f };
	_float m_fParticleAccPosition = { 0.1f };
	_float m_fRotateUvDegree = { 0.f };

	_float m_fAddScale = { 0.f };
	_float m_vAddScale[2] = { 0.f, 0.f };

	_float m_vMinMaxLifeTime[2] = { 0.f, 0.f };
	_float m_vMinMaxRange[2] = { 0.f, 0.f };

	_float m_fColor_Particle[3] = { 1.f, 1.f, 1.f };

	_int	m_iTextureIndex = { 0 };

	_int	m_iMaxNumInstance = { 500 };
	_int	m_iNumInstance = { 0 };

	CVIBuffer_Particle_Point::TYPE_ACTION	m_eParticleTypeAction = { CVIBuffer_Particle_Point::TYPE_ACTION::SPHERE };
	CVIBuffer_Particle_Point::TYPE_FADE		m_eParticleTypeFade = { CVIBuffer_Particle_Point::TYPE_FADE::FADE_NONE };

private:
	class CGameInstance* m_pGameInstance = { nullptr };

public:
	static CWindow_EffectTool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END

