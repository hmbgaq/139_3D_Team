#include "Shader_Defines.hlsli"

/* Base */
matrix      g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix      g_BoneMatrices[800];
float       g_fCamFar;
float       g_TimeDelta;

texture2D   g_DiffuseTexture;
Texture2D   g_NormalTexture;

/* Dissolve  */
Texture2D   g_DissolveTexture;
float       g_fDissolveWeight;

/* Masking */
Texture2D   g_MaskingTexture;

/* Bloom */
float4      g_BloomColor;

/* Reflection */
matrix      g_ReflectionMatrix;

float4      g_vRimColor = { 0.f, 0.f, 0.f, 0.f };

Texture2D   g_InstanceTransform;



float4x4 Get_AnimTexture_Transform(uint iIndex, uint iID)
{
    float4 vRight, vUp, vLook, vPos;

    vRight = g_InstanceTransform.Load(int3(iIndex, iID, 0));
    vUp = g_InstanceTransform.Load(int3(iIndex + 1, iID, 0));
    vLook = g_InstanceTransform.Load(int3(iIndex + 2, iID, 0));
    vPos = g_InstanceTransform.Load(int3(iIndex + 3, iID, 0));

    return matrix(vRight, vUp, vLook, vPos);
}

struct VS_IN
{
    float3  vPosition : POSITION;
    float3  vNormal : NORMAL;
    float2  vTexcoord : TEXCOORD0;
    float3  vTangent : TANGENT;
    uint4   vBlendIndices : BLENDINDEX;
    float4  vBlendWeights : BLENDWEIGHT;

    uint iID : INSTANCEID;
    matrix matWorld : INST_WORLD;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
	
    float3 vViewNormal : NORMAL1; /* ssao */
    float3 vPositionView : POSITION;
};



VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    

    In.vBlendIndices *= 4;

    float fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);
	
    float4x4 vMatX = Get_AnimTexture_Transform(In.vBlendIndices.x, In.iID);
    float4x4 vMatY = Get_AnimTexture_Transform(In.vBlendIndices.y, In.iID);
    float4x4 vMatZ = Get_AnimTexture_Transform(In.vBlendIndices.z, In.iID);
    float4x4 vMatW = Get_AnimTexture_Transform(In.vBlendIndices.w, In.iID);

    float4x4 BoneMatrix = vMatX * In.vBlendWeights.x +
		vMatY * In.vBlendWeights.y +
		vMatZ * In.vBlendWeights.z +
		vMatW * fWeightW;

    vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
    vector vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);

    Out.vPosition = mul(vPosition, matWVP);
    Out.vNormal = normalize(mul(vNormal, In.matWorld));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
    
	
    
    
    return Out;
}



struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    
	/* ssao */
    float3 vViewNormal : NORMAL1;
    float3 vPositionView : POSITION;
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vViewNormal : SV_TARGET3;
    float4 vBloom : SV_TARGET4;
};


/* ------------------- Base Pixel Shader (0) -------------------*/

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a < 0.1f)
        discard;

    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f); /* -1 ~ 1 -> 0 ~ 1 */
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.0f, 0.0f, 0.0f);
   // Out.vBloom = float4(1.0f, 0.f, 0.f, 1.0f);
    Out.vViewNormal = float4(1.0f, 1.0f, 1.0f, 1.0f);

    return Out;
}



/* ------------------- Shadow Pixel Shader(2) -------------------*/

struct PS_OUT_SHADOW
{
    vector vLightDepth : SV_TARGET0;
};

PS_OUT_SHADOW PS_MAIN_SHADOW(PS_IN In)
{
    PS_OUT_SHADOW Out = (PS_OUT_SHADOW) 0;

    Out.vLightDepth = In.vProjPos.w / g_fCamFar;
	
    return Out;
}

technique11 DefaultTechnique
{
    pass Instance // 0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();

    }

    pass Model_Wireframe // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend_Add, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Shadow // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SHADOW();
    }
}