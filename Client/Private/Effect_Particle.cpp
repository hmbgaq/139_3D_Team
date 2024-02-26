#include "stdafx.h"
#include "..\Public\Effect_Particle.h"

#include "GameInstance.h"

#include "Effect.h"

CEffect_Particle::CEffect_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CEffect_Void(pDevice, pContext, strPrototypeTag)
{

}

CEffect_Particle::CEffect_Particle(const CEffect_Particle& rhs)
	: CEffect_Void(rhs)
{
}

HRESULT CEffect_Particle::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));

	return S_OK;
}

HRESULT CEffect_Particle::Initialize(void* pArg)
{
	m_tParticleDesc = *(PARTICLE_DESC*)pArg;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (m_tRigidbodyDesc.bStartJump)
		m_pVIBufferCom->Add_Velocity(m_tParticleDesc.iCurInstanceCnt, m_tRigidbodyDesc.vStartMinVelocity, m_tRigidbodyDesc.vStartMaxVelocity);

	return S_OK;
}

void CEffect_Particle::Priority_Tick(_float fTimeDelta)
{


}

void CEffect_Particle::Tick(_float fTimeDelta)
{

	if (Is_Dead() == true )
		return;

	if (m_bParticleDelete)
	{
		if (m_pVIBufferCom->Get_Finished())
		{
			m_bParticleDie = true;
			if (ECast(LEVEL_TOOL) != m_pGameInstance->Get_CurrentLevel())
				Set_Dead(true);
			return;
		}
	}




	//CVIBuffer_Particle_Point::PARTICLE_POINT_DESC* pDesc = m_pVIBufferCom_Point->Get_Desc();

	if (m_tParticleDesc.bActive_Tool)
	{
		m_fSequenceTime = m_fLifeTime + m_fRemainTime;

		//pDesc->bActive_Tool = TRUE;
		//pDesc->vMinMaxLifeTime.x = m_fWaitingTime;
		//pDesc->vMinMaxLifeTime.y = m_fLifeTime;

		if (m_tParticleDesc.bPlay)
		{
			m_fSequenceAcc += fTimeDelta;

			// 시작지연 누적시간이 지나면 렌더 시작(파티클 시작)
			if (m_fWaitingAcc <= m_fWaitingTime)
			{
				m_fWaitingAcc += fTimeDelta;

				if (m_fWaitingAcc >= m_fWaitingTime)
				{
					m_tParticleDesc.bRender = TRUE;
					//pDesc->bPlay	= TRUE;
				}
				else
					return;
			}

			// 시간 누적 시작
			m_fTimeAcc += fTimeDelta;
			m_fLifeTimeRatio = min(1.0f, m_fTimeAcc / m_fLifeTime);


			/* ======================= 라이프 타임 동작 시작 ======================= */



			/* ======================= 라이프 타임 동작 끝  ======================= */

			if (m_fTimeAcc >= m_fLifeTime)
			{
				// 삭제 대기시간 누적 시작
				m_fRemainAcc += fTimeDelta;

				// 디졸브 시작
				m_tParticleDesc.bDissolve = TRUE;
				if (m_tParticleDesc.bDissolve)
				{
					m_tParticleDesc.fDissolveAmount = Easing::LerpToType(0.f, 1.f, m_fRemainAcc, m_fRemainTime, m_tParticleDesc.eType_Easing);
				}

				if (m_fRemainAcc >= m_fRemainTime)
				{
					m_tParticleDesc.fDissolveAmount = 1.f;
					m_tParticleDesc.bRender = TRUE;
					return;
				}
			}

			if (m_tParticleDesc.bRender)
			{
				m_pVIBufferCom->Update(fTimeDelta);
				//m_pVIBufferCom_Point->Update(fTimeDelta);
			}
		}
	}
	else
	{
		//m_pVIBufferCom_Point->Get_Desc()->bActive_Tool = FALSE;
	}
}

