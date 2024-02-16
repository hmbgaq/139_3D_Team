#ifndef Engine_Enum_h__
#define Engine_Enum_h__

namespace Engine
{
	/* Input Device */
	enum MOUSEKEYSTATE { DIM_LB, DIM_RB, DIM_MB, DIM_END };
	enum MOUSEMOVESTATE { DIMS_X, DIMS_Y, DIMS_Z, DIMS_END };

	/* Transform */
	static const wchar_t* g_pTransformTag = TEXT("Com_Transform");

	enum MINMAX { MIN, MAX, MINMAX_END };
	enum AXIS	{ AXIS_X, AXIS_Y, AXIS_Z, AXIS_END };

}

#endif // Engine_Enum_h__
