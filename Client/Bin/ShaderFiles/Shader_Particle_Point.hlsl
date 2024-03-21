#include "Shader_Defines.hlsli"

matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D	g_DepthTexture;

Texture2D	g_DiffuseTexture;
Texture2D	g_MaskTexture;
Texture2D	g_NoiseTexture;

Texture2D	g_NormalTexture;

// Camera ====================
vector		g_vCamPosition;
float3		g_vCamDirection;
float		g_fCamFar;
// ===========================


// Sprite ====================
//bool		g_bSprite;
float2		g_UVOffset;
float2		g_UVScale;
// ===========================


bool        g_bBillBoard;
float		g_fDegree;

float       g_fAlpha_Discard;
float3      g_vBlack_Discard;


/* Color */
int			g_iColorMode;
//float4		g_vColor_Mul;




// 소영 Test ( + 참고 주석 )  ================= 
// 일단 값 때려박은거. 수정해도됨. 
// 생각못한게 있었는데, 림라이트 적용하려면 노말벡터가 필요함. 
// 그래서 BloomPower만 적용해서 블룸을 얼마나 넣을것인지 정도는 넣을 수 있음. 
// 참고로 BloomPower 가 0.3 0.3 0.3 을 넣었을때 Calculation_Brightness함수내에서 fPixelBrightness가 임계를 넘지못해서 안나오는경우도 있었음.. 
// 여차하면 이 fPixelBrightness 도 전역으로 떄려서 조절해도됨. 함수만 제대로 작동하면 안에 어떤값이던 던져서 함수를 변형해서 써도 됨 
//  어 ? 노말이 된다고 ?? ??? ?? ???????????? ??
float4	g_vRimColor; /* = { 1.0f, 0.f, 0.f, 0.5f }; /* RimLight */
float	g_fRimPower;
float3	g_vBloomPower; /*= { 0.7f, 0.7f, 0.7f }; /* Bloom */


// Distortion ====================
float	g_fFrameTime;

int		g_iScrollType;

float3	g_vScrollSpeeds;
float3	g_vScales;

float2	g_vDistortion1;
float2	g_vDistortion2;
float2	g_vDistortion3;
float	g_fDistortionScale;
float	g_fDistortionBias;
// ================================


// ================================
struct EffectDesc
{
	float3	g_vDir;
	float	g_Padding;
	
    float4 g_vRight;
    float4 g_vUp;
    float4 g_vLook;
	
	
    float4  g_vColors_Mul;
};
EffectDesc g_EffectDesc[500];


// Custom Function ==============================================================================================================
float2 Rotate_Texcoord(float2 vTexcoord, float fDegree)
{
	float fDegree2Radian = 3.14159265358979323846 * 2 / 360.f;
	float fRotationRadian = fDegree * fDegree2Radian;
	float cosA = cos(fRotationRadian);
	float sinA = sin(fRotationRadian);

	float2x2 RotateMatrix = float2x2(cosA, -sinA, sinA, cosA);

	vTexcoord -= 0.5f;
	vTexcoord = mul(vTexcoord, RotateMatrix);
	vTexcoord += 0.5f;

	return vTexcoord;
}

float2 RotateTexture(float2 texCoord, float angle)
{
    float2 rotatedTexCoord;
    rotatedTexCoord.x = texCoord.x * cos(angle) - texCoord.y * sin(angle);
    rotatedTexCoord.y = texCoord.x * sin(angle) + texCoord.y * cos(angle);
    
    return rotatedTexCoord;
}

float4 Calculation_RimColor(float4 In_Normal, float4 In_Pos)
{
    float fRimPower = 1.f - saturate(dot(In_Normal, normalize((-1.f * (In_Pos - g_vCamPosition)))));
	fRimPower = pow(fRimPower, 5.f) * g_fRimPower;
    float4 vRimColor = g_vRimColor * fRimPower;
    
    return vRimColor;
}

float4 Calculation_Brightness(float4 Out_Diffuse)
{
    float4 vBrightnessColor = float4(0.f, 0.f, 0.f, 0.f);

    float fPixelBrightness = dot(Out_Diffuse.rgb, g_vBloomPower.rgb);
    
    if (fPixelBrightness > 0.99f)
        vBrightnessColor = float4(Out_Diffuse.rgb, 1.0f);

    return vBrightnessColor;
}


