
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
};
