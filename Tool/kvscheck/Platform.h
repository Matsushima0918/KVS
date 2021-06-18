/*****************************************************************************/
/**
 *  @file   Platform.h
 *  @author Naohisa Sakamoto
 */
/*****************************************************************************/
#pragma once
#include <string>
#include <iostream>
#include <kvs/Program>
#include <kvs/Platform>
#include <kvs/SystemInformation>
#include <kvs/Endian>


namespace kvscheck
{

/*===========================================================================*/
/**
 *  @brief  Platform checker class.
 */
/*===========================================================================*/
class Platform : public kvs::Program
{
    int exec()
    {
        const std::string name = kvs::Platform::Name();
        const std::string cpu = kvs::Platform::CPUName();
        const std::string byte_order = kvs::Endian::IsBig() ? "Big endian" : "Little endian";
        const size_t ncores = kvs::SystemInformation::NumberOfProcessors();

#if defined( KVS_PLATFORM_CPU_64 )
        const size_t nbits = 64;
#elif defined( KVS_PLATFORM_CPU_32 )
        const size_t nbits = 32;
#else
        const size_t nbits = 0;
#endif

        std::cout << "Platform   : " << name << std::endl;
        std::cout << "CPU        : " << cpu << " (" << nbits << " bits)" << std::endl;
        std::cout << "# of cores : " << ncores << std::endl;
        std::cout << "Byte-order : " << byte_order << std::endl;

        return 0;
    }
};

} // end of namespace kvscheck
