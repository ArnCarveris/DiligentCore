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
/// Definition of the Diligent::IShaderResourceBinding interface and related data structures

#include "Object.h"

namespace Diligent
{

// {061F8774-9A09-48E8-8411-B5BD20560104}
static const Diligent::INTERFACE_ID IID_ShaderResourceBinding =
{ 0x61f8774, 0x9a09, 0x48e8, { 0x84, 0x11, 0xb5, 0xbd, 0x20, 0x56, 0x1, 0x4 } };


/// Shader resource binding interface

class IShaderResourceBinding : public IObject
{
public:
    /// Queries the specific interface, see IObject::QueryInterface() for details
    virtual void QueryInterface( const Diligent::INTERFACE_ID &IID, IObject **ppInterface ) = 0;

    /// Returns pointer to the referenced buffer object.

    /// The method calls AddRef() on the returned interface, 
    /// so Release() must be called to avoid memory leaks.
    virtual class IPipelineState* GetPipelineState() = 0;

    virtual void BindResources(Uint32 ShaderFlags, IResourceMapping *pResMapping, Uint32 Flags) = 0;

    virtual IShaderVariable *GetVariable(SHADER_TYPE ShaderType, const char *Name) = 0;
};

}
