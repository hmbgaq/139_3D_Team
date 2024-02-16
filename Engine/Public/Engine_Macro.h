#ifndef Engine_Macro_h__
#define Engine_Macro_h__


/* Color */
#define D3DCOLOR_ARGB(a,r,g,b)    ((D3DCOLOR)((((a)&0xff)<<24)|(((b)&0xff)<<16)|(((g)&0xff)<<8)|((r)&0xff)))

#define DEBUG_RGBA_RED					_float4(1.f, 0.f, 0.f, 1.f)
#define DEBUG_RGBA_DARK_RED				_float4(0.5f, 0.0f, 0.0f, 1.0f)
#define DEBUG_RGBA_ORANGE				_float4(1.f, 0.5f, 0.f, 1.f)
#define DEBUG_RGBA_LIGHT_ORANGE			_float4(1.0f, 0.7f, 0.4f, 1.0f) 
#define DEBUG_RGBA_PASTEL_ORANGE		_float4(1.0f, 0.8f, 0.6f, 1.0f) 
#define DEBUG_RGBA_DARK_ORANGE			_float4(0.8f, 0.4f, 0.0f, 1.0f)
#define DEBUG_RGBA_YELLOW				_float4(1.f, 1.f, 0.f, 1.f)
#define DEBUG_RGBA_LIGHT_YELLOW			_float4(1.0f, 1.0f, 0.5f, 1.0f) 
#define DEBUG_RGBA_PASTEL_YELLOW		_float4(1.0f, 1.0f, 0.7f, 1.0f) 
#define DEBUG_RGBA_GREEN				_float4(0.f, 1.f, 0.f, 1.f)
#define DEBUG_RGBA_BLUE					_float4(0.f, 0.f, 1.f, 1.f)
#define DEBUG_RGBA_CYAN					_float4(0.f, 1.f, 1.f, 1.f)
#define DEBUG_RGBA_PURPLE				_float4(0.5f, 0.0f, 0.5f, 1.0f)
#define DEBUG_RGBA_MAGENTA				_float4(1.f, 0.f, 1.f, 1.f)
#define DEBUG_RGBA_WHITE				_float4(1.f, 1.f, 1.f, 1.f)
#define DEBUG_RGBA_BLACK				_float4(0.f, 0.f, 0.f, 1.f)
#define DEBUG_RGBA_GRAY					_float4(0.5f, 0.5f, 0.5f, 1.f)
#define DEBUG_RGBA_BLUEGRAY				_float4(0.2f, 0.4f, 0.6f, 1.f)
#define DEBUG_RGBA_BLUEGREEN			_float4(0.1f, 0.5f, 0.4f, 1.f)
#define DEBUG_RGBA_TURQUOISE			_float4(0.1f, 0.6f, 0.6f, 1.f)
#define DEBUG_RGBA_TEAL					_float4(0.0f, 0.4f, 0.4f, 1.f)

/* MsgBox*/
#ifndef	MSG_BOX
#define	MSG_BOX(_message)		MessageBox(nullptr, TEXT(_message), L"System Message", MB_OK)
#endif

/* Namespace*/
#define	BEGIN(NAMESPACE)		namespace NAMESPACE {
#define	END						}
#define	USING(NAMESPACE)		using namespace NAMESPACE;

/* Build Option*/
#ifdef	ENGINE_EXPORTS
#define ENGINE_DLL		_declspec(dllexport)
#else
#define ENGINE_DLL		_declspec(dllimport)
#endif

/*  Debug */
#ifdef _DEBUG
#define DEBUG_BREAK  DebugBreak()
#else
#define DEBUG_BREAK
#endif

#ifdef _DEBUG
#define DEBUG_ASSERT assert(false)
#endif

/* ================== NULL CHECK ================== */
#define NULL_CHECK( _ptr)	\
	{if( 0 == (_ptr)){ DEBUG_BREAK;return;}}

#define NOT_NULL_CHECK( _ptr)	\
	{if( 0 != (_ptr)){ DEBUG_BREAK;return;}}

#define NULL_CHECK_RETURN( _ptr, _return)	\
	{if( 0 == (_ptr) ){ DEBUG_BREAK;return _return;}}

#define NOT_NULL_CHECK_RETURN( _ptr, _return)	\
	{if( 0 != (_ptr) ){ DEBUG_BREAK;return _return;}}

#define NULL_CHECK_MSG( _ptr, _message )		\
	{if( (_ptr) == 0){MessageBox(NULL, _message, L"System Message",MB_OK); DEBUG_BREAK;}}

#define NULL_CHECK_RETURN_MSG( _ptr, _return, _message )	\
	{if( (_ptr) == 0){MessageBox(NULL, _message, L"System Message",MB_OK); DEBUG_BREAK;return _return;}}

/* ================== FAILED CHECK ================== */
#define FAILED_CHECK(_hr)	if( ((HRESULT)(_hr)) < 0 )	\
	{ MessageBoxW(NULL, L"Failed", L"System Error",MB_OK); DEBUG_BREAK; return E_FAIL;}

#define FAILED_CHECK_RETURN(_hr, _return)	if( ((HRESULT)(_hr)) < 0 )		\
	{ MessageBoxW(NULL, L"Failed", L"System Error",MB_OK); DEBUG_BREAK; return _return;}

#define FAILED_CHECK_MSG( _hr, _message)	if( ((HRESULT)(_hr)) < 0 )	\
	{ MessageBoxW(NULL, _message, L"System Message",MB_OK); DEBUG_BREAK;}

#define FAILED_CHECK_RETURN_MSG( _hr, _return, _message)	if( ((HRESULT)(_hr)) < 0 )	\
	{ MessageBoxW(NULL, _message, L"System Message",MB_OK); DEBUG_BREAK;return _return;}

/* Singleton */
#define NO_COPY(CLASSNAME)								\
		private:										\
		CLASSNAME(const CLASSNAME&);					\
		CLASSNAME& operator = (const CLASSNAME&);		

#define DECLARE_SINGLETON(CLASSNAME)					\
		NO_COPY(CLASSNAME)								\
		private:										\
		static CLASSNAME*	m_pInstance;				\
		public:											\
		static CLASSNAME*	GetInstance( void );		\
		static unsigned long DestroyInstance( void );			

#define IMPLEMENT_SINGLETON(CLASSNAME)							\
		CLASSNAME*	CLASSNAME::m_pInstance = nullptr;			\
		CLASSNAME*	CLASSNAME::GetInstance( void )	{			\
			if(NULL == m_pInstance) {							\
				m_pInstance = new CLASSNAME;					\
			}													\
			return m_pInstance;									\
		}														\
		unsigned long CLASSNAME::DestroyInstance( void ) {		\
			unsigned long dwRefCnt = 0;							\
			if(nullptr != m_pInstance)							\
			{													\
				dwRefCnt = m_pInstance->Release();				\
				if(0 == dwRefCnt)								\
					m_pInstance = nullptr;						\
			}													\
			return dwRefCnt;									\
		}

/* SH_Add */
#define UIPATH(path) "../Bin/Resources/Textures/UI/Textures/" path
#define UIPATHPNG(path) "../Bin/Resources/Textures/UI/Textures/" path ".png"
#define UIPATHJPG(path) "../Bin/Resources/Textures/UI/Textures/" path ".jpg"
#define UIPATHDDS(path) "../Bin/Resources/Textures/UI/Textures/" path ".dds"
/* SH_End*/

#endif // Engine_Macro_h__