float4 Calculation_ColorBlend(float4 vDiffuse, float4 vBlendColor, int iColorMode)
{
    float4 vResault = vDiffuse;
	
    if (0 == iColorMode)
    {
		// 곱하기
        vResault = vResault * vBlendColor;
    }
    else if (1 == iColorMode)
    {
		// 스크린
        vResault = 1.f - ((1.f - vResault) * (1.f - vBlendColor));
    }
    else if (2 == iColorMode)
    {
		// 오버레이
        vResault = max(vResault, vBlendColor);
    }
    else if (3 == iColorMode)
    {
		// 더하기
        vResault = vResault + vBlendColor;
    }
    else if (4 == iColorMode)
    {
		// 번(Burn)
        vResault = vResault + vBlendColor - 1.f;
    }
	
 
    return vResault;
}


// 두 벡터 사이의 각도 계산 (라디안)
float Calculate_AngleBetweenVectors_Radian(float3 v1, float3 v2)
{
	float fDot = dot(normalize(v1), normalize(v2));
	float fAngle = acos(fDot);

	return fAngle;
}

// 두 벡터 사이의 각도 계산 (도 Degree)
float Calculate_AngleBetweenVectors_Degree(float3 v1, float3 v2)
{
	float fRadians = Calculate_AngleBetweenVectors_Radian(v1, v2);
	float fDegree = fRadians * (180.0f / 3.14159265358979323846);

	return fDegree;
}


float4 Calculation_Distortion(float2 In_TexUV, float2 In_vTexcoord1, float2 In_vTexcoord2, float2 In_vTexcoord3)
{
    float4 vDistortionTex_1;
    float4 vDistortionTex_2;
    float4 vDistortionTex_3;
    
    float4 vDistortionTex_Final;
   
	
	// 디스토션 텍스쳐의 텍스쿠드를 첫번째 크기 및 윗방향 스크롤 속도 값을 이용하여 계산 x 3
    // 텍스쿠드에 곱하기는 크기 변화
    In_vTexcoord1 = (In_TexUV * g_vScales.x);
    In_vTexcoord2 = (In_TexUV * g_vScales.y);
    In_vTexcoord3 = (In_TexUV * g_vScales.z);
    
    
    if (0 == g_iScrollType)
    {
        // SCROLL_ROW : 가로 스크롤 
        In_vTexcoord1.x = In_vTexcoord1.x + (g_fFrameTime * g_vScrollSpeeds.x);
        In_vTexcoord2.x = In_vTexcoord2.x + (g_fFrameTime * g_vScrollSpeeds.y);
        In_vTexcoord3.x = In_vTexcoord3.x + (g_fFrameTime * g_vScrollSpeeds.z);
    }
    else if (1 == g_iScrollType)
    {
        // SCROLL_COL : 세로 스크롤      
        In_vTexcoord1.y = In_vTexcoord1.y + (g_fFrameTime * g_vScrollSpeeds.x);
        In_vTexcoord2.y = In_vTexcoord2.y + (g_fFrameTime * g_vScrollSpeeds.y);
        In_vTexcoord3.y = In_vTexcoord3.y + (g_fFrameTime * g_vScrollSpeeds.z);
    }
    else if (2 == g_iScrollType)
    {
        // SCROLL_BOTH : 가로 + 세로 스크롤
        In_vTexcoord1 = In_vTexcoord1 + (g_fFrameTime * g_vScrollSpeeds.x);
        In_vTexcoord2 = In_vTexcoord2 + (g_fFrameTime * g_vScrollSpeeds.y);
        In_vTexcoord3 = In_vTexcoord3 + (g_fFrameTime * g_vScrollSpeeds.z);
    }
    else if (3 == g_iScrollType)
    {
        In_vTexcoord1 = RotateTexture(In_vTexcoord1, g_fFrameTime * g_vScrollSpeeds.x);
        In_vTexcoord2 = RotateTexture(In_vTexcoord2, g_fFrameTime * g_vScrollSpeeds.y);
        In_vTexcoord3 = RotateTexture(In_vTexcoord3, g_fFrameTime * g_vScrollSpeeds.z);
    }


    // 노이즈 텍스처로 디스토션(왜곡효과)를 만든다.
	// 셋 다 동일한 노이즈 텍스쳐이지만, 서로 다른 텍스쿠드를 사용하였기에 서로 다른 모양으로 샘플링된다.  
    vDistortionTex_1 = g_NoiseTexture.Sample(LinearSampler, In_vTexcoord1);
    vDistortionTex_2 = g_NoiseTexture.Sample(LinearSampler, In_vTexcoord2);
    vDistortionTex_3 = g_NoiseTexture.Sample(LinearSampler, In_vTexcoord3);

	
	// 디스토션 값의 범위를 (0, 1)에서 (-1, +1)이 되도록한다.
    vDistortionTex_1 = (vDistortionTex_1 - 0.5f) * 2.0f;
    vDistortionTex_2 = (vDistortionTex_2 - 0.5f) * 2.0f;
    vDistortionTex_3 = (vDistortionTex_3 - 0.5f) * 2.0f;

	
	// 디스토션의 xy값을 세 개의 서로 다른 왜곡xy좌표로 흩뜨린다.
    vDistortionTex_1.xy = vDistortionTex_1.xy * g_vDistortion1.xy;
    vDistortionTex_2.xy = vDistortionTex_2.xy * g_vDistortion2.xy;
    vDistortionTex_3.xy = vDistortionTex_3.xy * g_vDistortion3.xy;

	
	// 왜곡된 세 디스토션 값들을 하나의 디스토션으로 합성한다.
    vDistortionTex_Final = vDistortionTex_1 + vDistortionTex_2 + vDistortionTex_3;

    
    return vDistortionTex_Final;
}
// Custom Function ==============================================================================================================




struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vPSize : PSIZE;

    row_major float4x4 TransformMatrix : WORLD;
    float4 vColor : COLOR0;

    uint iInstanceID : SV_INSTANCEID;
};


struct VS_OUT
{
    /* Distortion */
    float2 vTexcoord1 : TEXCOORD0;
    float2 vTexcoord2 : TEXCOORD1;
    float2 vTexcoord3 : TEXCOORD2;
    
    
    float4 vPosition : POSITION;
    float2 vPSize : PSIZE;
    float4 vColor : COLOR0;

     
    uint iInstanceID : SV_INSTANCEID;
	
};



VS_OUT VS_MAIN_PARTICLE(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    vector vPosition = mul(float4(In.vPosition, 1.f), In.TransformMatrix);

    Out.vPosition = mul(vPosition, g_WorldMatrix);
    Out.vPSize = float2(In.vPSize.x * In.TransformMatrix._11, In.vPSize.y * In.TransformMatrix._22);
    Out.vColor = In.vColor;

    Out.iInstanceID = In.iInstanceID;

    return Out;
}



struct GS_IN
{
    float2 vTexcoord1   : TEXCOORD0;
    float2 vTexcoord2   : TEXCOORD1;
    float2 vTexcoord3   : TEXCOORD2;
    
    float4 vPosition    : POSITION;
    float2 vPSize       : PSIZE;
    float4 vColor       : COLOR0;

     
    uint iInstanceID    : SV_INSTANCEID;
};


struct GS_OUT
{
    float2 vTexcoord1   : TEXCOORD0;
    float2 vTexcoord2   : TEXCOORD1;
    float2 vTexcoord3   : TEXCOORD2;
    
    float4 vPosition    : SV_POSITION;
    float2 vTexcoord    : TEXCOORD3;
    float4 vColor       : COLOR0;
	
     
    uint iInstanceID    : SV_INSTANCEID;
};


