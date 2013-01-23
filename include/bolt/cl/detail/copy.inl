/***************************************************************************                                                                                     
*   Copyright 2012 Advanced Micro Devices, Inc.                                     
*                                                                                    
*   Licensed under the Apache License, Version 2.0 (the "License");   
*   you may not use this file except in compliance with the License.                 
*   You may obtain a copy of the License at                                          
*                                                                                    
*       http://www.apache.org/licenses/LICENSE-2.0                      
*                                                                                    
*   Unless required by applicable law or agreed to in writing, software              
*   distributed under the License is distributed on an "AS IS" BASIS,              
*   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.         
*   See the License for the specific language governing permissions and              
*   limitations under the License.                                                   

***************************************************************************/                                                                                     

#if !defined( COPY_INL )
#define COPY_INL
#pragma once

#ifndef BURST
#define BURST 4
#endif

#include <boost/thread/once.hpp>
#include <boost/bind.hpp>
#include <type_traits> 

#include "bolt/cl/bolt.h"

// bumps dividend up (if needed) to be evenly divisible by divisor
// returns whether dividend changed
// makeDivisible(9,4) -> 12,true
// makeDivisible(9,3) -> 9, false
template< typename Type1, typename Type2 >
bool makeDivisible( Type1& dividend, Type2 divisor)
{
    size_t lowerBits = static_cast<size_t>( dividend & (divisor-1) );
    if( lowerBits )
    { // bump it up
        dividend &= ~lowerBits;
        dividend += divisor;
        return true;
    }
    else
    { // already evenly divisible
      return false;
    }
}

// bumps dividend up (if needed) to be evenly divisible by divisor
// returns whether dividend changed
// roundUpDivide(9,4,?)  -> 12,4,3,true
// roundUpDivide(10,2,?) -> 10,2,5,false
template< typename Type1, typename Type2, typename Type3 >
bool roundUpDivide( Type1& dividend, Type2 divisor, Type3& quotient)
{
    size_t lowerBits = static_cast<size_t>( dividend & (divisor-1) );
    if( lowerBits )
    { // bump it up
        dividend &= ~lowerBits;
        dividend += divisor;
        quotient = dividend / divisor;
        return true;
    }
    else
    { // already evenly divisible
      quotient = dividend / divisor;
      return false;
    }
}

namespace bolt {
    namespace cl {

        // user control
        template<typename InputIterator, typename OutputIterator> 
        OutputIterator copy(const bolt::cl::control &ctl,  InputIterator first, InputIterator last, OutputIterator result, 
            const std::string& user_code)
        {
            int n = static_cast<int>( std::distance( first, last ) );
            return detail::copy_detect_random_access( ctl, first, n, result, user_code, std::iterator_traits< InputIterator >::iterator_category( ) );
        }

        // default control
        template<typename InputIterator, typename OutputIterator> 
        OutputIterator copy( InputIterator first, InputIterator last, OutputIterator result, 
            const std::string& user_code)
        {
            int n = static_cast<int>( std::distance( first, last ) );
            return detail::copy_detect_random_access( control::getDefault(), first, n, result, user_code, std::iterator_traits< InputIterator >::iterator_category( ) );
        }

        // default control
        template<typename InputIterator, typename Size, typename OutputIterator> 
        OutputIterator copy_n(InputIterator first, Size n, OutputIterator result, 
            const std::string& user_code)
        {
            return detail::copy_detect_random_access( control::getDefault(), first, n, result, user_code, std::iterator_traits< InputIterator >::iterator_category( ) );
        }

        // user control
        template<typename InputIterator, typename Size, typename OutputIterator> 
        OutputIterator copy_n(const bolt::cl::control &ctl, InputIterator first, Size n, OutputIterator result, 
            const std::string& user_code)
        {
            return detail::copy_detect_random_access( ctl, first, n, result, user_code, std::iterator_traits< InputIterator >::iterator_category( ) );
        }


    }//end of cl namespace
};//end of bolt namespace


namespace bolt {
namespace cl {
namespace detail {

        enum copyTypeName { copy_iType, copy_oType };

/***********************************************************************************************************************
 * Kernel Template Specializer
 **********************************************************************************************************************/
class Copy_KernelTemplateSpecializer : public KernelTemplateSpecializer
{
    public:

