// #include "CustomPxErrorCallback.h"
// 
// void CustomPxErrorCallback::reportError(PxErrorCode::Enum code, const char* message, const char* file, int line)
// {
// 	const char* errorCode = NULL;
// 
// 	switch (code)
// 	{
// 	case PxErrorCode::eNO_ERROR:
// 		errorCode = "no error";
// 		break;
// 	case PxErrorCode::eINVALID_PARAMETER:
// 		errorCode = "invalid parameter";
// 		break;
// 	case PxErrorCode::eINVALID_OPERATION:
// 		errorCode = "invalid operation";
// 		break;
// 	case PxErrorCode::eOUT_OF_MEMORY:
// 		errorCode = "out of memory";
// 		break;
// 	case PxErrorCode::eDEBUG_INFO:
// 		errorCode = "info";
// 		break;
// 	case PxErrorCode::eDEBUG_WARNING:
// 		errorCode = "warning";
// 		break;
// 	case PxErrorCode::ePERF_WARNING:
// 		errorCode = "performance warning";
// 		break;
// 	case PxErrorCode::eABORT:
// 		errorCode = "abort";
// 		break;
// 	case PxErrorCode::eINTERNAL_ERROR:
// 		errorCode = "internal error";
// 		break;
// 	case PxErrorCode::eMASK_ALL:
// 		errorCode = "unknown error";
// 		break;
// 	}
// 
// 	PX_ASSERT(errorCode);
// 	if (errorCode)
// 	{
// 		char buffer[1024];
// 		sprintf_s(buffer, "%s (%d) : %s : %s\n", file, line, errorCode, message);
// 
// 		// in debug builds halt execution for abort codes
// 		PX_ASSERT(e != PxErrorCode::eABORT);
// 
// 		cout << buffer << endl;
// 
// 		// in release builds we also want to halt execution 
// 		// and make sure that the error message is flushed  
// 		while (code == PxErrorCode::eABORT)
// 		{
// 			cout << buffer << endl;
// 		}
// 	}
// }
