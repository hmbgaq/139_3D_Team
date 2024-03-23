#include "Shader_Defines.hlsli"


/* float2, float3, float4 == vector */
/* vector.x == vector.r */
/* vector.xy = 1.f;*/
/* float4x4 == matrix */
/* matrix._11_12 = 1.f; */

/* 셰이더의 전역변수 == 상수테이블(Constant Table) */
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

Texture2D g_DiffuseTexture;
Texture2D g_DiffuseTexture_Second;
Texture2D g_DiffuseTexture_Third;
Texture2D g_DiffuseTexture_Fourth;

Texture2D g_MaskTexture;
Texture2D g_NoiseTexture;
Texture2D g_DepthTexture;

/* Alpha */
float g_Alpha;

/* Loading */
float g_LoadingProgress;

/* Hp */
Texture2D g_HpBarWhite_Texture;
Texture2D g_HpBarRed_Texture;
Texture2D g_HpBarDecal_Texture;
float g_MaxHP;
float g_CurrentHP;
float g_LerpHP;

/* Aim */
float2 g_Recoil;
float2 g_Offset;
Texture2D g_AimTop_Texture;
Texture2D g_AimBottom_Texture;
Texture2D g_AimLeft_Texture;
Texture2D g_AimRight_Texture;

/* CoolDown */
Texture2D g_CoolDownTexture;
float2    g_Center;
float     g_Radius;
float     g_CoolTime;
float     g_MaxCoolTime;
float     g_Ratio;

texture2D g_DissolveTexture;
texture2D g_AlphaTexture;


// ======= Distortion
float g_fFrameTime;
float3 g_vScrollSpeeds;
float3 g_vScales;
float4 g_vColor_Mul;
int g_iColorMode;

float2 g_vDistortion1;
float2 g_vDistortion2;
float2 g_vDistortion3;
float g_fDistortionScale;
float g_fDistortionBias;
// =======

float4 Calculation_ColorBlend(float4 vDiffuse, float4 vBlendColor, int g_iColorMode)
{
    float4 vResault = vDiffuse;
   
    if (0 == g_iColorMode)
    {
      // 곱하기
        vResault = vResault * vBlendColor;
    }
    else if (1 == g_iColorMode)
    {
      // 스크린
        vResault = 1.f - ((1.f - vResault) * (1.f - vBlendColor));
    }
    else if (2 == g_iColorMode)
    {
      // 오버레이
        vResault = max(vResault, vBlendColor);
    }
    else if (3 == g_iColorMode)
    {
      // 더하기
        vResault = vResault + vBlendColor;
    }
    else if (4 == g_iColorMode)
    {
      // 번(Burn)
        vResault = vResault + vBlendColor - 1.f;
    }
 
    return vResault;
}

/* 정점의 변환(월드변환, 뷰변환, 투영변환.)을 수행한다. */
/* 정점의 구성정보를 추가, 삭제등의 변경을 수행한다. */

/* 정점 네개. */
/* 인덱스 여섯개 .*/

/* 객체마다 Render를 돌려, VIBuffer에서 IASetPrimitiveTopology함수를 통해 텍스처의 정보들을 받아온다. */
struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};


struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

/* Distortion */
struct VS_OUT_DISTORTION
{
    float4 vPosition : SV_POSITION;

    float2 vTexcoord : TEXCOORD0;
    float2 vTexcoord1 : TEXCOORD1;
    float2 vTexcoord2 : TEXCOORD2;
    float2 vTexcoord3 : TEXCOORD3;

    float4 vProjPos : TEXCOORD4;
};

struct PS_IN_DISTORTION
{
    float4 vPosition : SV_POSITION;

    float2 vTexcoord : TEXCOORD0;
    float2 vTexcoord1 : TEXCOORD1;
    float2 vTexcoord2 : TEXCOORD2;
    float2 vTexcoord3 : TEXCOORD3;

    float4 vProjPos : TEXCOORD4;
};
/* Distortion End */