    Copy_KernelTemplateSpecializer() : KernelTemplateSpecializer()
    {
        addKernelName( "copyBoundsCheck"   );
        addKernelName( "copyNoBoundsCheck" );
        addKernelName( "copyA"             );
        addKernelName( "copyB"             );
        addKernelName( "copyC"             );
        addKernelName( "copyD"             );
    }
    
    const ::std::string operator() ( const ::std::vector<::std::string>& typeNames ) const
    {
        const std::string templateSpecializationString = 
            "// Dynamic specialization of generic template definition, using user supplied types\n"
            "template __attribute__((mangled_name(" + name(0) + "Instantiated)))\n"
            "__attribute__((reqd_work_group_size(256,1,1)))\n"
            "__kernel void " + name(0) + "(\n"
            "global " + typeNames[copy_iType] + " *src,\n"
            "global " + typeNames[copy_oType] + " *dst,\n"
            "const uint length\n"
            ");\n\n"


            "// Dynamic specialization of generic template definition, using user supplied types\n"
            "template __attribute__((mangled_name(" + name(1) + "Instantiated)))\n"
            "__attribute__((reqd_work_group_size(256,1,1)))\n"
            "__kernel void " + name(1) + "(\n"
            "global " + typeNames[copy_iType] + " *src,\n"
            "global " + typeNames[copy_oType] + " *dst,\n"
            "const uint length\n"
            ");\n\n"

            "// Dynamic specialization of generic template definition, using user supplied types\n"
            "template __attribute__((mangled_name(" + name(2) + "Instantiated)))\n"
            "__attribute__((reqd_work_group_size(256,1,1)))\n"
            "__kernel void " + name(2) + "(\n"
            "global " + typeNames[copy_iType] + " *src,\n"
            "global " + typeNames[copy_oType] + " *dst,\n"
            "const uint numElements\n"
            ");\n\n"

            "// Dynamic specialization of generic template definition, using user supplied types\n"
            "template __attribute__((mangled_name(" + name(3) + "Instantiated)))\n"
            "__attribute__((reqd_work_group_size(256,1,1)))\n"
            "__kernel void " + name(3) + "(\n"
            "global " + typeNames[copy_iType] + " *src,\n"
            "global " + typeNames[copy_oType] + " *dst,\n"
            "const uint numElements\n"
            ");\n\n"

            "// Dynamic specialization of generic template definition, using user supplied types\n"
            "template __attribute__((mangled_name(" + name(4) + "Instantiated)))\n"
            "__attribute__((reqd_work_group_size(256,1,1)))\n"
            "__kernel void " + name(4) + "(\n"
            "global " + typeNames[copy_iType] + " *src,\n"
            "global " + typeNames[copy_oType] + " *dst,\n"
            "const uint numElements\n"
            ");\n\n"
            ;
    
        return templateSpecializationString;
    }
};



// Wrapper that uses default control class, iterator interface
template<typename InputIterator, typename Size, typename OutputIterator> 
OutputIterator copy_detect_random_access( const bolt::cl::control& ctl, const InputIterator& first, const Size& n, 
    const OutputIterator& result, const std::string& user_code, std::input_iterator_tag )
{
    static_assert( false, "Bolt only supports random access iterator types" );
    return NULL;
};

template<typename InputIterator, typename Size, typename OutputIterator> 
OutputIterator copy_detect_random_access( const bolt::cl::control& ctl, const InputIterator& first, const Size& n, 
    const OutputIterator& result, const std::string& user_code, std::random_access_iterator_tag )
{
    if (n > 0)
    {
        copy_pick_iterator( ctl, first, n, result, user_code );
    }
    return (result+n);
};

/*! \brief This template function overload is used to seperate device_vector iterators from all other iterators
    \detail This template is called by the non-detail versions of inclusive_scan, it already assumes random access
 *  iterators.  This overload is called strictly for non-device_vector iterators
*/
template<typename InputIterator, typename Size, typename OutputIterator> 
typename std::enable_if< 
             !(std::is_base_of<typename device_vector<typename std::iterator_traits<InputIterator>::value_type>::iterator,InputIterator>::value &&
               std::is_base_of<typename device_vector<typename std::iterator_traits<OutputIterator>::value_type>::iterator,OutputIterator>::value),
         void >::type
copy_pick_iterator(const bolt::cl::control &ctl,  const InputIterator& first, const Size& n, 
        const OutputIterator& result, const std::string& user_code)
{
    typedef std::iterator_traits<InputIterator>::value_type iType;
    typedef std::iterator_traits<OutputIterator>::value_type oType;

    // Use host pointers memory since these arrays are only read once - no benefit to copying.

    // Map the input iterator to a device_vector
    device_vector< iType >  dvInput( first,  n, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, true, ctl );

    // Map the output iterator to a device_vector
    device_vector< oType > dvOutput( result, n, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY, false, ctl );

    copy_enqueue( ctl, dvInput.begin( ), n, dvOutput.begin( ), user_code );

    // This should immediately map/unmap the buffer
    dvOutput.data( );
}

// This template is called by the non-detail versions of inclusive_scan, it already assumes random access iterators
// This is called strictly for iterators that are derived from device_vector< T >::iterator
template<typename DVInputIterator, typename Size, typename DVOutputIterator> 
typename std::enable_if< 
              (std::is_base_of<typename device_vector<typename std::iterator_traits<DVInputIterator>::value_type>::iterator,DVInputIterator>::value &&
               std::is_base_of<typename device_vector<typename std::iterator_traits<DVOutputIterator>::value_type>::iterator,DVOutputIterator>::value),
         void >::type
copy_pick_iterator(const bolt::cl::control &ctl,  const DVInputIterator& first, const Size& n,
    const DVOutputIterator& result, const std::string& user_code)
{
    copy_enqueue( ctl, first, n, result, user_code );
}

template< typename DVInputIterator, typename Size, typename DVOutputIterator > 
void copy_enqueue(const bolt::cl::control &ctl, const DVInputIterator& first, const Size& n, 
    const DVOutputIterator& result, const std::string& cl_code)
{
    /**********************************************************************************
     * Type Names - used in KernelTemplateSpecializer
     *********************************************************************************/
    typedef std::iterator_traits<DVInputIterator>::value_type iType;
    typedef std::iterator_traits<DVOutputIterator>::value_type oType;
    std::vector<std::string> typeNames(2);
    typeNames[copy_iType] = TypeName< iType >::get( );
    typeNames[copy_oType] = TypeName< oType >::get( );

    /**********************************************************************************
     * Type Definitions - directly concatenated into kernel string (order may matter)
     *********************************************************************************/
    std::vector<std::string> typeDefs;
    PUSH_BACK_UNIQUE( typeDefs, ClCode< iType >::get() )
    PUSH_BACK_UNIQUE( typeDefs, ClCode< oType >::get() )

    /**********************************************************************************
     * Compile Options
     *********************************************************************************/
    std::string compileOptions;
    std::ostringstream oss;
    oss << " -DBURST=" << BURST;
    compileOptions = oss.str();

    /**********************************************************************************
     * Request Compiled Kernels
     *********************************************************************************/
    Copy_KernelTemplateSpecializer c_kts;
    std::vector< ::cl::Kernel > kernels = bolt::cl::getKernels(
        ctl,
        typeNames,
        &c_kts,
        typeDefs,
        copy_kernels,
        compileOptions);

    const size_t workGroupSize  = 256; //kernelWithBoundsCheck.getWorkGroupInfo< CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE >( ctl.device( ), &l_Error );
    const size_t numComputeUnits = 40; //ctl.device( ).getInfo< CL_DEVICE_MAX_COMPUTE_UNITS >( ); // = 28
    const size_t numWorkGroupsPerComputeUnit = 10; //ctl.wgPerComputeUnit( );
    const size_t numWorkGroups = numComputeUnits * numWorkGroupsPerComputeUnit;
    
    const cl_uint numThreadsIdeal = static_cast<cl_uint>( numWorkGroups * workGroupSize );
    cl_uint numElementsPerThread = n / numThreadsIdeal;
    //if (numElementsPerThread*numThreads < n)
    //{
    //    std::cout << "not even num elements per thread" << std::endl;
    //    numElementsPerThread++;
    //}
    // bool roundedUp = makeDivisible( numElementsPerThread
    //  Ceiling function to bump the size of input to the next whole wavefront size
    // unsigned int kernelWgSize = 256;
    cl_uint numThreadsRUP = n;
    size_t mod = (n & (workGroupSize-1));
    if( mod )
    {
        numThreadsRUP &= ~mod;
        numThreadsRUP += workGroupSize;
    }
    //cl_uint numWorkGroups = static_cast< cl_uint >( numThreads / kernelWgSize );
    
    /**********************************************************************************
     *  Kernel
     *********************************************************************************/
    ::cl::Event kernelEvent;
    cl_int l_Error;
    try
    {
        int whichKernel = 2;
        cl_uint numThreadsChosen;
        cl_uint workGroupSizeChosen = workGroupSize;
        switch( whichKernel )
        {
        case 0: // 1 thread per element, even
        case 1: // 1 thread per element, boundary check
            numThreadsChosen = numThreadsRUP;
            V_OPENCL( kernels[whichKernel].setArg( 0, first->getBuffer()), "Error setArg kernels[ 0 ]" ); // Input keys
            V_OPENCL( kernels[whichKernel].setArg( 1, result->getBuffer()),"Error setArg kernels[ 0 ]" ); // Input buffer
            V_OPENCL( kernels[whichKernel].setArg( 2, static_cast<cl_uint>( n ) ),                 "Error setArg kernels[ 0 ]" ); // Size of buffer
            break;
        case 2: // A: ideal threads, loop
        case 3: // B: 
            numThreadsChosen = numThreadsIdeal;
            V_OPENCL( kernels[whichKernel].setArg( 0, first->getBuffer()), "Error setArg kernels[ 0 ]" ); // Input keys
            V_OPENCL( kernels[whichKernel].setArg( 1, result->getBuffer()),"Error setArg kernels[ 0 ]" ); // Input buffer
            V_OPENCL( kernels[whichKernel].setArg( 2, static_cast<cl_uint>(n) ),                 "Error setArg kernels[ 0 ]" ); // Size of buffer
            break;
            
        case 4: // C: 
        case 5: // D:
            numThreadsChosen = numThreadsRUP / BURST;
            V_OPENCL( kernels[whichKernel].setArg( 0, first->getBuffer()), "Error setArg kernels[ 0 ]" ); // Input keys
            V_OPENCL( kernels[whichKernel].setArg( 1, result->getBuffer()),"Error setArg kernels[ 0 ]" ); // Input buffer
            V_OPENCL( kernels[whichKernel].setArg( 2, static_cast<cl_uint>(n) ),                 "Error setArg kernels[ 0 ]" ); // Size of buffer
        } // switch

        l_Error = ctl.commandQueue( ).enqueueNDRangeKernel(
            kernels[whichKernel],
            ::cl::NullRange,
            ::cl::NDRange( numThreadsChosen ),
            ::cl::NDRange( workGroupSizeChosen ),
            NULL,
            &kernelEvent);
        V_OPENCL( l_Error, "enqueueNDRangeKernel() failed for kernel" );
    }
    catch( const ::cl::Error& e)
    {
        std::cerr << "::cl::enqueueNDRangeKernel( ) in bolt::cl::copy_enqueue()" << std::endl;
        std::cerr << "Error Code:   " << clErrorStringA(e.err()) << " (" << e.err() << ")" << std::endl;
        std::cerr << "File:         " << __FILE__ << ", line " << __LINE__ << std::endl;
        std::cerr << "Error String: " << e.what() << std::endl;
    }

    // wait for results
    bolt::cl::wait(ctl, kernelEvent);

    if (0) {
        cl_ulong start_time, stop_time;
        
        l_Error = kernelEvent.getProfilingInfo<cl_ulong>(CL_PROFILING_COMMAND_START, &start_time);
        V_OPENCL( l_Error, "failed on getProfilingInfo<CL_PROFILING_COMMAND_QUEUED>()");
        l_Error = kernelEvent.getProfilingInfo<cl_ulong>(CL_PROFILING_COMMAND_END, &stop_time);
        V_OPENCL( l_Error, "failed on getProfilingInfo<CL_PROFILING_COMMAND_END>()");
        size_t time = stop_time - start_time;
        double gb = (n*(1.0*sizeof(iType)+sizeof(oType))/1024/1024/1024);
        double sec = time/1000000000.0;
        std::cout << "Global Memory Bandwidth: " << ( gb / sec) << " ( "
          << time/1000000.0 << " ms)" << std::endl;
    }
};
}//End OF detail namespace
}//End OF cl namespace
}//End OF bolt namespace

#endif