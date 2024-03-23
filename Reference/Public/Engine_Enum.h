#ifndef Engine_Enum_h__
#define Engine_Enum_h__

namespace Engine
{
	/* Input Device */
	enum MOUSEKEYSTATE { DIM_LB, DIM_RB, DIM_MB, DIM_END };
	enum MOUSEMOVESTATE { DIMS_X, DIMS_Y, DIMS_Z, DIMS_END };

	/* Transform */
	static const wchar_t* g_pTransformTag = TEXT("Com_Transform");
	
	/* RigidBody */
	static const wchar_t* g_pRigidBodyTag = TEXT("Com_RigidBody");


	/* Shader */
	enum class DEFERRED_SHADER { 
		DEBUG, 
		DIRECTIONAL_LIGHT, 
		POINT_LIGHT, 
		SPOT_LIGHT, 
		DEFERRED, 
		PBR, 
		MYPBR,
		SHADER_END };

	enum class SSAO_SHADER {
		SSAO,
		GODRAY,
		SHADER_END
	};

	enum class BLUR_SHADER {
		BLUR_DOWN,
		BLUR_HORIZON_LOW, BLUR_HORIZON_QUARTER, BLUR_HORIZON_MIDDLE, BLUR_HORIZON_HIGH,
		BLUR_VERTICAL_LOW, BLUR_VERTICAL_QUARTER, BLUR_VERTICAL_MIDDLE, BLUR_VERTICAL_HIGH,
		BLUR_UP_ADD, BLUR_UP_MAX,
		BLUR_END
	};

	enum class POST_SHADER {
		POST_ORIGIN,
		POST_HDR, POST_RADIAL, POST_DOF, 
		POST_EFFECT_MIX, POST_EFFECT_DISTORTION, 
		POST_END
	};

	enum class FINAL_SHADER {
		FINAL_DEFAULT, FINAL_HSV, FINAL_MIX, FINAL, FINAL_MIXEFFECT, FINAL_END
	};

	enum class EFFECT_SHADER {
		EFFECT_Default, EFFECT_Mask, EFFECT_END
	};

	enum class MINMAX { MIN, MAX, MINMAX_END };
	enum class AXIS { AXIS_X, AXIS_Y, AXIS_Z, AXIS_END };
	/*
	FORCE			:	질량 사용, 연속적인 힘을 가하는 경우 사용
						주로 바람이나 자기력과 같이 연속적으로 주어지는 힘

	IMPULSE			:	질량 사용, 타격이나 폭발과 같은 순간적인 힘
	
	ACCELERATION	:	질량 무시, 직접 연속적인 가속력 부여 (주로 지구의 중력 표현)

	VELOCITY_CHANGE :	질량 무시, 직접 순간적인 가속력 부여
						ACCELERATION은 시간이 흘러가면서 변화를 일으키는데 비해 이 모드는 순간적으로 지정한 속도로 변화를 일으킨다.
	*/
	enum class FORCE_MODE { FORCE, IMPULSE, ACCELERATION, VELOCITY_CHANGE, FORCE_MODE_END }; 


	enum class EASING_TYPE
	{
		LINEAR

		, QUAD_IN, QUAD_OUT, QUAD_INOUT
		, CUBIC_IN, CUBIC_OUT, CUBIC_INOUT
		, QUART_IN, QUART_OUT, QUART_INOUT
		, QUINT_IN, QUINT_OUT, QUINT_INOUT
		, SINE_IN, SINE_OUT, SINE_INOUT
		, EXPO_IN, EXPO_OUT, EXPO_INOUT
		, CIRC_IN, CIRC_OUT, CIRC_INOUT
		, ELASTIC_IN, ELASTIC_OUT, ELASTIC_INOUT
		, BOUNCE_IN, BOUNCE_OUT
		, OUT_BACK, INOUT_BACK

		, EASING_TYPE_END
	};



	/* Move */
	enum class MoveDirection
	{
		Front,
		FrontLeft,
		FrontRight,
		Back,
		BackLeft,
		BackRight,
		Left,
		Right,
	};

	/* Weapon */
	enum Direction
	{
		Left,
		Right,
		Front,
		Back,
	};

	enum Power
	{
		Light,
		Medium,
		Heavy,
		Absolute,
		Power_End
	};

	enum Hit_Type
	{
		None,
		Hit_Lightly,
		Hit,
		Hit_Break,
		Hit_Finish,

	};


	enum class PHYSXCOLLIDER_TYPE
	{
		SPHERE,
		BOX,
		CYLINDER,
		CONVEXMESH,
		MESH,
		MODEL,
		CONVEXMODEL,
		BOXWITHPOINTS,
		TYPE_END
	};

	enum class PHYSXACTOR_TYPE
	{
		DYNAMIC,
		YFIXED_DYNAMIC,
		STATIC,
		ZONE,
		TYPE_END
	};

	enum class ROOTNODE_FLAG
	{
		X = (1 << 0),
		Y = (1 << 1),
		Z = (1 << 2),
		FLAG_END
	};

}

#endif // Engine_Enum_h__
