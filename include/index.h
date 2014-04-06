/******************************************************************************
 * The MIT License (MIT)
 *
 * Copyright (c) 2013 UCAPA Team and other contributors
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
#ifndef INDEX_H
#define INDEX_H

 
/*!
 * @mainpage
 *
 * @tableofcontents
 *
 *
 * <center>Documentation for UCAPA 1.0<br>
 * UCAPA means @c "Unofficial C++ API for Parrot ARDrone 2.0".<br>
 * It is a cross-platform API for the Parrot ARDrone 2.0.<br>
 * For developers, UCAPA is available under MIT licence.<br></center>
 *
 * @image html splash.png
 *
 *
 * @section getStarted Getting Started
 *
 * @subsection compileAPI Compiling the API
 *
 * The API is developped using C++11, so you need to compile it with a compliant
 * compiler. For example, we have used MinGW/GCC 4.8. More generally, you will
 * be able to use GCC >= 4.8, Clang >= 3.3 and VS >= 2013 (older compilers might
 * work but are not officially supported). Note: In the case of GCC and Clang,
 * you need to toggle an option to enable the C++11 support (normally
 * @c -c++11)<br>
 * Required dependencies are ASIO, in its standalone version (not Boost version),
 * and FFMPEG.<br>
 * You can configure a project to build UCAPA using your favorite IDE. It can be
 * generated using cmake (>= 2.8.9). You will find more information about how
 * to compile in the @c COMPILE file in the root directory of the project.
 *
 *
 * @subsection startCoding Start coding
 *
 * To test if everything is working fine, you can compile and run examples 
 * available in the examples/ subdirectory of UCAPA.<br>
 * Then you can start coding your own app. For this, the ARDrone class is the main
 * class to act on the drone. Others classes are only used, directly or indirectly, 
 * by this class. So you just need to create an ARDrone object and use it to do 
 * many awesome things with the drone.<br>
 * However, for your app, if you want to add a really specific behaviour not
 * already handled by the API, you have to know that you can derivate any of the 4
 * drone-specific classes to extend them. One of the examples will show you how to
 * do this.
 *
 */


#endif // INDEX_H