/* 지오메트리 쉐이더 : 셰이더안에서 정점을 추가적으로 생성해 준다. */
[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> OutStream)
{
    GS_OUT Out[4];

    float4 vLook;
    float3 vRight, vUp;
	
    if (g_bBillBoard)
    {
        vLook = g_vCamPosition - In[0].vPosition;
        vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook.xyz)) * In[0].vPSize.x * 0.5f;
        vUp = normalize(cross(vLook.xyz, vRight)) * In[0].vPSize.y * 0.5f;
    }
    else
    {
		// 이동 진행 방향벡터를 Up으로 한 새로운 Right, Look 정해주기 ===================================
		//vUp = normalize(g_EffectDesc[In[0].iInstanceID].g_vDir).rgb * In[0].vPSize.y * 0.5f;
		//vLook.rgb = normalize(cross(float3(0.f, 1.f, 0.f), vUp));
		//vRight = normalize(cross(vUp, vLook.rgb)) * In[0].vPSize.x * 0.5f;
		//vLook.rgb = normalize(cross(vRight, vUp)); vLook.a = 0.f;
		
		
        vRight = normalize(g_EffectDesc[In[0].iInstanceID].g_vRight.rgb) * In[0].vPSize.x * 0.5f;
        vUp = normalize(g_EffectDesc[In[0].iInstanceID].g_vUp.rgb) * In[0].vPSize.y * 0.5f;
        vLook = normalize(g_EffectDesc[In[0].iInstanceID].g_vLook);
		
    }

    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);

	// 중앙 원점
    //Out[0].vPosition = mul(float4(In[0].vPosition.xyz + vRight + vUp, 1.f), matVP);
    //Out[1].vPosition = mul(float4(In[0].vPosition.xyz - vRight + vUp, 1.f), matVP);
    //Out[2].vPosition = mul(float4(In[0].vPosition.xyz - vRight - vUp, 1.f), matVP);
    //Out[3].vPosition = mul(float4(In[0].vPosition.xyz + vRight - vUp, 1.f), matVP);
	
	
	// 중앙 원점이 아님! (중앙 위로 원점 바꿔서 찍었음!!)
    Out[0].vPosition = mul(float4(In[0].vPosition.xyz + vRight, 1.f), matVP);
    Out[1].vPosition = mul(float4(In[0].vPosition.xyz - vRight, 1.f), matVP);
    Out[2].vPosition = mul(float4(In[0].vPosition.xyz - vRight - (vUp * 2), 1.f), matVP);
    Out[3].vPosition = mul(float4(In[0].vPosition.xyz + vRight - (vUp * 2), 1.f), matVP);
	
	
	// 아래로 누움
    //Out[0].vPosition = mul(float4(In[0].vPosition.xyz + vRight + vLook.rgb, 1.f), matVP);
    //Out[1].vPosition = mul(float4(In[0].vPosition.xyz - vRight + vLook.rgb, 1.f), matVP);
    //Out[2].vPosition = mul(float4(In[0].vPosition.xyz - vRight - vLook.rgb, 1.f), matVP);
    //Out[3].vPosition = mul(float4(In[0].vPosition.xyz + vRight - vLook.rgb, 1.f), matVP);
	
	
    Out[0].vTexcoord = Rotate_Texcoord(float2(0.f, 0.f), g_fDegree);
    Out[0].vColor = In[0].vColor;

    Out[1].vTexcoord = Rotate_Texcoord(float2(1.f, 0.f), g_fDegree);
    Out[1].vColor = In[0].vColor;

    Out[2].vTexcoord = Rotate_Texcoord(float2(1.f, 1.f), g_fDegree);
    Out[2].vColor = In[0].vColor;

    Out[3].vTexcoord = Rotate_Texcoord(float2(0.f, 1.f), g_fDegree);
    Out[3].vColor = In[0].vColor;



    Out[0].iInstanceID = In[0].iInstanceID;
    Out[1].iInstanceID = In[0].iInstanceID;
    Out[2].iInstanceID = In[0].iInstanceID;
    Out[3].iInstanceID = In[0].iInstanceID;
	
    
    
    for (int i = 0; i < 4; ++i)
    {
        Out[i].vTexcoord1 = In[0].vTexcoord1;
        Out[i].vTexcoord2 = In[0].vTexcoord2;
        Out[i].vTexcoord3 = In[0].vTexcoord3;
    }


  
    OutStream.Append(Out[0]);
    OutStream.Append(Out[1]);
    OutStream.Append(Out[2]);
    OutStream.RestartStrip();

    OutStream.Append(Out[0]);
    OutStream.Append(Out[2]);
    OutStream.Append(Out[3]);
    OutStream.RestartStrip();
	
}



struct PS_IN
{
    float2 vTexcoord1   : TEXCOORD0;
    float2 vTexcoord2   : TEXCOORD1;
    float2 vTexcoord3   : TEXCOORD2;
    
    float4 vPosition    : SV_POSITION;
    float2 vTexcoord    : TEXCOORD3;
    float4 vColor       : COLOR0;
	
     
    uint iInstanceID    : SV_INSTANCEID;
};


struct PS_OUT
{
    float4 vColor       : SV_TARGET0; // Diffuse
    float4 vSolid       : SV_TARGET1;
    float4 vNormal      : SV_TARGET2; // Normal
    float4 vDepth       : SV_TARGET3; // Depth
    float4 vRimBloom    : SV_TARGET4; // RimBloom
    float4 vDistortion  : SV_TARGET5;
};



