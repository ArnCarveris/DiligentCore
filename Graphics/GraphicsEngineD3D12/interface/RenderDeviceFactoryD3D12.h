/*     Copyright 2015-2016 Egor Yusov
 *  
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF ANY PROPRIETARY RIGHTS.
 *
 *  In no event and under no legal theory, whether in tort (including negligence), 
 *  contract, or otherwise, unless required by applicable law (such as deliberate 
 *  and grossly negligent acts) or agreed to in writing, shall any Contributor be
 *  liable for any damages, including any direct, indirect, special, incidental, 
 *  or consequential damages of any character arising as a result of this License or 
 *  out of the use or inability to use the software (including but not limited to damages 
 *  for loss of goodwill, work stoppage, computer failure or malfunction, or any and 
 *  all other commercial damages or losses), even if such Contributor has been advised 
 *  of the possibility of such damages.
 */

#pragma once

#include "Errors.h"

extern "C"
{
//#ifdef ENGINE_DLL
//    typedef void (*CreateDeviceAndImmediateContextD3D12Type)( 
//        const Diligent::EngineCreationAttribs& CreationAttribs, 
//        Diligent::IRenderDevice **ppDevice, 
//        Diligent::IDeviceContext **ppContext );
//
//    typedef void (*CreateSwapChainD3D12Type)( 
//        Diligent::IRenderDevice *pDevice, 
//        Diligent::IDeviceContext *pImmediateContext, 
//        const Diligent::SwapChainDesc& SwapChainDesc, 
//        void* pNativeWndHandle, 
//        Diligent::ISwapChain **ppSwapChain );
//
//    static void LoadGraphicsEngineD3D12(CreateDeviceAndImmediateContextD3D12Type &CreateDeviceFunc, CreateSwapChainD3D12Type &CreateSwapChainFunc)
//    {
//        CreateDeviceFunc = nullptr;
//        CreateSwapChainFunc = nullptr;
//        std::string LibName = "GraphicsEngineD3D12_";
//
//#if _WIN64
//        LibName += "64";
//#else
//        LibName += "32";
//#endif
//
//#ifdef _DEBUG
//        LibName += "d";
//#else
//        LibName += "r";
//#endif
//
//        LibName += ".dll";
//        auto hModule = LoadLibraryA( LibName.c_str() );
//        if( hModule == NULL )
//        {
//            LOG_ERROR_MESSAGE( "Failed to load ", LibName, " library." );
//            return;
//        }
//
//        CreateDeviceFunc = reinterpret_cast<CreateDeviceAndImmediateContextD3D12Type>( GetProcAddress(hModule, "CreateDeviceAndImmediateContextD3D12") );
//        if( CreateDeviceFunc == NULL )
//        {
//            LOG_ERROR_MESSAGE( "Failed to load CreateDeviceAndImmediateContextD3D12() from ", LibName, " library." );
//            FreeLibrary( hModule );
//            return;
//        }
//
//        CreateSwapChainFunc = reinterpret_cast<CreateSwapChainD3D12Type>( GetProcAddress(hModule, "CreateSwapChainD3D12") );
//        if( CreateSwapChainFunc == NULL )
//        {
//            LOG_ERROR_MESSAGE( "Failed to load CreateSwapChainD3D12() from ", LibName, " library." );
//            FreeLibrary( hModule );
//            return;
//        }
//    }
//#else
    void CreateDeviceAndImmediateContextD3D12( const Diligent::EngineCreationAttribs& CreationAttribs, 
                                               Diligent::IRenderDevice **ppDevice, 
                                               Diligent::IDeviceContext **ppContext );
    void CreateSwapChainD3D12( Diligent::IRenderDevice *pDevice, 
                               Diligent::IDeviceContext *pImmediateContext, 
                               const Diligent::SwapChainDesc& SwapChainDesc, 
                               void* pNativeWndHandle, 
                               Diligent::ISwapChain **ppSwapChain );
//#endif
}