void CEffect_Particle::Late_Tick(_float fTimeDelta)
{
	if (Is_Dead() == true || m_bParticleDie)
		return;

	__super::Late_Tick(fTimeDelta);


	//if (nullptr != m_tParticleDesc.pOwner)
	//{
	//	if (m_tParticleDesc.pOwner->Is_Dead())
	//	{
	//		Set_Dead(true);
	//		return;
	//	}

	//	CTransform* pOwnerTransform = m_tParticleDesc.pOwner->Get_Transform();
	//	if (nullptr != pOwnerTransform)
	//	{
	//		// Position
	//		_vector vCurrentPosition = pOwnerTransform->Get_Position();
	//		_vector vFinalPosition = vCurrentPosition;
	//		vFinalPosition += m_pTransformCom->Get_State(CTransform::STATE_RIGHT) * m_vLocalPos.x;
	//		vFinalPosition += m_pTransformCom->Get_State(CTransform::STATE_UP) * m_vLocalPos.y;
	//		vFinalPosition += m_pTransformCom->Get_State(CTransform::STATE_LOOK) * m_vLocalPos.z;
	//		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(XMVectorGetX(vFinalPosition) + m_vAddOffsetPos.x, XMVectorGetY(vFinalPosition) + m_vAddOffsetPos.y, XMVectorGetZ(vFinalPosition) + m_vAddOffsetPos.z, 1.f));
	//	}
	//}

	//if (m_tParticleDesc.bSortZ)
	//{
	//	Compute_CamDistance();
	//	m_pVIBufferCom->Sort_Z(m_tParticleDesc.iCurInstanceCnt);
	//}
		
	if (m_tParticleDesc.eType_Proj == TYPE_PERSPECTIVE)
	{
		// CRenderer::RENDER_BLEND
		if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP(m_tParticleDesc.iRenderGroup), this)))
			return;
	}


	//if (m_tParticleDesc.bActive_Tool)
	//{
	//	if (m_tParticleDesc.bRender)
	//	{
	//		Compute_CamDistance();

	//		// CRenderer::RENDER_BLEND
	//		if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP(m_tParticleDesc.iRenderGroup), this)))
	//			return;
	//	}
	//}
}

HRESULT CEffect_Particle::Render()
{
	if (m_tParticleDesc.bActive_Tool)
	{
		if (FAILED(Bind_ShaderResources()))
			return E_FAIL;

		/* 이 쉐이더에 0번째 패스로 그릴거야. */
		m_pShaderCom->Begin(m_tParticleDesc.iShaderPassIndex);

		/* 내가 그리려고하는 정점, 인덱스버퍼를 장치에 바인딩해. */
		//m_pVIBufferCom->Bind_VIBuffers(m_tParticleDesc.iCurInstanceCnt);
		m_pVIBufferCom->Bind_VIBuffers();
		//m_pVIBufferCom_Point->Bind_VIBuffers();

		/* 바인딩된 정점, 인덱스를 그려. */
		m_pVIBufferCom->Render();
		//m_pVIBufferCom_Point->Render();
	}


	return S_OK;
}

void CEffect_Particle::ReSet_Effect()
{
	__super::ReSet_Effect();

	m_tParticleDesc.fDissolveAmount = 0.f;
	m_tParticleDesc.bDissolve = FALSE;
	m_tParticleDesc.bRender = FALSE;

	//m_pVIBufferCom->ReSet();

	m_pVIBufferCom->Restart_ParticleBufferDesc(m_tParticleDesc.iCurInstanceCnt);

}

void CEffect_Particle::End_Effect()
{
	__super::End_Effect();

	if (m_tParticleDesc.bLoop)
	{
		ReSet_Effect();
	}

}

_bool CEffect_Particle::Write_Json(json& Out_Json)
{
	__super::Write_Json(Out_Json);

	for (_int i = 0; i < (_int)TEXTURE_END; i++)
		Out_Json["strTextureTag"][i] = m_pGameInstance->Convert_WString_To_String(m_tParticleDesc.strTextureTag[i]);

	Out_Json["strShaderTag"] = m_pGameInstance->Convert_WString_To_String(m_tParticleDesc.strShaderTag);

	return true;
}

