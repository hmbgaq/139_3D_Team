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
float2 g_Center;
float g_Radius;

Texture2D g_DepthTexture;
Texture2D g_DissolveTexture;
Texture2D g_AlphaTexture;


/* 정점의 변환(월드변환, 뷰변환, 투영변환.)을 수행한다. */
/* 정점의 구성정보를 추가, 삭제등의 변경을 수행한다.*/

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


	//if (vGaugeColor.a < 0.3f)
	//	discard;
	//Out.vColor = lerp(vLerpColor, vGaugeColor, vGaugeColor.a);
    //Out.vColor.a = g_Alpha;
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
// 픽셀 셰이더 메인 함수
PS_OUT PS_MAIN_COOLTIME(PS_IN In) // 5
{
    PS_OUT Out;

    // 쿨타임 텍스처에서 샘플링하여 색상을 가져오고, 원형 이미지로 변환
    float  fCircleValue = Circle(In.vTexcoord, g_Center, g_Radius);
    float4 vTexColor = g_CoolDownTexture.Sample(LinearSampler, In.vTexcoord);

    // 샘플된 색상을 원형 이미지에 맞게 조절하여 출력
    Out.vColor = vTexColor * fCircleValue;

    //Out.vColor.a = g_Alpha;
    return Out;
}

technique11 DefaultTechnique
{
	/* 내가 원하는 특정 셰이더들을 그리는 모델에 적용한다. */
    pass Default // 0
    {
		/* 셰이더(렌더스테이츠) 그리기전에 적용할것들 세팅해주고 */
        SetRasterizerState(RS_Default);
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
        SetRasterizerState(RS_Default);
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
        SetRasterizerState(RS_Default);
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
        SetRasterizerState(RS_Default);
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
        SetRasterizerState(RS_Default);
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
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend_Add, float4(0.0f, 0.0f, 0.0f, 1.0f), 0xffffffff);
   
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_COOLTIME();
    }
}