/* 받아온 텍스처의 정보를 어떻게 해줄지 세팅해주자 */
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

	/* In.vPosition * 월드 * 뷰 * 투영 */
    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;

	/* 투영까지 올렸고, z나누기 직전이다. z나누기는 PS_IN의 vPosition으로 받을 때, SV_POSITION옵션으로 받게되면 알아서 처리가된다. */
	
    return Out;
}

/* 통과된 정점을 대기 .*/

/* 투영변환 : /w */ /* -> -1, 1 ~ 1, -1 */ 
/* 뷰포트변환-> 0, 0 ~ WINSX, WINSY */ 
/* 래스터라이즈 : 정점정보에 기반하여 픽셀의 정보를 만든다. */


struct PS_IN
{
    float4 vPosition : SV_POSITION; // z값을 처리한 상태가 된다.
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

/* 픽셀셰이더 : 픽셀의 색!!!! 을 결정한다. */
PS_OUT PS_MAIN(PS_IN In) // 0
{
    PS_OUT Out = (PS_OUT) 0;

	/* 이 셰이더를 사용하는 객체의 색상을 g_DiffuseTexture의 색상으로 적용시키겠다. */
    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
   
    Out.vColor.a -= g_Alpha;
    
    if (Out.vColor.a < 0.1f)
        discard;
	
    return Out;
}

PS_OUT PS_HPBAR_GAUGE_LERP(PS_IN In) // 1
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vLerpColor = g_HpBarWhite_Texture.Sample(LinearSampler, In.vTexcoord); // Hp Pre
    float4 vGaugeColor = g_HpBarRed_Texture.Sample(LinearSampler, In.vTexcoord); // Hp Cur

    if (g_LerpHP / g_MaxHP < In.vTexcoord.x) // 현재 체력과 최대 체력의 비에 따라 UV좌표가 잘린다.
        discard;

    if (g_LerpHP / g_MaxHP >= In.vTexcoord.x && g_CurrentHP / g_MaxHP <= In.vTexcoord.x)
        Out.vColor = vLerpColor;

    if (g_CurrentHP / g_MaxHP > In.vTexcoord.x) 
        Out.vColor = vGaugeColor;


    //if (vGaugeColor.a < 0.1f)
    //    discard;
    //
    //Out.vColor = lerp(vLerpColor, vGaugeColor, vGaugeColor.a);
    
    return Out;
}

/* Loading */
PS_OUT PS_MAIN_LOADING(PS_IN In) // 2
{
    PS_OUT Out = (PS_OUT) 0;

    float fLoadingPer = 1.f;

    if (In.vTexcoord.x < g_LoadingProgress || In.vTexcoord.x > (g_LoadingProgress + fLoadingPer))
    {
        Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    }
    else
    {
        discard;
		//Out.vColor = float4(0.0, 0.0, 0.0, 0.0);
    }

    //Out.vColor.a = g_Alpha;
    
    return Out;
}

/* Loading */
PS_OUT PS_MAIN_OPTION_BACKGROUND(PS_IN In) // 3
{
    PS_OUT Out = (PS_OUT) 0;
    //
    //float fLoadingPer = 1.f;
    //
    //g_DiffuseTexture;   // Background Texture
    //g_DissolveTexture;  // Fog Texture
    //g_AlphaTexture;     // AlphaTexture
    //
    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    //Out.vColor.a = g_Alpha;
    return Out;
}