void CEffect_Particle::Load_FromJson(const json& In_Json)
{
	__super::Load_FromJson(In_Json);

	for (_int i = 0; i < (_int)TEXTURE_END; i++)
		m_pGameInstance->Convert_WString_To_String(m_tParticleDesc.strTextureTag[i]) = In_Json["strTextureTag"][i];

	m_pGameInstance->Convert_WString_To_String(m_tParticleDesc.strShaderTag) = In_Json["strShaderTag"];

}


void CEffect_Particle::Set_ParticleDesc(const PARTICLE_DESC& tDesc)
{
	m_tParticleDesc = tDesc;

	// 텍스처 셋팅
	//Set_Texture_Diffuse();
	//Set_Texture_Alpha();

	// 버퍼 재시작
	if (m_pVIBufferCom != nullptr)
	{
		m_pVIBufferCom->Restart_ParticleBufferDesc(m_tParticleDesc.iCurInstanceCnt);

		if (m_tRigidbodyDesc.bStartJump)
			m_pVIBufferCom->Add_Velocity(m_tParticleDesc.iCurInstanceCnt, m_tRigidbodyDesc.vStartMinVelocity, m_tRigidbodyDesc.vStartMaxVelocity);
	}

	m_bParticleDie = false;

}

void CEffect_Particle::Set_RigidbodyDesc(const PARTICLE_RIGIDBODY_DESC& tDesc)
{
	m_tRigidbodyDesc = tDesc;

	// 버퍼 재시작
	if (m_pVIBufferCom != nullptr)
	{
		m_pVIBufferCom->Restart_ParticleBufferDesc(m_tParticleDesc.iCurInstanceCnt);

		if (m_tRigidbodyDesc.bStartJump)
			m_pVIBufferCom->Add_Velocity(m_tParticleDesc.iCurInstanceCnt, m_tRigidbodyDesc.vStartMinVelocity, m_tRigidbodyDesc.vStartMaxVelocity);
	}

	m_bParticleDie = false;
}

void CEffect_Particle::Add_Velocity(_float4 _vMinVelocity, _float4 _vMaxVelocity)
{
	if (nullptr == m_pVIBufferCom)
		return;

	m_pVIBufferCom->Add_Velocity(m_tParticleDesc.iCurInstanceCnt, _vMinVelocity, _vMaxVelocity);
}

void CEffect_Particle::Set_Texture_Diffuse()
{
	_uint iCurLevel = m_pGameInstance->Get_CurrentLevel();

	_int  iBufferSizeName = WideCharToMultiByte(CP_UTF8, 0, m_tParticleDesc.strTextureTag[TEXTURE_DIFFUSE].c_str(), -1, nullptr, 0, nullptr, nullptr);
	char* pszFileName = new char[iBufferSizeName];
	WideCharToMultiByte(CP_UTF8, 0, m_tParticleDesc.strTextureTag[TEXTURE_DIFFUSE].c_str(), -1, pszFileName, iBufferSizeName, nullptr, nullptr);
	if (strcmp(pszFileName, "") != 0)
	{
		if (m_pTextureCom[TEXTURE_DIFFUSE] != nullptr)
			Safe_Release(m_pTextureCom[TEXTURE_DIFFUSE]);	// 이전 텍스처 해제
		
		m_pTextureCom[TEXTURE_DIFFUSE] = static_cast<CTexture*>(m_pGameInstance->Clone_Component(iCurLevel, m_tParticleDesc.strTextureTag[TEXTURE_DIFFUSE]));
	}
	else
	{
		// 텍스처 이름이 비어있는 경우(이전 텍스처가 없었던 경우)

		_int  iBufferSizePath = WideCharToMultiByte(CP_UTF8, 0, m_tParticleDesc.strTexturePath[TEXTURE_DIFFUSE].c_str(), -1, nullptr, 0, nullptr, nullptr);
		char* pszFilePath = new char[iBufferSizePath];
		WideCharToMultiByte(CP_UTF8, 0, m_tParticleDesc.strTexturePath[TEXTURE_DIFFUSE].c_str(), -1, pszFilePath, iBufferSizePath, nullptr, nullptr);
		if (strcmp(pszFilePath, "") != 0)
		{
			if (m_pTextureCom[TEXTURE_DIFFUSE] != nullptr)
				Safe_Release(m_pTextureCom[TEXTURE_DIFFUSE]);

			char szFileName[MAX_PATH] = ""; // 파일이름
			_splitpath_s(pszFilePath, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);
			if (strcmp(szFileName, "") == 0)
				m_pTextureCom[TEXTURE_DIFFUSE] = CTexture::Create(m_pDevice, m_pContext, m_tParticleDesc.strTexturePath[TEXTURE_DIFFUSE], 0);
			else
				m_pTextureCom[TEXTURE_DIFFUSE] = CTexture::Create(m_pDevice, m_pContext, m_tParticleDesc.strTexturePath[TEXTURE_DIFFUSE]);
		}
		else
		{
			if (m_pTextureCom[TEXTURE_DIFFUSE] != nullptr)
				Safe_Release(m_pTextureCom[TEXTURE_DIFFUSE]);
		}
		Safe_Delete(pszFilePath);
	}
	Safe_Delete(pszFileName);

	if (m_pTextureCom[TEXTURE_DIFFUSE] != nullptr && m_tParticleDesc.iTextureIndex[TEXTURE_DIFFUSE] >= m_pTextureCom[TEXTURE_DIFFUSE]->Get_NumTextures())
		m_tParticleDesc.iTextureIndex[TEXTURE_DIFFUSE] = m_pTextureCom[TEXTURE_DIFFUSE]->Get_NumTextures() - 1;

}

