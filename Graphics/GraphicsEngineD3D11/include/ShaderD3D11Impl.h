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

/// \file
/// Declaration of Diligent::ShaderD3D11Impl class

#include "ShaderD3D11.h"
#include "RenderDeviceD3D11.h"
#include "ShaderBase.h"
#include "ShaderD3DBase.h"
#include "ShaderResourceLayoutD3D11.h"
#include "ShaderResourceCacheD3D11.h"
#include "EngineD3D11Defines.h"

namespace Diligent
{

class FixedBlockMemoryAllocator;
class ResourceMapping;

/// Implementation of the Diligent::IShaderD3D11 interface
class ShaderD3D11Impl : public ShaderBase<IShaderD3D11, IRenderDeviceD3D11, FixedBlockMemoryAllocator>, public ShaderD3DBase
{
public:
    typedef ShaderBase<IShaderD3D11, IRenderDeviceD3D11, FixedBlockMemoryAllocator> TShaderBase;

    ShaderD3D11Impl(FixedBlockMemoryAllocator &ShaderObjAllocator, class RenderDeviceD3D11Impl *pRenderDeviceD3D11, const ShaderCreationAttribs &CreationAttribs);
    ~ShaderD3D11Impl();
    
    virtual void QueryInterface( const Diligent::INTERFACE_ID &IID, IObject **ppInterface )override final;

    virtual void BindResources( IResourceMapping* pResourceMapping, Uint32 Flags  )override final;
    
    virtual IShaderVariable* GetShaderVariable( const Char* Name )override final;

    virtual ID3D11DeviceChild* GetD3D11Shader()override final{ return m_pShader; }

    ID3DBlob* GetBytecode(){return m_pShaderByteCode;}

    ShaderResourceLayoutD3D11& GetStaticResourceLayout(){return m_StaticResLayout;}
    ShaderResourceLayoutD3D11& GetResourceLayout(){return m_ResourceLayout;}
    Uint32 GetShaderTypeIndex(){return m_ShaderTypeIndex;}

private:

    /// D3D11 shader
    CComPtr<ID3D11DeviceChild> m_pShader;
    
    ShaderResourceCacheD3D11 m_StaticResCache;
    ShaderResourceLayoutD3D11 m_ResourceLayout;
    ShaderResourceLayoutD3D11 m_StaticResLayout;
    Uint32 m_ShaderTypeIndex; // VS == 0, PS == 1, GS == 2, HS == 3, DS == 4, CS == 5
};

}