/* AIM_CROSSHAIR */ // 크기조절/위치변경 진행중
PS_OUT PS_MAIN_AIM_CROSSHAIR(PS_IN In) // 4
{
    PS_OUT Out = (PS_OUT) 0;
    
    // 이미지 간격을 결정하는 값으로 g_Recoil을 사용
    float2 vOffset = g_Offset * g_Recoil;

    // 각 이미지의 샘플링 위치를 살짝 이동하여 겹치지 않도록 조정
    float2 vTopTexCoord = In.vTexcoord + float2(0, vOffset.y);
    float2 vBottomTexCoord = In.vTexcoord + float2(0, -vOffset.y);
    float2 vLeftTexCoord = In.vTexcoord + float2(-vOffset.x, 0);
    float2 vRightTexCoord = In.vTexcoord + float2(vOffset.x, 0);

    // 각 텍스처 샘플링
    float4 vTopColor = g_AimTop_Texture.Sample(LinearSampler, vTopTexCoord);
    float4 vBottomColor = g_AimBottom_Texture.Sample(LinearSampler, vBottomTexCoord);
    float4 vLeftColor = g_AimLeft_Texture.Sample(LinearSampler, vLeftTexCoord);
    float4 vRightColor = g_AimRight_Texture.Sample(LinearSampler, vRightTexCoord);

    // 상하좌우 이미지를 십자 형태로 조합
    float4 vCombinedColor = vTopColor + vBottomColor + vLeftColor + vRightColor;

    // 조합된 결과를 출력
    Out.vColor = vCombinedColor;

    //Out.vColor.a = g_Alpha;
    return Out;
}


// 원을 그리는 함수
float Circle(float2 uv, float2 center, float radius)
{
    float2 diff = uv - center;
    return saturate(1 - length(diff) / radius);
}

PS_OUT PS_MAIN_COOLTIME(PS_IN In) // 5
{
    PS_OUT Out = (PS_OUT) 0; // 초기화

    float4 vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord); // Diffuse Tex Sampling
    float4 vMaskColor = g_MaskTexture.Sample(LinearSampler, In.vTexcoord); // Mask Tex Sampling

    if (vMaskColor.r > 0.9f && vMaskColor.g > 0.9f && vMaskColor.b > 0.9f)
    {
        Out.vColor = saturate(vColor);
        if (Out.vColor.a < 0.1f)
            discard;
    }
    else
        discard;

	// 여기까지 마스크를 씌운 상태

    float2 vDir = In.vTexcoord - float2(0.5f, 0.5f); // float2(0.5f, 0.5f)는 중점이다.
    vDir = normalize(vDir); // 방향벡터 Normalize
    float2 vUpDir = float2(0.0f, sign(vDir.x));
    vUpDir = normalize(vUpDir);

    float fDot = dot(vUpDir, vDir); // 두 벡터를 내적한다.
    float fDotRatio = g_Ratio;

	// 방향벡터가 음수인 경우, 비교할 기준 벡터의 방향은 위
    if (vDir.x < 0.f)
    {
        fDotRatio -= 0.5f;
    }

    fDotRatio = fDotRatio * 4.f - 1.f;

    if (fDotRatio < fDot) // 잔여 쿨타임이 직관적으로 보여지는 픽셀이다
    {
		//Out.vColor.rgb = lerp(vColor.rgb, float3(0.0f, 0.0f, 0.0f), 0.5f);
        Out.vColor.a = 0.f;
    }

	// 특정 영역에서만 온전한 원본 이미지가 표시된다.
    return Out;
}

