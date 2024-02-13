#include "stdafx.h"
#include "PreviewAnimationModel.h"
#include "Model.h"
#include "Animation.h"
#include "GameInstance.h"
#include "Bone.h"
#include "Wea"

CPreviewAnimationModel::CPreviewAnimationModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice,pContext)
{
}

CPreviewAnimationModel::CPreviewAnimationModel(const CPreviewAnimationModel& rhs)
	:CGameObject(rhs)
{
}

CModel* CPreviewAnimationModel::Get_CurrentModel()
{
	return nullptr;
}

HRESULT CPreviewAnimationModel::Initialize_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CPreviewAnimationModel::Initialize(void* pArg)
{
	return E_NOTIMPL;
}

void CPreviewAnimationModel::Priority_Tick(_float fTimeDelta)
{
}

void CPreviewAnimationModel::Tick(_float fTimeDelta)
{
}

void CPreviewAnimationModel::Late_Tick(_float fTimeDelta)
{
}

HRESULT CPreviewAnimationModel::Render()
{
	return E_NOTIMPL;
}

HRESULT CPreviewAnimationModel::Render_Shadow()
{
	return E_NOTIMPL;
}

void CPreviewAnimationModel::Init_EditPreviewAnimationModel(const string& In_szModelKey)
{
}

void CPreviewAnimationModel::Change_AnimationFromIndex(const _uint& In_iAnimIndex)
{
}

void CPreviewAnimationModel::Play_Animation(_float fTimeDelta)
{
}

void CPreviewAnimationModel::Add_DebugWeapon(const string& In_szBoneName, const _float3& In_vOffset, const _float In_fSize)
{
}

void CPreviewAnimationModel::Remove_DebugWeapon(const string& In_szBoneName)
{
}

void CPreviewAnimationModel::Clear_DebugWeapon()
{
}

void CPreviewAnimationModel::Clear_ModelWeapon()
{
}

void CPreviewAnimationModel::Bind_KeyFrameEvent(const string& In_szKeyFrameEventFileName)
{
}

void CPreviewAnimationModel::Release_BeforeModel()
{
}

CGameObject* CPreviewAnimationModel::Clone(void* pArg)
{
	return nullptr;
}

void CPreviewAnimationModel::Free()
{
}
