#pragma once

#if defined(DEBUG) || defined(_DEBUG)
#define DEBUGMODE
#endif

#define noxnd noexcept(!_DEBUG)