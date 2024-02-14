#include "stdafx.h"
#include "PreviewAnimationModel.h"
#include "Model.h"
#include "Animation.h"
#include "GameInstance.h"
#include "Bone.h"


CPreviewAnimationModel::CPreviewAnimationModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CCharacter(pDevice,pContext)
{
}

CPreviewAnimationModel::CPreviewAnimationModel(const CPreviewAnimationModel& rhs)
	: CCharacter(rhs)
{
}

CModel* CPreviewAnimationModel::Get_CurrentModel()
{
	return Get_Body()->Get_Model();
}

HRESULT CPreviewAnimationModel::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	return S_OK;
}

HRESULT CPreviewAnimationModel::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CPreviewAnimationModel::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CPreviewAnimationModel::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CPreviewAnimationModel::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CPreviewAnimationModel::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPreviewAnimationModel::Render_Shadow()
{
	return S_OK;
}

void CPreviewAnimationModel::Init_EditPreviewAnimationModel(const string& In_szModelKey)
{

}

void CPreviewAnimationModel::Change_AnimationFromIndex(const _uint& In_iAnimIndex)
{
}

void CPreviewAnimationModel::Play_Animation(_float fTimeDelta, _bool bIsLoop)
{
	m_pBody->Get_Model()->Play_Animation(fTimeDelta, true);
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