//// 픽셀 셰이더 메인 함수
//PS_OUT PS_MAIN_COOLTIME(PS_IN In) // 5
//{
//    PS_OUT Out = (PS_OUT) 0;
//    
//    float2 center = float2(0.5f, 0.5f); // 중심 좌표
//    float radius = 0.4f; // 반지름
//    float thickness = 0.1f; // 게이지 두께
//
//    // 현재 쿨타임 값을 이용하여 게이지를 계산
//    float progress = saturate(1.0f - g_CoolTime / g_MaxCoolTime); // 현재 쿨타임을 [0, 1] 범위로 변환
//    float angle = progress * 1 * 3.14159265359f; // 0부터 2파이까지의 각도 범위로 변환
//    //        (* 가운데 *) 이부분의 값은 증감시켰을 때 이미지가 잘리는 범위크기.
//    
//    // 게이지의 시작과 끝 각도 계산
//    float startAngle = 0.0f;
//    float endAngle = angle;
//
//    // 게이지 색상
//    float4 gaugeColor = float4(0.0f, 1.0f, 0.0f, 1.0f); // 녹색
//
//    // 게이지를 그리기 위한 조건식
//    float2 uv = In.vTexcoord;
//    float2 diff = uv - center;
//    float distance = length(diff);
//    float angleUV = atan2(diff.y, diff.x);
//    float angleDeg = angleUV * (360.0f / 3.14159265359f);
//
//    // 게이지를 그리기 위한 조건식
//    float gaugeValue = saturate((angleDeg - startAngle) / (endAngle - startAngle));
//    float gauge = smoothstep(radius - thickness, radius, distance);
//
//    // 게이지를 그리는 부분
//    float4 finalColor = lerp(float4(0.0f, 0.0f, 0.0f, 0.0f), gaugeColor, gauge * gaugeValue);
//
//    Out.vColor = finalColor;
//    
//    return Out;
//    
//    //PS_OUT Out;
//    //
//    //// 쿨타임 텍스처에서 샘플링하여 색상을 가져오고, 원형 이미지로 변환
//    //float  fCircleValue = Circle(In.vTexcoord, g_Center, g_Radius);
//    //float4 vTexColor = g_CoolDownTexture.Sample(LinearSampler, In.vTexcoord);
//    //
//    //// 샘플된 색상을 원형 이미지에 맞게 조절하여 출력
//    //Out.vColor = vTexColor * fCircleValue;
//    //
//    ////Out.vColor.a = g_Alpha;
//    //return Out;
//}

VS_OUT_DISTORTION VS_MAIN_DISTORTION(VS_IN In)
{
    VS_OUT_DISTORTION Out = (VS_OUT_DISTORTION) 0;

	/* In.vPosition * 월드 * 뷰 * 투영 */
    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;

    return Out;
}

PS_OUT PS_MAIN_DISTORTION(PS_IN_DISTORTION In) // 6
{
    PS_OUT Out = (PS_OUT) 0;
    /*
    1. Mask로 모양을 잡는다.
    2. Noise로 일렁거리는 효과를 준다.
    3. Diffuse로 색상을 정해준다.
    
    => Mask + Noise + Diffuse를 섞어서 모양 + 효과 + 색상이 합쳐진 결과물을 컬러로 내보낸다.
    */
    float4 vNoise1;
    float4 vNoise2;
    float4 vNoise3;

    float4 vFinalNoise;
    float  fPerturb;
    float2 vNoiseCoords;
    float4 vFireColor;
    float4 vAlphaColor;


	// 노이즈 텍스쳐의 좌표를 첫번째 크기 및 윗방향 스크롤 속도 값을 이용하여 계산 x 3
    In.vTexcoord1 = (In.vTexcoord * g_vScales.x);
    In.vTexcoord1.y = In.vTexcoord1.y + (g_fFrameTime * g_vScrollSpeeds.x);

    In.vTexcoord2 = (In.vTexcoord * g_vScales.y);
    In.vTexcoord2.y = In.vTexcoord2.y + (g_fFrameTime * g_vScrollSpeeds.y);

    In.vTexcoord3 = (In.vTexcoord * g_vScales.z);
    In.vTexcoord3.y = In.vTexcoord3.y + (g_fFrameTime * g_vScrollSpeeds.z);


	// 동일한 노이즈 텍스쳐를 서로 다른 세 텍스쳐 좌표를 사용하여 세 개의 다른 크기의 노이즈를 얻는다.
    vNoise1 = g_NoiseTexture.Sample(LinearSampler, In.vTexcoord1);
    vNoise2 = g_NoiseTexture.Sample(LinearSampler, In.vTexcoord2);
    vNoise3 = g_NoiseTexture.Sample(LinearSampler, In.vTexcoord3);

	// 노이즈 값의 범위를 (0, 1)에서 (-1, +1)이 되도록한다.
    vNoise1 = (vNoise1 - 0.5f) * 2.0f;
    vNoise2 = (vNoise2 - 0.5f) * 2.0f;
    vNoise3 = (vNoise3 - 0.5f) * 2.0f;

	// 노이즈의 x와 y값을 세 개의 다른 왜곡 x및 y좌표로 흩뜨린다.
    vNoise1.xy = vNoise1.xy * g_vDistortion1.xy;
    vNoise2.xy = vNoise2.xy * g_vDistortion2.xy;
    vNoise3.xy = vNoise3.xy * g_vDistortion3.xy;

	// 왜곡된 세 노이즈 값들을 하나의 노이즈로 함성한다.
    vFinalNoise = vNoise1 + vNoise2 + vNoise3;

	// 입력으로 들어온 텍스쳐의 Y좌표를 왜곡 크기와 바이어스 값으로 교란시킨다.
	// 이 교란은 텍스쳐의 위쪽으로 갈수록 강해져서 맨 위쪽에는 깜박이는 효과를 만들어낸다.
    fPerturb = ((1.0f - In.vTexcoord.y) * g_fDistortionScale) + g_fDistortionBias;

	// 불꽃 색상 텍스쳐를 샘플링하는데 사용될 왜곡 및 교란된 텍스쳐 좌표를 만든다.
    vNoiseCoords.xy = (vFinalNoise.xy * fPerturb) + In.vTexcoord.xy;


	// 왜곡되고 교란된 텍스쳐 좌표를 이용하여 불꽃 텍스쳐에서 색상을 샘플링한다.
	// wrap를 사용하는 스테이트 대신 clamp를 사용하는 스테이트를 사용하여 불꽃 텍스쳐가 래핑되는 것을 방지한다.
    vFireColor = g_DiffuseTexture.Sample(ClampSampler, vNoiseCoords.xy);


	// 왜곡되고 교란된 텍스쳐 좌표를 이용하여 알파 텍스쳐에서 알파값을 샘플링한다. (불꽃의 투명도를 지정하는 데 사용)
	// wrap를 사용하는 스테이트 대신 clamp를 사용하는 스테이트를 사용하여 불꽃 텍스쳐가 래핑되는 것을 방지한다.
    vAlphaColor = g_MaskTexture.Sample(ClampSampler, vNoiseCoords.xy);

    vFireColor.a = vAlphaColor;

       // 컬러 혼합
    Out.vColor = Calculation_ColorBlend(vFireColor, g_vColor_Mul, g_iColorMode);
    
    return Out;
}

