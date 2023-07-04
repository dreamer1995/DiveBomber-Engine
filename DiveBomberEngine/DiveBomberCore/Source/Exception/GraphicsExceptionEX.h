#pragma once
#include "GraphicsException.h"

namespace DiveBomber::DEException
{
#define GFX_THROW_NOINFO_NAMESPACE(hrcall) if( FAILED( hr = (hrcall) ) ) throw DEException::GraphicsHrException( __LINE__,__FILE__,hr )

#ifndef NDEBUG
#define GFX_EXCEPT_NAMESPACE(hr) DEException::GraphicsHrException( __LINE__,__FILE__,(hr),DEException::DxgiInfoManager::GetInstance()->GetMessages() )
#define GFX_THROW_INFO_NAMESPACE(hrcall) DEException::DxgiInfoManager::GetInstance()->Set(); if( FAILED( hr = (hrcall) ) ) throw GFX_EXCEPT_NAMESPACE(hr)
#else
#define GFX_THROW_INFO_NAMESPACE(hrcall) GFX_THROW_NOINFO_NAMESPACE(hrcall)
#endif
}