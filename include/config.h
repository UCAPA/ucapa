/******************************************************************************
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 UCAPA Team and other contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *****************************************************************************/

#ifndef CONFIG_H
#define CONFIG_H

#if defined(SHARED_BUILD)
    #if defined(_WIN32) || defined(WIN32)
        // Windows compilers need specific (and different) keywords for export and import
        #ifdef ucapa_EXPORTS
            #define UCAPA_API __declspec(dllexport)
        #else
            #define UCAPA_API __declspec(dllimport)
        #endif
        // For Visual C++ compilers, we also need to turn off this annoying C4251 warning
        #ifdef _MSC_VER
            #pragma warning(disable: 4251)
        #endif
    #else // Linux, FreeBSD, Mac OS X
        // GCC 4 has special keywords for showing/hidding symbols
        #if __GNUC__ >= 4
            #define UCAPA_API __attribute__ ((__visibility__ ("default")))
        #else
            #define UCAPA_API
        #endif
    #endif
#else
    // Static build doesn't need import/export macros
    #define UCAPA_API
    #define UCAPA_API
#endif

#endif // CONFIG_H