void* CEffect_Particle::Get_BufferDesc()
{
	CVIBuffer_Particle::PARTICLE_BUFFER_DESC tBufferInfo = {};

	tBufferInfo.pLoop = &m_tParticleDesc.bLoop;

	tBufferInfo.pMaxInstanceCnt = &m_tParticleDesc.iMaxInstanceCnt;

	tBufferInfo.pRange = &m_tParticleDesc.fRange;
	tBufferInfo.pRangeDistance = &m_tParticleDesc.fRangeDistance;

	tBufferInfo.pScaleSameRate = &m_tParticleDesc.bScaleSameRate;
	tBufferInfo.pScaleStart = &m_tParticleDesc.fScaleStart;

	tBufferInfo.pScaleChange = &m_tParticleDesc.bScaleChange;
	tBufferInfo.pScaleChangeStartDelay = &m_tParticleDesc.fScaleChangeStartDelay;

	tBufferInfo.pScaleChangeRandom = &m_tParticleDesc.bScaleChangeRandom;
	tBufferInfo.pScaleChangeTime = &m_tParticleDesc.fScaleChangeTime;

	tBufferInfo.pScaleAdd = &m_tParticleDesc.bScaleAdd;
	tBufferInfo.pScaleLoop = &m_tParticleDesc.bScaleLoop;
	tBufferInfo.pScaleLoopStart = &m_tParticleDesc.bScaleLoopStart;

	tBufferInfo.pScaleMin = &m_tParticleDesc.fScaleMin;
	tBufferInfo.pScaleMax = &m_tParticleDesc.fScaleMax;
	tBufferInfo.pScaleSpeed = &m_tParticleDesc.fScaleSpeed;


	tBufferInfo.pVelocitySpeed = &m_tParticleDesc.fVelocitySpeed;

	tBufferInfo.pVelocityMinStart = &m_tParticleDesc.vVelocityMinStart;
	tBufferInfo.pVelocityMaxStart = &m_tParticleDesc.vVelocityMaxStart;

	tBufferInfo.pVelocityChange = &m_tParticleDesc.bVelocityChange;
	tBufferInfo.pVelocityChangeStartDelay = &m_tParticleDesc.fVelocityChangeStartDelay;

	tBufferInfo.pVelocityChangeRandom = &m_tParticleDesc.bVelocityChangeRandom;
	tBufferInfo.pVelocityChangeTime = &m_tParticleDesc.fVelocityChangeTime;

	tBufferInfo.pVelocityLoop = &m_tParticleDesc.bVelocityLoop;
	tBufferInfo.pVelocityCountCur = &m_tParticleDesc.iVelocityCountCur;
	tBufferInfo.pVelocityCountMax = &m_tParticleDesc.iVelocityCountMax;

	tBufferInfo.pVelocityUse = &m_tParticleDesc.iVelocityUse;
	tBufferInfo.pVelocityMin = m_tParticleDesc.pVelocityMin;
	tBufferInfo.pVelocityMax = m_tParticleDesc.pVelocityMax;
	tBufferInfo.pVelocityTime = m_tParticleDesc.pVelocityTime;


	tBufferInfo.pBillboard = &m_tParticleDesc.bBillBoard;
	tBufferInfo.pRandomAxis = &m_tParticleDesc.bRandomAxis;
	tBufferInfo.pAxis = &m_tParticleDesc.vAxis;
	tBufferInfo.pRandomAngle = &m_tParticleDesc.bRandomAngle;
	tBufferInfo.pAngle = &m_tParticleDesc.fAngle;

	tBufferInfo.pRotationChange = &m_tParticleDesc.bRotationChange;
	tBufferInfo.pRotationChangeStartDelay = &m_tParticleDesc.fRotationChangeStartDelay;

	tBufferInfo.pRotationChangeRandom = &m_tParticleDesc.bRotationChangeRandom;
	tBufferInfo.pRotationChangeTime = &m_tParticleDesc.fRotationChangeTime;
	tBufferInfo.pRotationAdd = &m_tParticleDesc.bRotationAdd;
	tBufferInfo.pRotationSpeed = &m_tParticleDesc.fRotationSpeed;


	tBufferInfo.pLifeTime = &m_tParticleDesc.vLifeTime;

	tBufferInfo.pUseBox = &m_tParticleDesc.bUseBox;
	tBufferInfo.pBoxMin = &m_tParticleDesc.fBoxMin;
	tBufferInfo.pBoxMax = &m_tParticleDesc.fBoxMax;

	tBufferInfo.pRandomStartIndex = &m_tParticleDesc.bRandomStartIndex;
	tBufferInfo.pUVIndex = &m_tParticleDesc.fUVIndex;
	tBufferInfo.pUVMaxCount = &m_tParticleDesc.fUVMaxCount;

	tBufferInfo.pAnimation = &m_tParticleDesc.bAnimation;
	tBufferInfo.pAnimationLoop = &m_tParticleDesc.bAnimationLoop;
	tBufferInfo.pAnimationSpeed = &m_tParticleDesc.fAnimationSpeed;


	tBufferInfo.pFadeCreate = &m_tParticleDesc.bFadeCreate;
	tBufferInfo.pFadeDelete = &m_tParticleDesc.bFadeDelete;

	tBufferInfo.pFadeChange = &m_tParticleDesc.bFadeChange;
	tBufferInfo.pFadeChangeStartDelay = &m_tParticleDesc.fFadeChangeStartDelay;
	tBufferInfo.pFadeIn = &m_tParticleDesc.bFadeIn;
	tBufferInfo.pStartAlpha = &m_tParticleDesc.fStartAlpha;
	tBufferInfo.pFadeSpeed = &m_tParticleDesc.fFadeSpeed;


	tBufferInfo.pColorRandom = &m_tParticleDesc.bColorRandom;
	tBufferInfo.pColor_Start = &m_tParticleDesc.vColor_Start;

	tBufferInfo.pColorChange = &m_tParticleDesc.bColorChange;

	tBufferInfo.pColorChangeRandom = &m_tParticleDesc.bColorChangeRandom;
	tBufferInfo.pColorChangeRandomTime = &m_tParticleDesc.fColorChangeRandomTime;

	tBufferInfo.pColorLoop = &m_tParticleDesc.bColorLoop;
	tBufferInfo.pColorChangeStartDelay = &m_tParticleDesc.fColorChangeStartDelay;

	tBufferInfo.pColorChangeStartM = &m_tParticleDesc.fColorChangeStartM;
	tBufferInfo.pColor_Mid = &m_tParticleDesc.fColorMid;

	tBufferInfo.pColorChangeStartE = &m_tParticleDesc.vColorChangeStartE;
	tBufferInfo.pColor_End = &m_tParticleDesc.vColor_End;

	tBufferInfo.pColorDuration = &m_tParticleDesc.fColorDuration;


	tBufferInfo.pBloomPowerRandom = &m_tParticleDesc.bBloomPowerRandom;
	tBufferInfo.pBloomPower = &m_tParticleDesc.fBloomPower;
	tBufferInfo.pBlurPowerRandom = &m_tParticleDesc.bBlurPowerRandom;
	tBufferInfo.pBlurPower = &m_tParticleDesc.fBlurPower;

	tBufferInfo.pRigidbody = &m_tRigidbodyDesc.bRigidbody;
	tBufferInfo.pGravity = &m_tRigidbodyDesc.bGravity;
	tBufferInfo.pStopZero = &m_tRigidbodyDesc.bStopZero;
	tBufferInfo.pStopStartY = &m_tRigidbodyDesc.bStopStartY;
	tBufferInfo.pGroundSlide = &m_tRigidbodyDesc.bGroundSlide;

	tBufferInfo.pMaxVelocity = &m_tRigidbodyDesc.vMaxVelocity;
	tBufferInfo.pMass = &m_tRigidbodyDesc.fMass;
	tBufferInfo.pFricCoeff = &m_tRigidbodyDesc.fFricCoeff;

	return &tBufferInfo;

}

