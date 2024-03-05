#include "stdafx.h"
#include "..\Public\Effect_Void.h"

#include "GameInstance.h"

CEffect_Void::CEffect_Void(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CAlphaObject(pDevice, pContext, strPrototypeTag)
{

}

CEffect_Void::CEffect_Void(const CEffect_Void & rhs)
	: CAlphaObject(rhs)
{
}

HRESULT CEffect_Void::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CEffect_Void::Initialize(void* pArg)
{	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	return S_OK;
}

void CEffect_Void::Priority_Tick(_float fTimeDelta)
{

	
}

void CEffect_Void::Tick(_float fTimeDelta)
{
	/* 예시 틀 백업 */
	//if (m_tEffectDesc.bActive_Tool)
	//{
	//	m_fSequenceTime = m_fLifeTime + m_fRemainTime;

	//	if (m_tEffectDesc.bPlay)
	//	{
	//		// 시작지연 누적시간이 지나면 렌더 시작(이 이펙트 시작)
	//		if (m_fWaitingAcc <= m_fWaitingTime)
	//		{
	//			m_fWaitingAcc += fTimeDelta;

	//			if (m_fWaitingAcc >= m_fWaitingTime)
	//			{
	//				m_tEffectDesc.bRender = TRUE;
	//			}
	//			else
	//				return;
	//		}

	//		// 라이프타임 누적 시작
	//		m_fTimeAcc += fTimeDelta;
	//		m_fLifeTimeRatio = min(1.0f, m_fTimeAcc / m_fLifeTime);

	//		/* ======================= 라이프 타임 동작 시작 ======================= */




	//		/* ======================= 라이프 타임 동작 끝  ======================= */

	//		if (m_fTimeAcc >= m_fLifeTime)
	//		{
	//			// 삭제 대기시간 누적 시작
	//			m_fRemainAcc += fTimeDelta;

	//			// 디졸브 시작
	//			m_tEffectDesc.bDissolve = TRUE;
	//			if (m_tEffectDesc.bDissolve)
	//			{
	//				m_tEffectDesc.fDissolveAmount = Easing::LerpToType(0.f, 1.f, m_fRemainAcc, m_fRemainTime, m_tEffectDesc.eType_Easing);
	//			}
	//			// 디졸브 끝


	//			// 이 이펙트의 타임라인 종료
	//			if (m_fRemainAcc >= m_fRemainTime)
	//			{
	//				m_tEffectDesc.bRender = TRUE;
	//				return;
	//			}
	//		}
	//	}
	//}

}

void CEffect_Void::Late_Tick(_float fTimeDelta)
{

}

HRESULT CEffect_Void::Render()
{
	return S_OK;
}

_bool CEffect_Void::Write_Json(json& Out_Json)
{
	__super::Write_Json(Out_Json);

	return true;
}

void CEffect_Void::Load_FromJson(const json& In_Json)
{
	__super::Load_FromJson(In_Json);

}

void CEffect_Void::Write_VoidDesc(json& Out_Json, void* pArg)
{
	EFFECTVOID_DESC tVoidDesc = {};
	*static_cast<EFFECTVOID_DESC*>(&tVoidDesc) = *static_cast<EFFECTVOID_DESC*>(pArg);


	Out_Json["eType_Effect"] = m_eType_Effect;

	string strTag = "";
	m_pGameInstance->WString_To_String(tVoidDesc.strProtoTag, strTag);
	Out_Json["strProtoTag"] = strTag;

	m_pGameInstance->WString_To_String(tVoidDesc.strPartTag, strTag);
	Out_Json["strPartTag"] = strTag;


	//Out_Json["strProtoTag"] = m_pGameInstance->Convert_WString_To_String(tVoidDesc.strProtoTag);
	//Out_Json["strPartTag"] = m_pGameInstance->Convert_WString_To_String(tVoidDesc.strPartTag);


	Out_Json["m_fWaitingTime"] = m_fWaitingTime;
	Out_Json["m_fLifeTime"] = m_fLifeTime;
	Out_Json["m_fRemainTime"] = m_fRemainTime;
	Out_Json["m_fSequenceTime"] = m_fSequenceTime;


	Out_Json["strModelTag"] = m_pGameInstance->Convert_WString_To_String(tVoidDesc.strModelTag);
	for (_int i = 0; i < (_int)TEXTURE_END; i++)
	{
		Out_Json["strTextureTag"][i] = m_pGameInstance->Convert_WString_To_String(tVoidDesc.strTextureTag[i]);
	}

	for (_int i = 0; i < (_int)TEXTURE_END; i++)
	{
		Out_Json["iTextureIndex"][i] = tVoidDesc.iTextureIndex[i];
	}

	Out_Json["iCurNumInstance"] = tVoidDesc.iCurNumInstance;

	Out_Json["iRenderGroup"] = tVoidDesc.iRenderGroup;
	Out_Json["iShaderPassIndex"] = tVoidDesc.iShaderPassIndex;

	Out_Json["bBillBoard"] = tVoidDesc.bBillBoard;
	Out_Json["bDissolve"] = tVoidDesc.bDissolve;

	CJson_Utility::Write_Float2(Out_Json["vUV_Offset"], tVoidDesc.vUV_Offset);
	CJson_Utility::Write_Float2(Out_Json["vUV_Scale"], tVoidDesc.vUV_Scale);

	Out_Json["fUV_RotDegree"] = tVoidDesc.fUV_RotDegree;

	Out_Json["bUV_Wave"] = tVoidDesc.bUV_Wave;
	Out_Json["fUV_WaveSpeed"] = tVoidDesc.fUV_WaveSpeed;

	CJson_Utility::Write_Float4(Out_Json["vColor_Offset"], tVoidDesc.vColor_Offset);
	CJson_Utility::Write_Float4(Out_Json["vColor_Clip"], tVoidDesc.vColor_Clip);
	CJson_Utility::Write_Float4(Out_Json["vColor_Mul"], tVoidDesc.vColor_Mul);

	Out_Json["fBloom"] = tVoidDesc.fBloom;
	Out_Json["fRadial"] = tVoidDesc.fRadial;
	Out_Json["fDissolveAmount"] = tVoidDesc.fDissolveAmount;
	Out_Json["padding"] = tVoidDesc.padding;

	/* States */
	Out_Json["bActive_Tool"] = tVoidDesc.bActive_Tool;
	Out_Json["bPlay"] = tVoidDesc.bPlay;
	Out_Json["bLoop"] = tVoidDesc.bLoop;
	Out_Json["bReverse"] = tVoidDesc.bReverse;
	Out_Json["bRender"] = tVoidDesc.bRender;
	Out_Json["bUseSpriteAnim"] = tVoidDesc.bUseSpriteAnim;
	Out_Json["bUseRigidBody"] = tVoidDesc.bUseRigidBody;

	Out_Json["eType_Easing"] = tVoidDesc.eType_Easing;
	Out_Json["bParentPivot"] = tVoidDesc.bParentPivot;

	for (_int i = 0; i < 4; ++i)
		CJson_Utility::Write_Float4(Out_Json["matPivot"][i], XMLoadFloat4x4(&tVoidDesc.matPivot).r[i]);

	for (_int i = 0; i < 4; ++i)
		CJson_Utility::Write_Float4(Out_Json["matOffset"][i], XMLoadFloat4x4(&tVoidDesc.matOffset).r[i]);

	CJson_Utility::Write_Float3(Out_Json["vPosition_Start"], tVoidDesc.vPosition_Start);
	CJson_Utility::Write_Float3(Out_Json["vPosition_End"], tVoidDesc.vPosition_End);
	Out_Json["bPosition_Lerp"] = tVoidDesc.bPosition_Lerp;

	CJson_Utility::Write_Float3(Out_Json["vRotation_Start"], tVoidDesc.vRotation_Start);
	CJson_Utility::Write_Float3(Out_Json["vRotation_End"], tVoidDesc.vRotation_End);
	Out_Json["bRotation_Lerp"] = tVoidDesc.bRotation_Lerp;

	CJson_Utility::Write_Float3(Out_Json["vScaling_Start"], tVoidDesc.vScaling_Start);
	CJson_Utility::Write_Float3(Out_Json["vScaling_End"], tVoidDesc.vScaling_End);
	Out_Json["bScaling_Lerp"] = tVoidDesc.bScaling_Lerp;

	CJson_Utility::Write_Float3(Out_Json["vVelocity_Start"], tVoidDesc.vVelocity_Start);
	CJson_Utility::Write_Float3(Out_Json["vVelocity_End"], tVoidDesc.vVelocity_End);
	CJson_Utility::Write_Float3(Out_Json["vVelocity_Cur"], tVoidDesc.vVelocity_Cur);
	Out_Json["bVelocity_Lerp"] = tVoidDesc.bVelocity_Lerp;


	CJson_Utility::Write_Float4(Out_Json["vColor_Start"], tVoidDesc.vColor_Start);
	CJson_Utility::Write_Float4(Out_Json["vColor_End"], tVoidDesc.vColor_End);
	CJson_Utility::Write_Float4(Out_Json["vColor_Cur"], tVoidDesc.vColor_Cur);
	Out_Json["bColor_Lerp"] = tVoidDesc.bColor_Lerp;

}

CEffect_Void::EFFECTVOID_DESC CEffect_Void::Load_VoidDesc(const json& In_Json)
{
	EFFECTVOID_DESC tVoidDesc = {};

	/* Effect_Void */
	m_eType_Effect = In_Json["eType_Effect"];

	string strTag = "";
	strTag = static_cast<string>(In_Json["strProtoTag"]);
	m_pGameInstance->String_To_WString(strTag, tVoidDesc.strProtoTag);

	strTag = static_cast<string>(In_Json["strPartTag"]);
	m_pGameInstance->String_To_WString(strTag, tVoidDesc.strPartTag);

	
	//m_pGameInstance->Convert_WString_To_String(tVoidDesc.strProtoTag)	= In_Json["strProtoTag"];
	//m_pGameInstance->Convert_WString_To_String(tVoidDesc.strPartTag)	= In_Json["strPartTag"];


	m_fWaitingTime	= In_Json["m_fWaitingTime"];
	m_fLifeTime		= In_Json["m_fLifeTime"];
	m_fRemainTime	= In_Json["m_fRemainTime"];
	m_fSequenceTime = In_Json["m_fSequenceTime"];


	m_pGameInstance->Convert_WString_To_String(tVoidDesc.strModelTag) = In_Json["strModelTag"];
	for (_int i = 0; i < (_int)TEXTURE_END; i++)
	{
		m_pGameInstance->Convert_WString_To_String(tVoidDesc.strTextureTag[i]) = In_Json["strTextureTag"][i];
		tVoidDesc.iTextureIndex[i] = In_Json["iTextureIndex"][i];
	}

	tVoidDesc.iCurNumInstance	= In_Json["iCurNumInstance"];

	tVoidDesc.iRenderGroup		= In_Json["iRenderGroup"];
	tVoidDesc.iShaderPassIndex	= In_Json["iShaderPassIndex"];

	tVoidDesc.bBillBoard		= In_Json["bBillBoard"];
	tVoidDesc.bDissolve			= In_Json["bDissolve"];


	CJson_Utility::Load_Float2(In_Json["vUV_Offset"], tVoidDesc.vUV_Offset);
	CJson_Utility::Load_Float2(In_Json["vUV_Scale"], tVoidDesc.vUV_Scale);


	tVoidDesc.fUV_RotDegree = In_Json["fUV_RotDegree"];

	tVoidDesc.bUV_Wave		= In_Json["bUV_Wave"];
	tVoidDesc.fUV_WaveSpeed = In_Json["fUV_WaveSpeed"];


	CJson_Utility::Load_Float4(In_Json["vColor_Offset"], tVoidDesc.vColor_Offset);
	CJson_Utility::Load_Float4(In_Json["vColor_Clip"], tVoidDesc.vColor_Clip);
	CJson_Utility::Load_Float4(In_Json["vColor_Mul"], tVoidDesc.vColor_Mul);

	tVoidDesc.fBloom	= In_Json["fBloom"];
	tVoidDesc.fRadial	= In_Json["fRadial"];
	tVoidDesc.fDissolveAmount = In_Json["fDissolveAmount"];
	tVoidDesc.padding	= In_Json["padding"];


	/* States */
	tVoidDesc.bActive_Tool		= In_Json["bActive_Tool"];
	tVoidDesc.bPlay				= In_Json["bPlay"];
	tVoidDesc.bLoop				= In_Json["bLoop"];
	tVoidDesc.bReverse			= In_Json["bReverse"];
	tVoidDesc.bRender			= In_Json["bRender"];
	tVoidDesc.bUseSpriteAnim	= In_Json["bUseSpriteAnim"];
	tVoidDesc.bUseRigidBody		= In_Json["bUseRigidBody"];


	tVoidDesc.eType_Easing = In_Json["eType_Easing"];

	tVoidDesc.bParentPivot = In_Json["bParentPivot"];

	_float4x4 matPivot;
	ZeroMemory(&matPivot, sizeof(_float4x4));
	CJson_Utility::Load_JsonFloat4x4(In_Json["matPivot"], matPivot);
	tVoidDesc.matPivot = matPivot;


	_float4x4 matOffset;
	ZeroMemory(&matOffset, sizeof(_float4x4));
	CJson_Utility::Load_JsonFloat4x4(In_Json["matOffset"], matOffset);
	tVoidDesc.matOffset = matOffset;


	CJson_Utility::Load_Float3(In_Json["vPosition_Start"], tVoidDesc.vPosition_Start);
	CJson_Utility::Load_Float3(In_Json["vPosition_End"], tVoidDesc.vPosition_End);

	tVoidDesc.bPosition_Lerp = In_Json["bPosition_Lerp"];

	CJson_Utility::Load_Float3(In_Json["vRotation_Start"], tVoidDesc.vRotation_Start);
	CJson_Utility::Load_Float3(In_Json["vRotation_End"], tVoidDesc.vRotation_End);
	tVoidDesc.bRotation_Lerp = In_Json["bRotation_Lerp"];

	CJson_Utility::Load_Float3(In_Json["vScaling_Start"], tVoidDesc.vScaling_Start);
	CJson_Utility::Load_Float3(In_Json["vScaling_End"], tVoidDesc.vScaling_End);
	tVoidDesc.bScaling_Lerp = In_Json["bScaling_Lerp"];

	CJson_Utility::Load_Float3(In_Json["vVelocity_Start"], tVoidDesc.vVelocity_Start);
	CJson_Utility::Load_Float3(In_Json["vVelocity_End"], tVoidDesc.vVelocity_End);
	CJson_Utility::Load_Float3(In_Json["vVelocity_Cur"], tVoidDesc.vVelocity_Cur);
	tVoidDesc.bVelocity_Lerp = In_Json["bVelocity_Lerp"];


	CJson_Utility::Load_Float4(In_Json["vColor_Start"], tVoidDesc.vColor_Start);
	CJson_Utility::Load_Float4(In_Json["vColor_End"], tVoidDesc.vColor_End);
	CJson_Utility::Load_Float4(In_Json["vColor_Cur"], tVoidDesc.vColor_Cur);
	tVoidDesc.bColor_Lerp = In_Json["bColor_Lerp"];


	return tVoidDesc;
}

void CEffect_Void::ReSet_Effect()
{
	//__super::ReSet_Effect();

	m_fSequenceAcc		= 0.f;
	m_fTimeAcc			= 0.f;
	m_fWaitingAcc		= 0.f;
	m_fLifeTimeRatio	= 0.f;
	m_fRemainAcc		= 0.f;

	//m_tEffectDesc.fDissolveAmount = 0.f;
	//m_tEffectDesc.bDissolve		 = FALSE;
	//m_tEffectDesc.bRender		 = FALSE;

}

void CEffect_Void::End_Effect()
{
	//__super::End_Effect();

	//if (m_tEffectDesc.bLoop)
	//{
	//	ReSet_Effect();
	//}

}



void CEffect_Void::Free()
{
	__super::Free();

}

