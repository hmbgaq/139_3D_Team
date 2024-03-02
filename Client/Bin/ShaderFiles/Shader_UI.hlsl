#include "Shader_Defines.hlsli"


/* float2, float3, float4 == vector */
/* vector.x == vector.r */
/* vector.xy = 1.f;*/
/* float4x4 == matrix */
/* matrix._11_12 = 1.f; */

/* 셰이더의 전역변수 == 상수테이블(Constant Table) */
matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D		g_DiffuseTexture;
texture2D		g_DiffuseTexture_Second;
texture2D		g_DiffuseTexture_Third;
texture2D		g_DiffuseTexture_Fourth;

texture2D		g_HpBarWhite_Texture;
texture2D		g_HpBarRed_Texture;
texture2D		g_HpBarDecal_Texture;

float			g_MaxHP;
float			g_CurrentHP;
float			g_LerpHP;

texture2D		g_DepthTexture;


/* 정점의 변환(월드변환, 뷰변환, 투영변환.)을 수행한다. */
/* 정점의 구성정보를 추가, 삭제등의 변경을 수행한다.*/

/* 정점 네개. */
/* 인덱스 여섯개 .*/

/* 객체마다 Render를 돌려, VIBuffer에서 IASetPrimitiveTopology함수를 통해 텍스처의 정보들을 받아온다. */
struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexcoord : TEXCOORD0;
};


struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;	
};


/* 받아온 텍스처의 정보를 어떻게 해줄지 세팅해주자 */
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	/* In.vPosition * 월드 * 뷰 * 투영 */
	matrix		matWV, matWVP;

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
	float4		vPosition : SV_POSITION; // z값을 처리한 상태가 된다.
	float2		vTexcoord : TEXCOORD0;
};

struct PS_OUT 
{
	float4		vColor : SV_TARGET0;
};

/* 픽셀셰이더 : 픽셀의 색!!!! 을 결정한다. */
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	/* 이 셰이더를 사용하는 객체의 색상을 g_DiffuseTexture의 색상으로 적용시키겠다. */
    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
		
    if (Out.vColor.a < 0.1f)
        discard;
	
     return Out;
}

PS_OUT PS_HPBAR_GAUGE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vBackColor = g_HpBarRed_Texture.Sample(LinearSampler, In.vTexcoord);

    if (g_CurrentHP / g_MaxHP < In.vTexcoord.x) // 현재 체력과 최대 체력의 비에 따라 UV좌표가 잘린다.
    {
        Out.vColor = vBackColor;
        return Out;
    }

    float4 vGaugeColor = g_HpBarWhite_Texture.Sample(LinearSampler, In.vTexcoord);
    if (vGaugeColor.a < 0.3f)
        discard;

    Out.vColor = lerp(vBackColor, vGaugeColor, vGaugeColor.a);

    return Out;
}

PS_OUT PS_HPBAR_GAUGE_LERP(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vLerpColor = g_HpBarWhite_Texture.Sample(LinearSampler, In.vTexcoord);   // Hp Pre
    float4 vGaugeColor = g_HpBarRed_Texture.Sample(LinearSampler, In.vTexcoord);    // Hp Cur
    float4 vDecalColor = g_HpBarDecal_Texture.Sample(LinearSampler, In.vTexcoord);  // Hp Decal

    
    
    Out.vColor = vDecalColor;
    
    if (vDecalColor.a == 0.f)
    {
        if (g_LerpHP / g_MaxHP < In.vTexcoord.x) // 현재 체력과 최대 체력의 비에 따라 UV좌표가 잘린다.
            discard;

        if (g_LerpHP / g_MaxHP >= In.vTexcoord.x && g_CurrentHP / g_MaxHP <= In.vTexcoord.x)
            Out.vColor = vLerpColor;

        if (g_CurrentHP / g_MaxHP > In.vTexcoord.x) 
            Out.vColor = vGaugeColor;
    }
    


	//if (vGaugeColor.a < 0.3f)
	//	discard;
	//Out.vColor = lerp(vLerpColor, vGaugeColor, vGaugeColor.a);

    return Out;
}

PS_OUT PS_INCREASING_PROGRESS(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vGaugeColor = g_HpBarRed_Texture.Sample(LinearSampler, In.vTexcoord); // 게이지 바 Texture
    float4 vBackColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord); // Frame Texture

    if (vGaugeColor.a < 0.3f)
        discard;
    if (vBackColor.a < 0.3f)
        discard;

    if (g_LerpHP / g_MaxHP > In.vTexcoord.x) // 현재 체력과 최대 체력의 비에 따라 UV좌표가 잘린다.
    {
        Out.vColor = vBackColor;
        return Out;
    }

    if (g_CurrentHP / g_MaxHP <= In.vTexcoord.x)
    {
        Out.vColor = vGaugeColor;
    }

    Out.vColor = lerp(vBackColor, vGaugeColor, vGaugeColor.a);

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
		VertexShader = compile vs_5_0 VS_MAIN();	// 값 받고
		GeometryShader = NULL;						// 옵션들 적용하고
		HullShader = NULL;
		DomainShader = NULL;						
		PixelShader = compile ps_5_0 PS_MAIN();		// 마지막으로 결정/세팅 한 후 출력한다. (ex : 색상)
	}
    
    pass HPBarGauge // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend_Add, float4(1.f, 1.f, 1.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_HPBAR_GAUGE();
    }

    pass HPBarGauge_Lerp // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend_Add, float4(1.f, 1.f, 1.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_HPBAR_GAUGE_LERP();
    }

    pass IncreasingProgress // 3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_INCREASING_PROGRESS();
    }

}
