/* ---------------- Sampler ---------------- */
#define FLT_MAX 3.402823466e+38F

sampler LinearSampler = sampler_state
{
	filter = min_mag_mip_linear;
	AddressU = wrap;
	AddressV = wrap;
};

sampler PointSampler = sampler_state
{
	filter = min_mag_mip_Point;
	AddressU = wrap;
	AddressV = wrap;
};

sampler ClampSampler = sampler_state
{
    filter = min_mag_mip_linear;
    AddressU = CLAMP;
    AddressV = CLAMP;
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

/* ---------------- Rasterizer ---------------- */

RasterizerState RS_Default
{
	FillMode = Solid;	
	CullMode = Back;
	FrontCounterClockwise = false;
};

RasterizerState RS_Fill_Wireframe
{
	FillMode = Wireframe;
	CullMode = Back;
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
    CullMode = FRONT;
    FrontCounterClockwise = false;
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

	SrcBlend = SRC_ALPHA;
	DestBlend = Inv_Src_Alpha;
	BlendOp = add;
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
	/* 렌더타겟 블렌딩 동작중에 색상 및 알파 블렌딩중에 각각 두개의 값중 큰값ㅇ르 사용하여 렌더링한다. */ 
};