PS_OUT PS_MAIN_PARTICLE(PS_IN In, uniform bool bSolid)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vFinalDiffuse;
    float4 vAlphaColor;

    float4 vDistortion;
    float fPerturb;
    float2 vDistortedCoord;
    
    
    // 텍스쿠드
    In.vTexcoord = In.vTexcoord * g_UVScale + g_UVOffset;
    In.vTexcoord = Rotate_Texcoord(In.vTexcoord, g_fDegree);
    
    /* Distortion ============================================================ */	
    vDistortion = Calculation_Distortion(In.vTexcoord, In.vTexcoord1, In.vTexcoord2, In.vTexcoord3);
    
    // 입력으로 들어온 텍스쳐의 Y좌표를 왜곡 크기와 바이어스 값으로 교란시킨다. 이 교란은 텍스쳐의 위쪽으로 갈수록 강해져서 맨 위쪽에는 깜박이는 효과를 만들어낸다.
    fPerturb = ((1.0f - In.vTexcoord.y) * g_fDistortionScale) + g_fDistortionBias;
    
    // 텍스쳐를 샘플링하는데 사용될 왜곡 및 교란된 텍스쳐 좌표를(UV) 만든다.
    vDistortedCoord = (vDistortion.xy * fPerturb) + In.vTexcoord.xy;


	// 디퓨즈 텍스처 (clamp 샘플러 사용)
    vFinalDiffuse = g_DiffuseTexture.Sample(ClampSampler, vDistortedCoord.xy);


	// 마스크 텍스처를 알파로 사용 (clamp 샘플러 사용)
    vAlphaColor = g_MaskTexture.Sample(ClampSampler, vDistortedCoord.xy);
    vFinalDiffuse.a *= vAlphaColor;

	/* Discard & Color Mul ==================================================== */
    if (vFinalDiffuse.a <= g_fAlpha_Discard) // 알파 잘라내기
        discard;
	
	// 컬러 혼합
    Out.vColor = Calculation_ColorBlend(vFinalDiffuse, g_EffectDesc[In.iInstanceID].g_vColors_Mul, g_iColorMode);

		
    /* RimBloom ================================================================ */
    //float4 vRimColor = Calculation_RimColor(float4(In.vNormal.r, In.vNormal.g, In.vNormal.b, 0.f), In.vWorldPos);
    //Out.vColor += vRimColor;
    Out.vRimBloom = float4(g_vBloomPower, 1.0f); //Out.vRimBloom = Calculation_Brightness(Out.vDiffuse) /*+ vRimColor*/;
    
	
    if (bSolid)
        Out.vSolid = Out.vColor;
    
	
    return Out;
    
  
}



//  DISTORTION_POST =============================================================================================================
PS_OUT PS_MAIN_DISTORTION_POST(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    In.vTexcoord = In.vTexcoord * g_UVScale + g_UVOffset;
    In.vTexcoord = Rotate_Texcoord(In.vTexcoord, g_fDegree);
    
    

	/* Distortion ============================================================ */ 
    float4 vDistortion;
    
    vDistortion = Calculation_Distortion(In.vTexcoord, In.vTexcoord1, In.vTexcoord2, In.vTexcoord3);
    Out.vDistortion = vDistortion;
    
	
	/* RimBloom ============================================================== */
    Out.vColor = float4(0.f, 0.f, 0.f, 0.f);
    Out.vRimBloom = float4(g_vBloomPower, 1.0f); //Out.vRimBloom = Calculation_Brightness(Out.vDiffuse) /*+ vRimColor*/;
	
      
    return Out;
}
//  DISTORTION_POST =============================================================================================================




technique11 DefaultTechnique
{
	/* 내가 원하는 특정 셰이더들을 그리는 모델에 적용한다. */
	pass Particle  // 0
	{
		SetRasterizerState(RS_Cull_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend_Add, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

		/* 렌더스테이츠 */
		VertexShader	= compile vs_5_0 VS_MAIN_PARTICLE();
		GeometryShader	= compile gs_5_0 GS_MAIN();
		HullShader		= NULL;
		DomainShader	= NULL;
		PixelShader		= compile ps_5_0 PS_MAIN_PARTICLE(false);
	}

    pass Particle_Solid // 1
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend_Add, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

		/* 렌더스테이츠 */
        VertexShader	= compile vs_5_0 VS_MAIN_PARTICLE();
        GeometryShader = compile gs_5_0 GS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_PARTICLE(true);
    }


    pass Distortion_Post // 2
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend_Add, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

		/* 렌더스테이츠 */
        VertexShader = compile vs_5_0 VS_MAIN_PARTICLE();
        GeometryShader = compile gs_5_0 GS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DISTORTION_POST();
    }

}