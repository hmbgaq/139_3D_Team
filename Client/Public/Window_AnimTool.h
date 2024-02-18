#pragma once

#include "Imgui_Window.h"

BEGIN(Engine)
class CGameObject;
class CAnimation;
class CCollider;
class CBounding;
class CBone;
END

BEGIN(Client)
class CPreviewAnimationModel;

class CWindow_AnimTool final : public CImgui_Window
{

public:
	struct WINDOW_MAPTOOL_DESC : public ImGuiDESC
	{
	};

protected:
	CWindow_AnimTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CWindow_AnimTool() = default;


public:
	virtual	HRESULT Initialize() override;
	virtual	void	Tick(_float fTimeDelta) override;
	virtual void	Render() override;

public:
	_float			Get_Speed() const { return m_fSpeed; }
	_bool			Is_Stop() const { return m_bStop; }

public:
	void			Call_UpdatePreViewModel();
	void			Call_NextAnimationKey(const _uint& In_Key);

private:
	void			Add_EffectKeyEvent();
	void			Add_EnableWeaponEvent(const _bool In_bEnable);
	void			Add_SoundKeyEvent();
	void			Add_RandomSoundKeyEvent();

	void			Save_KeyEvent();
	HRESULT			Load_KeyEvent();
	void			Clear_KeyEvent();
	void			ClearBack_KeyEvent();
	void			ClearAll_KeyEvent();
	void			Clear_WeaponEvent();

	void			Create_Object(const wstring& strLayerTag, const wstring& strPrototypeTag);
	void			Draw_Player();
	void			Draw_Monster();
	void			Draw_KeyEventEditer();
	void			Draw_AnimationList(_float fTimeDelta);
	void			Draw_BoneList(_float fTimeDelta);
private://콜라이더 
	void			BonePoint_Update();
	void			Create_Bounding(_float3 fPoint,_float fRadius);
	void			Set_Bounding(CBounding_Sphere* _Bounding);

public://문자열 변환 
	char*			ConverWStringtoC(const wstring& wstr);
	char*			ConvertWCtoC(const wchar_t* str);
	wchar_t*		ConvertCtoWC(const char* str);

private:
	CPreviewAnimationModel*	m_pPreViewModel = { nullptr };
	CAnimation*				m_pCurrentAnimation = { nullptr };
	CGameObject*			m_PickingObject = { nullptr };
	CBody*					m_pBody = { nullptr };
	CBounding_Sphere*		m_pBounding = { nullptr };
	CCollider*				m_pCollider = { nullptr };
	BoundingSphere*			m_pSphere = {};

	//애니메이션 재생
	_float					m_fSpeed = 1.f;
	_float					m_fCurrentTrackPosition = 0.f;
	_float					m_fDuration = 0.f;
	_float					m_iColliderSize = 0;

	_float3					m_fBonePosition = { 0.f,0.f,0.f };
	_float4x4				m_fBoneMatrix = {};
	
	_int					m_CurrentAnimationIndex = 0;
	_int					m_iCreateObjectSize = 0;

	_uint					m_iAnimationNum = 0;
	_uint					m_iBoneNum = 0;
	_uint					m_iCreateColliderNum = 0;
	_uint					m_iSelectColliderIndex = 0;

	string					m_strKeyEventFileName = "";
	string					m_strSoundFileName = "";
	string					m_strTest = "";

	vector<string>			m_vObjectTag;
	vector<CGameObject*>	m_CreateList;
	vector<CAnimation*>		m_pAnimation;
	vector<CBone*>			m_pBones;
	vector<CCollider*>		m_pBoneCollider;
	vector<CBone*>			m_vBoneColliderIndex;
public:
	_bool					m_bStop = false;
	_bool					m_bHold = false;
	_bool					m_bKeyEventEditer = false;
	_bool					m_bCreateCheck = false;
	_bool					m_bDeleteCheck = false;
	_bool					m_bListCheck = false;
	_bool					m_bCloneCount = false;
	_bool					m_bFirstcheck = false;
	_bool					m_bTrackPositionCheck = false;
	_bool					m_bguizmo = false;
	_bool					m_bCreatCollider = false;
	_bool					m_bColliderSize = false;
	_bool					m_bDeleteCollider = false;
public:
	static CWindow_AnimTool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END

