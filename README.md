Bolt is a C++ template library optimized for heterogeneous computing. Bolt is designed to provide high-performance library implementations for common algorithms such as scan, reduce, transform, and sort. The Bolt interface was modeled on the C++ Standard Template Library (STL). Developers familiar with the STL will recognize many of the Bolt APIs and customization techniques.

The primary goal of Bolt is to make it easier for developers to utilize the inherent performance and power efficiency benefits of heterogeneous computing.  It has interfaces that are easy to use, and has comprehensive documentation for the library routines, memory management, control interfaces, and host/device code sharing.

Compared to writing the equivalent functionality in OpenCL™, you’ll find that Bolt requires significantly fewer lines-of-code and less developer effort.  Bolt is designed to provide a standard way to develop an application that can execute on either a regular CPU, or use any available OpenCL™ capable accelerated compute unit, with a single code path.

Here's a link to our <a href="https://github.com/HSA-Libraries/bolt/wiki">BOLT wiki page</a>.

<b>Prerequisites:</b>

1.  Windows® 7/8
2.  Visual Studio 2010 onwards (VS2012 for C++ AMP)
3.  CMake 2.8.10
4.  TBB (For Multicore CPU path, BOLT is tested with 4.1 Update 3)
5.  APP SDK 2.7 onwards

Currently BOLT is tested with AMD HD7970 TAHITI card and APU(Richland and Trinity).

*Note:* If the user has installed both Visual Studio 2012 and Visual Studio 2010, the latter should be updated to SP1.

<b>Examples:</b>

The simple example below shows how to use Bolt to sort a random array of 8192 integers.

    #include <bolt/cl/sort.h>
    #include <vector>
    #include <algorithm>
    void main ()
    {
        // generate random data (on host)
        size_t length = 8192
        std::vector<int> a (length);
        std::generate ( a.begin (), a.end(), rand );
    
        // sort, run on best device in the platform
        bolt::cl::sort(a.begin(), a.end());
    }

The code will be familiar to programmers who have used the C++ Standard Template Library; the difference is the include file (bolt/cl/sort.h) and the bolt::cl namespace before the sort call. Bolt developers do not need to learn a new device-specific programming model to leverage the power and performance advantages of heterogeneous computing.

    #include <bolt/cl/device_vector.h>
    #include <bolt/cl/scan.h>
    #include <vector>
    #include <numeric>
    
    void main()
    {
      size_t length = 1024;
      // Create device_vector and initialize it to 1
      bolt::cl::device_vector< int > boltInput( length, 1 );
    
      // Calculate the inclusive_scan of the device_vector
      bolt::cl::inclusive_scan(boltInput.begin(),boltInput.end(),boltInput.begin( ) );
    
      // Create an std vector and initialize it to 1
      std::vector<int> stdInput( length, 1 );
     
      // Calculate the inclusive_scan of the std vector
      bolt::cl::inclusive_scan(stdInput.begin( ),stdInput.end( ),stdInput.begin( ) );
    }

This example shows how Bolt simplifies management of heterogeneous memory.  The creation and destruction of device resident memory is abstracted inside of the `bolt::cl::device_vector <>` class, which provides an interface familiar to nearly all C++ programmers.  All of Bolt’s provided algorithms can take either the normal std::vector or the `bolt::cl::device_vector<>` class, which allows the user to control when and where memory is transferred between host and device to optimize performance.

<b>Copyright and Licensing information:</b>

Copyright 2012 - 2013 Advanced Micro Devices, Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0
       
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
