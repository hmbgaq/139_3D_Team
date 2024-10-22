
#pragma warning(disable : 31) /* HLSL - Texture method 'Sample(sampler, float2)' 경고 */
#pragma warning(disable: 6146) /* PS에서 RTV에 바인딩안했을때 뜨는 경고 무시 */
/* ---------------- Sampler ---------------- */
#define FLT_MAX         3.402823466e+38F
#define F_ZERO          float3(0.04f, 0.04f, 0.04f)
#define PI              3.14159265359f
#define TWO_PI          6.28318530718f
#define PI_OVER_TWO     1.5707963268f
#define EPSILON         0.000001f

sampler LinearSampler = sampler_state
{
    filter = min_mag_mip_linear;
    AddressU = wrap;
    AddressV = wrap;
    AddressW = wrap;
};

sampler PointSampler = sampler_state
{
	filter = min_mag_mip_Point;
	AddressU = wrap;
    AddressV = wrap;
    AddressW = wrap;
};

sampler ClampSampler = sampler_state
{
    filter = min_mag_mip_linear;
    AddressU = CLAMP;
    AddressV = CLAMP;
    AddressW = CLAMP;
};

sampler PointClampSampler = sampler_state
{
    filter = min_mag_mip_Point;
    AddressU = CLAMP;
    AddressV = CLAMP;
    AddressW = CLAMP;
};

sampler SSAONormalDepth = sampler_state
{
    filter = min_mag_linear_mip_point; // D3D11_FILTER 0x14
    AddressU = Border; // 4
    AddressV = Border; // 4
    AddressW = Border; // 4

    BorderColor = float4(0.0f, 0.0f, 0.0f, 1e5f); // float [4]
    MinLOD = 0.0f; //_float
    MaxLOD = FLT_MAX; //_float
};

sampler SSAORandVec = sampler_state
{
    filter = MIN_MAG_LINEAR_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
};

sampler AnisotropicClampSampler = sampler_state /* For. 안티-앨리어싱 */
{
    Filter = ANISOTROPIC;
    MaxAnisotropy = 4;
    AddressU = clamp;
    AddressV = clamp;
    AddressW = clamp;

    ComparisonFunc = ALWAYS;
    MaxLOD = 0.f;
    MinLOD = 0.f;
};

sampler LinearBorderSampler = sampler_state
{
    filter = min_mag_mip_linear;
    AddressU = Border;
    AddressV = Border;
    AddressW = Border;
};

sampler CubeSampler = sampler_state
{
    filter = min_mag_mip_linear;
    AddressU = CLAMP;
    AddressV = CLAMP;
    AddressW = CLAMP;
    ComparisonFunc = NEVER;
};

sampler ShadowSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
};

sampler MIPSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
};
/* ---------------- Rasterizer ---------------- */

RasterizerState RS_Default
{
	FillMode = Solid;	
	CullMode = Back; /* 후면 컬링 */ 
	FrontCounterClockwise = false;
};

RasterizerState RS_Fill_Wireframe
{
	FillMode = Wireframe;
	CullMode = Back;
	FrontCounterClockwise = false;
};

RasterizerState RS_NoneCull_Wireframe
{
    FillMode = Wireframe;
    CullMode = None;
    FrontCounterClockwise = false;
};

RasterizerState RS_Cull_None
{
	FillMode = Solid;
	CullMode = None;
	FrontCounterClockwise = false;
};


RasterizerState RS_Cull_CW
{
    FillMode = Solid;
    CullMode = FRONT; /* 앞면 컬링 -> 후면 보여줌 */
    FrontCounterClockwise = false; /* 시계방향 */
};

/* ---------------- DepthStencil ---------------- */


DepthStencilState DSS_Default
{
	DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = less_equal;
};

DepthStencilState DSS_None
{
	DepthEnable = false;
	DepthWriteMask = zero;
};

DepthStencilState DSS_Enable
{
    DepthEnable = true;
    DepthWriteMask = zero;
};

DepthStencilState DSS_DepthStencilEnable
{
    DepthEnable = true;
    DepthWriteMask = all;
    DepthFunc = less_equal;

    StencilEnable = true;
    StencilReadMask = 0xff;
    StencilWriteMask = 0xff;

    FrontFaceStencilFunc = always;
    FrontFaceStencilPass = replace;
    FrontFaceStencilFail = keep;
};

/* ---------------- BlendState ---------------- */

BlendState BS_Default
{
	BlendEnable[0] = false;	
};


BlendState BS_AlphaBlend_Add
{
	BlendEnable[0] = true;	

	SrcBlend    = SRC_ALPHA;
    DestBlend   = INV_SRC_ALPHA;
    BlendOp     = Add;
};

BlendState BS_AlphaBlend_Effect
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;
    //BlendEnable[2] = true;
    //BlendEnable[3] = true;
    //BlendEnable[4] = true;
    //BlendEnable[5] = true;

    SrcBlend = Src_Alpha;
    DestBlend = Inv_Src_Alpha;
    BlendOp = Add;

    SrcBlendAlpha = Src_Alpha;
    DestBlendAlpha = Dest_Alpha;
    BlendOpAlpha = MAX;
};

BlendState BS_Blend_Add
{
	BlendEnable[0] = true;
	BlendEnable[1] = true;

	SrcBlend = one;
	DestBlend = one;
	BlendOp = add;
	/* Src색상과 대상 색상을 더할 때 Add연산을 사용하도록 설정한다. */ 
};

BlendState BS_Blend_Max
{
	BlendEnable[0] = true;
	BlendEnable[1] = true;

	SrcBlend = one;
	DestBlend = one;
	BlendOp = max; 
	/* 렌더타겟 블렌딩 동작중에 색상 및 알파 블렌딩중에 각각 두개의 값중 큰값을 사용하여 렌더링한다. */ 
};

BlendState AdditiveBlendState
{
    AlphaToCoverageEnable = false;
    BlendEnable[0] = true;
    SrcBlend[0] = ONE;
    DestBlend[0] = ONE;
    BlendOp[0] = ADD;
    RenderTargetWriteMask[0] = 15;
};

BlendState BS_AlphaBlending
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;
    BlendEnable[2] = true;

    SrcBlend = one;
    DestBlend = one;
    BlendOp = add;
};
/* -------------------------------------------- */

