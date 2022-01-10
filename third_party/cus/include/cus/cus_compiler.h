#ifndef CUS_EXPORT_H
#define CUS_EXPORT_H

#if defined (_MSC_VER)
#define CUS_COMPILER_MSVC

#if (_MSC_VER < 1200) // Visual C++ 6.0
#define CUS_MSVS_VERSION    1998
#define CUS_MSVC_VERSION    60
#elif (_MSC_VER >= 1200) && (_MSC_VER < 1300) // Visual Studio 2002, MSVC++ 7.0
#define CUS_MSVS_VERSION    2002
#define CUS_MSVC_VERSION    70
#elif (_MSC_VER >= 1300) && (_MSC_VER < 1400) // Visual Studio 2003, MSVC++ 7.1
#define CUS_MSVS_VERSION    2003
#define CUS_MSVC_VERSION    71
#elif (_MSC_VER >= 1400) && (_MSC_VER < 1500) // Visual Studio 2005, MSVC++ 8.0
#define CUS_MSVS_VERSION    2005
#define CUS_MSVC_VERSION    80
#elif (_MSC_VER >= 1500) && (_MSC_VER < 1600) // Visual Studio 2008, MSVC++ 9.0
#define CUS_MSVS_VERSION    2008
#define CUS_MSVC_VERSION    90
#elif (_MSC_VER >= 1600) && (_MSC_VER < 1700) // Visual Studio 2010, MSVC++ 10.0
#define CUS_MSVS_VERSION    2010
#define CUS_MSVC_VERSION    100
#elif (_MSC_VER >= 1700) && (_MSC_VER < 1800) // Visual Studio 2012, MSVC++ 11.0
#define CUS_MSVS_VERSION    2012
#define CUS_MSVC_VERSION    110
#elif (_MSC_VER >= 1800) && (_MSC_VER < 1900) // Visual Studio 2013, MSVC++ 12.0
#define CUS_MSVS_VERSION    2013
#define CUS_MSVC_VERSION    120
#elif (_MSC_VER >= 1900) && (_MSC_VER < 1910) // Visual Studio 2015, MSVC++ 14.0
#define CUS_MSVS_VERSION    2015
#define CUS_MSVC_VERSION    140
#elif (_MSC_VER >= 1910) && (_MSC_VER < 1920) // Visual Studio 2017, MSVC++ 15.0
#define CUS_MSVS_VERSION    2017
#define CUS_MSVC_VERSION    150
#elif (_MSC_VER >= 1920) && (_MSC_VER < 2000) // Visual Studio 2019, MSVC++ 16.0
#define CUS_MSVS_VERSION    2019
#define CUS_MSVC_VERSION    160
#endif

#elif defined(__MINGW32__) || defined(__MINGW64__)
#define CUS_COMPILER_MINGW

#elif defined(__clang__)
#define CUS_COMPILER_CLANG

#elif defined(__GNUC__)
#define CUS_COMPILER_GCC

#else
#error "unsupport compiler!"
#endif

#endif //CUS_EXPORT_H