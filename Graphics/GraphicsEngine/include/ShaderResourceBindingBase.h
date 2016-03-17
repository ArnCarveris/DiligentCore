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
/// Implementation of the Diligent::ShaderResourceBindingBase template class

#include "ShaderResourceBinding.h"
#include "ObjectBase.h"
#include "GraphicsTypes.h"
#include "RefCntAutoPtr.h"

namespace Diligent
{

/// Template class implementing base functionality for a shader resource binding

/// \tparam BaseInterface - base interface that this class will inheret 
///                         (Diligent::IShaderResourceBindingD3D11 or Diligent::IShaderResourceBindingGL).
template<class BaseInterface, class SRBAllocator>
class ShaderResourceBindingBase : public ObjectBase<BaseInterface, SRBAllocator>
{
public:
    typedef ObjectBase<BaseInterface, SRBAllocator> TObjectBase;

    ShaderResourceBindingBase( SRBAllocator &ObjAllocator, IPipelineState *pPSO, bool IsInternal = false ) :
        TObjectBase( IsInternal ? pPSO : nullptr, &ObjAllocator ),
        m_pPSO( pPSO ),
        m_spPSO( IsInternal ? nullptr : pPSO )
    {}

    IMPLEMENT_QUERY_INTERFACE_IN_PLACE( IID_ShaderResourceBinding, TObjectBase )

    virtual IPipelineState* GetPipelineState()override final
    {
        return m_pPSO;
    }

protected:

    /// Weak reference to PSO
    Diligent::RefCntAutoPtr<IPipelineState> m_spPSO;
    IPipelineState *m_pPSO;
};

}