technique11 DefaultTechnique
{
	/* 내가 원하는 특정 셰이더들을 그리는 모델에 적용한다. */
    pass Default // 0
    {
		/* 셰이더(렌더스테이츠) 그리기전에 적용할것들 세팅해주고 */
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend_Add, float4(0.0f, 0.0f, 0.0f, 1.0f), 0xffffffff);
		
		/* 렌더스테이츠 */
        VertexShader = compile vs_5_0 VS_MAIN(); // 값 받고
        GeometryShader = NULL; // 옵션들 적용하고
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN(); // 마지막으로 결정/세팅 한 후 출력한다. (ex : 색상)
    }
    
    pass HPBarGauge_Lerp // 1
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend_Add, float4(0.0f, 0.0f, 0.0f, 1.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_HPBAR_GAUGE_LERP();
    }

    pass LoadingBar_Gauge // 2
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend_Add, float4(0.0f, 0.0f, 0.0f, 1.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LOADING();
    }

    pass Option_Background // 3
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend_Add, float4(0.0f, 0.0f, 0.0f, 1.0f), 0xffffffff);
   
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_OPTION_BACKGROUND();
    }

    pass Aim_Crosshair // 4
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend_Add, float4(0.0f, 0.0f, 0.0f, 1.0f), 0xffffffff);
   
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_AIM_CROSSHAIR();
    }

    pass CoolDown // 5
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend_Add, float4(1.f, 1.f, 1.f, 1.f), 0xffffffff);
   
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_COOLTIME();
    }

    pass Distortion // 6
    {
        SetBlendState(BS_AlphaBlend_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetDepthStencilState(DSS_DepthStencilEnable, 0);
        SetRasterizerState(RS_Cull_None);

        VertexShader = compile vs_5_0 VS_MAIN_DISTORTION();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DISTORTION();
    }
}