HRESULT CEffect_Particle::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(iNextLevel, m_tParticleDesc.strShaderTag,
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	CVIBuffer_Particle::PARTICLE_BUFFER_DESC pBufferInfo = *static_cast<CVIBuffer_Particle::PARTICLE_BUFFER_DESC*>(Get_BufferDesc());
	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_VIBuffer_Particle"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &pBufferInfo)))
		return E_FAIL;


	/* For.Com_VIBuffer */
	{
		CVIBuffer_Particle_Point::PARTICLE_POINT_DESC		tVIBufferDesc = {};
		tVIBufferDesc.eType_Action = { CVIBuffer_Particle_Point::TYPE_ACTION::SPHERE };
		tVIBufferDesc.eType_Fade = { CVIBuffer_Particle_Point::TYPE_FADE::FADE_OUT };

		tVIBufferDesc.bActive_Tool = { TRUE };
		tVIBufferDesc.bBillBoard = { TRUE };
		//tVIBufferDesc.bPlay			= { TRUE };
		tVIBufferDesc.bReverse = { FALSE };
		tVIBufferDesc.bLoop = { TRUE };

		tVIBufferDesc.vMinMaxLifeTime = _float2(0.5f, 3.0f);
		tVIBufferDesc.iCurNumInstance = { m_tParticleDesc.iCurInstanceCnt };

		tVIBufferDesc.vMinMaxRange = { 0.1f, 3.f };
		tVIBufferDesc.vCenterPosition = _float3(0.f, 0.f, 0.f);
		tVIBufferDesc.vOffsetPosition = _float3(0.f, 0.f, 0.f);

		tVIBufferDesc.vMinMaxLengthPosition = { 3.f, 3.f };

		tVIBufferDesc.vMinMaxSpeed = _float2(0.1f, 5.0f);
		tVIBufferDesc.fSpeedAcc = { 2.f };
		tVIBufferDesc.fAccPosition = { 0.1f };

		tVIBufferDesc.fGravityAcc = { -9.8 };
		tVIBufferDesc.vCurrentGravity = { 0.f, 0.f, 0.f };

		tVIBufferDesc.vMinMaxRotationOffsetX = { 0.0f, 360.f };
		tVIBufferDesc.vMinMaxRotationOffsetY = { 0.0f, 360.f };
		tVIBufferDesc.vMinMaxRotationOffsetZ = { 0.0f, 360.f };

		tVIBufferDesc.vCurrentRotation = { 0.f, 0.f, 0.f };
		tVIBufferDesc.vMinMaxRotationForce = { 0.f, 0.f, 0.f };

		tVIBufferDesc.vMinMaxScale = _float2(0.2f, 0.5f);
		tVIBufferDesc.vAddScale = { 0.f, 0.f };
		tVIBufferDesc.vMinMaxScaleForce = { 1.f, 1.f };

		tVIBufferDesc.vCurrentColor = _float4(1.f, 1.f, 1.f, 1.f);
		tVIBufferDesc.vColorSpeed = { 0.f, 0.f, 0.f, 0.f };
		tVIBufferDesc.vColorForce = { 0.f, 0.f, 0.f, 0.f };

		tVIBufferDesc.fMinMaxAlpha = { 1.f, 1.f };
		tVIBufferDesc.fAlphaForce = { 0.f };

		tVIBufferDesc.vSpriteUV = { 0.f, 0.f };
		tVIBufferDesc.vSpriteUVForce = { 0.f, 0.f };
		tVIBufferDesc.iSpriteFrameIndex = { 1 };

		//if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_VIBuffer_Particle_Point"),
		//	TEXT("Com_VIBuffer_Point"), reinterpret_cast<CComponent**>(&m_pVIBufferCom_Point), &tVIBufferDesc)))
		//	return E_FAIL;
	}


	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(iNextLevel, m_tParticleDesc.strTextureTag[TEXTURE_DIFFUSE],
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_DIFFUSE]))))
		return E_FAIL;

	if (TEXT("") != m_tParticleDesc.strTextureTag[TEXTURE_MASK])
	{
		/* For.Com_Mask */
		if (FAILED(__super::Add_Component(iNextLevel, m_tParticleDesc.strTextureTag[TEXTURE_MASK],
			TEXT("Com_Mask"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_MASK]))))
			return E_FAIL;
	}

	if (TEXT("") != m_tParticleDesc.strTextureTag[TEXTURE_NOISE])
	{
		/* For.Com_Noise */
		if (FAILED(__super::Add_Component(iNextLevel, m_tParticleDesc.strTextureTag[TEXTURE_NOISE],
			TEXT("Com_Noise"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_NOISE]))))
			return E_FAIL;
	}

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Model_splineMesh_tornado"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;


	CVIBuffer_Effect_Model_Instance::EFFECT_MODEL_INSTANCE_DESC Desc;
	Desc.pModel = m_pModelCom;
	Desc.iNumInstance = 1;


	///* For.Com_VIBuffer */
	//if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_VIBuffer_Effect_Model_Instance"),
	//	TEXT("Com_VIBuffer_Model"), reinterpret_cast<CComponent**>(&m_pVIBufferCom_Model), &Desc)))
	//	return E_FAIL;




	return S_OK;
}


