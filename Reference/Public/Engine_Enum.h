#ifndef Engine_Enum_h__
#define Engine_Enum_h__

namespace Engine
{
	/* Input Device */
	enum MOUSEKEYSTATE { DIM_LB, DIM_RB, DIM_MB, DIM_END };
	enum MOUSEMOVESTATE { DIMS_X, DIMS_Y, DIMS_Z, DIMS_END };

	/* Transform */
	static const wchar_t* g_pTransformTag = TEXT("Com_Transform");

	/* Shader */
	enum class DEFERRED_SHADER { 
		DEBUG, 
		DIRECTIONAL_LIGHT, 
		POINT_LIGHT, 
		SPOT_LIGHT, 
		DEFERRED, 
		DEFERRED_UI, 
		SHADER_END };

	enum class SSAO_SHADER { 
		SSAO, 
		GODRAY , 
		SHADER_END};

	enum class BLUR_SHADER { 
		BLUR_DOWN, 
		BLUR_HORIZON_LOW,	BLUR_HORIZON_QUARTER,	BLUR_HORIZON_MIDDLE,	BLUR_HORIZON_HIGH,
		BLUR_VERTICAL_LOW,	BLUR_VERTICAL_QUARTER,	BLUR_VERTICAL_MIDDLE,	BLUR_VERTICAL_HIGH,
		BLUR_UP_ADD, BLUR_UP_MAX,
		BLUR_END 
		};

	enum MINMAX { MIN, MAX, MINMAX_END };

	enum AXIS { AXIS_X, AXIS_Y, AXIS_Z, AXIS_END };
}

#endif // Engine_Enum_h__