HRESULT CEffect_Particle::Bind_ShaderResources()
{
	//if (FAILED(__super::Bind_ShaderResources()))
	//	return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDegree", &m_tParticleDesc.fUV_RotDegree, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pTextureCom[TEXTURE_DIFFUSE]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", m_tParticleDesc.iTextureIndex[TEXTURE_DIFFUSE])))
		return E_FAIL;

	if (nullptr != m_pTextureCom[TEXTURE_MASK])
	{
		if (FAILED(m_pTextureCom[TEXTURE_MASK]->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", m_tParticleDesc.iTextureIndex[TEXTURE_MASK])))
			return E_FAIL;
	}
	if (nullptr != m_pTextureCom[TEXTURE_NOISE])
	{
		if (FAILED(m_pTextureCom[TEXTURE_NOISE]->Bind_ShaderResource(m_pShaderCom, "g_NoiseTexture", m_tParticleDesc.iTextureIndex[TEXTURE_NOISE])))
			return E_FAIL;
	}
	     

	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4)));


	// 이펙트 정보
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_EffectDesc", m_pVIBufferCom->Get_ParticleShaderInfo().data(), sizeof(CVIBuffer_Particle::PARTICLE_SHADER_DESC) * m_pVIBufferCom->Get_ParticleShaderInfo().size()));
	//FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fUVIndex", &m_pVIBufferCom->Get_ParticleShaderInfo().data()->fUVIndex, sizeof(_float2)));
	//FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fMaxCount", &m_pVIBufferCom->Get_ParticleShaderInfo().data()->fMaxCount, sizeof(_float2)));
	//FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fColor", &m_pVIBufferCom->Get_ParticleShaderInfo().data()->fColor, sizeof(_float3)));
	//FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_pVIBufferCom->Get_ParticleShaderInfo().data()->fAlpha, sizeof(_float)));
	//FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fAxis", &m_pVIBufferCom->Get_ParticleShaderInfo().data()->fAxis, sizeof(_float3)));
	//FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fAngle", &m_pVIBufferCom->Get_ParticleShaderInfo().data()->fAngle, sizeof(_float)));

	//_float3 fAxis = _float3(0.f, 1.f, 0.f);
	//_float fAngle = 0.f;
	//FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fAxis", &fAxis, sizeof(_float3)));
	//FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fAngle", &fAngle, sizeof(_float)));

	//FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fBloomPower", &m_pVIBufferCom->Get_ParticleShaderInfo().data()->fBloomPower, sizeof(_float3)));
	//FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fBlurPower", &m_pVIBufferCom->Get_ParticleShaderInfo().data()->fBlurPower, sizeof(_float)));

	//m_tParticleDesc.bBillBoard = false;
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_bBillBoard", &m_tParticleDesc.bBillBoard, sizeof(_bool)));

	_float3 vBalck_Discard = { m_tParticleDesc.vColor_Clip.x, m_tParticleDesc.vColor_Clip.y, m_tParticleDesc.vColor_Clip.z };
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fBlack_Discard", &vBalck_Discard, sizeof(_float3)));
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fAlpha_Discard", &m_tParticleDesc.vColor_Clip.w, sizeof(_float)));

	//FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_DiscardValue", &m_tParticleDesc.vColor_Clip.w, sizeof(_float)));

	//if (SPRITE == m_tParticleDesc.eType)
	//{
	//	_vector vCamDirection = m_pGameInstance->Get_TransformMatrixInverse(CPipeLine::D3DTS_VIEW).r[2];
	//	vCamDirection = XMVector4Normalize(vCamDirection);
	//	_float4 vCamDirectionFloat4 = {};
	//	XMStoreFloat4(&vCamDirectionFloat4, vCamDirection);

	//	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamDirection", &vCamDirectionFloat4, sizeof(_float4))))
	//		return E_FAIL;


	//	m_tParticleDesc.vUV_Offset = { (_float)(m_tSpriteDesc.vUV_CurTileIndex.x * m_tSpriteDesc.vTileSize.x) / m_tSpriteDesc.vTextureSize.x
	//					, (_float)(m_tSpriteDesc.vUV_CurTileIndex.y * m_tSpriteDesc.vTileSize.y) / m_tSpriteDesc.vTextureSize.y };

	//	m_tParticleDesc.vUV_Scale = { (_float)m_tSpriteDesc.vTileSize.x / m_tSpriteDesc.vTextureSize.x
	//							, (_float)m_tSpriteDesc.vTileSize.y / m_tSpriteDesc.vTextureSize.y };


	//	if (FAILED(m_pShaderCom->Bind_RawValue("g_UVOffset", &m_tParticleDesc.vUV_Offset, sizeof(_float2))))
	//		return E_FAIL;

	//	if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &m_tParticleDesc.vUV_Scale, sizeof(_float2))))
	//		return E_FAIL;
	//}




	//if (FAILED(m_pShaderCom->Bind_RawValue("FX_Variables", &m_tVariables, sizeof(tagFX_Variables))))
	//	return E_FAIL;


	return S_OK;
}

CEffect_Particle* CEffect_Particle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CEffect_Particle* pInstance = new CEffect_Particle(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEffect_Particle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEffect_Particle::Clone(void* pArg)
{
	CEffect_Particle* pInstance = new CEffect_Particle(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEffect_Particle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEffect_Particle::Pool()
{
	return new CEffect_Particle(*this);
}

void CEffect_Particle::Free()
{
	__super::Free();


	Safe_Release(m_pModelCom);
	Safe_Release(m_pVIBufferCom_Model);

	Safe_Release(m_pVIBufferCom);

	for (_int i = 0; i < (_int)TEXTURE_END; i++)
		Safe_Release(m_pTextureCom[i]);

	Safe_Release(m_pShaderCom);
}

