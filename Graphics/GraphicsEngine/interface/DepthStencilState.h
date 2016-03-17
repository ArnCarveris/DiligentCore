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
/// Depth-stencil state description

#include "BasicTypes.h"

namespace Diligent
{

/// Stencil operation

/// [D3D11_STENCIL_OP]: https://msdn.microsoft.com/en-us/library/windows/desktop/ff476219(v=vs.85).aspx
/// [D3D12_STENCIL_OP]: https://msdn.microsoft.com/en-us/library/windows/desktop/dn770409(v=vs.85).aspx
/// This enumeration describes the stencil operation and generally mirrows
/// [D3D11_STENCIL_OP][]/[D3D12_STENCIL_OP][] enumeration. 
/// It is used by Diligent::StencilOpDesc structure to describe the stencil fail, depth fail
/// and stencil pass operations
enum STENCIL_OP : Int32
{
    /// Undefined operation.
    STENCIL_OP_UNDEFINED = 0,

    /// Keep the existing stencil data.\n
    /// Direct3D counterpart: D3D11_STENCIL_OP_KEEP/D3D12_STENCIL_OP_KEEP. OpenGL counterpart: GL_KEEP.
    STENCIL_OP_KEEP      = 1,

    /// Set the stencil data to 0.\n
    /// Direct3D counterpart: D3D11_STENCIL_OP_ZERO/D3D12_STENCIL_OP_ZERO. OpenGL counterpart: GL_ZERO.
    STENCIL_OP_ZERO      = 2,

    /// Set the stencil data to the reference value set by calling IDeviceContext::SetStencilRef().\n
    /// Direct3D counterpart: D3D11_STENCIL_OP_REPLACE/D3D12_STENCIL_OP_REPLACE. OpenGL counterpart: GL_REPLACE.
    STENCIL_OP_REPLACE   = 3,
    
    /// Increment the current stencil value, and clamp to the maximum representable unsigned value.\n
    /// Direct3D counterpart: D3D11_STENCIL_OP_INCR_SAT/D3D12_STENCIL_OP_INCR_SAT. OpenGL counterpart: GL_INCR.
    STENCIL_OP_INCR_SAT  = 4,

    /// Decrement the current stencil value, and clamp to 0.\n
    /// Direct3D counterpart: D3D11_STENCIL_OP_DECR_SAT/D3D12_STENCIL_OP_DECR_SAT. OpenGL counterpart: GL_DECR.
    STENCIL_OP_DECR_SAT  = 5,

    /// Bitwise invert the current stencil buffer value.\n
    /// Direct3D counterpart: D3D11_STENCIL_OP_INVERT/D3D12_STENCIL_OP_INVERT. OpenGL counterpart: GL_INVERT.
    STENCIL_OP_INVERT    = 6,

    /// Increment the current stencil value, and wrap the value to zero when incrementing 
    /// the maximum representable unsigned value. \n
    /// Direct3D counterpart: D3D11_STENCIL_OP_INCR/D3D12_STENCIL_OP_INCR. OpenGL counterpart: GL_INCR_WRAP.
    STENCIL_OP_INCR_WRAP = 7,

    /// Decrement the current stencil value, and wrap the value to the maximum representable 
    /// unsigned value when decrementing a value of zero.\n
    /// Direct3D counterpart: D3D11_STENCIL_OP_DECR/D3D12_STENCIL_OP_DECR. OpenGL counterpart: GL_DECR_WRAP.
    STENCIL_OP_DECR_WRAP = 8,

    /// Helper value that stores the total number of stencil operations in the enumeration.
    STENCIL_OP_NUM_OPS
};

/// Describes stencil operations that are performed based on the results of depth test.

/// [D3D11_DEPTH_STENCILOP_DESC]: https://msdn.microsoft.com/en-us/library/windows/desktop/ff476109(v=vs.85).aspx
/// [D3D12_DEPTH_STENCILOP_DESC]: https://msdn.microsoft.com/en-us/library/windows/desktop/dn770355(v=vs.85).aspx
/// The structure generally mirrows [D3D11_DEPTH_STENCILOP_DESC][]/[D3D12_DEPTH_STENCILOP_DESC][] structure. 
/// It is used by Diligent::DepthStencilStateDesc structure to describe the stencil 
/// operations for the front and back facing polygons.
struct StencilOpDesc
{
    /// The stencil operation to perform when stencil testing fails.
    STENCIL_OP          StencilFailOp;

    /// The stencil operation to perform when stencil testing passes and depth testing fails.
    STENCIL_OP          StencilDepthFailOp;

    /// The stencil operation to perform when stencil testing and depth testing both pass.
    STENCIL_OP          StencilPassOp;

    /// A function that compares stencil data against existing stencil data. See 
    /// Diligent::COMPARISON_FUNCTION.
    COMPARISON_FUNCTION StencilFunc;

    /// Initializes the structure members with default values

    /// Default values:
    /// Member              | Default value
    /// --------------------|--------------
    /// StencilFailOp       | STENCIL_OP_KEEP
    /// StencilDepthFailOp  | STENCIL_OP_KEEP
    /// StencilPassOp       | STENCIL_OP_KEEP
    /// StencilFunc         | COMPARISON_FUNC_ALWAYS
    StencilOpDesc() : 
        StencilFailOp     ( STENCIL_OP_KEEP ),
        StencilDepthFailOp( STENCIL_OP_KEEP ),
        StencilPassOp     ( STENCIL_OP_KEEP ),
        StencilFunc       ( COMPARISON_FUNC_ALWAYS )
    {}

    /// Tests if two structures are equivalent

    /// \param [in] rhs - reference to the structure to perform comparison with
    /// \return 
    /// - True if all members of the two structures are equal.
    /// - False otherwise
    bool operator == (const StencilOpDesc& rhs)const
    {
        return StencilFailOp      == rhs.StencilFailOp      &&
               StencilDepthFailOp == rhs.StencilDepthFailOp &&
               StencilPassOp      == rhs.StencilPassOp      &&
               StencilFunc        == rhs.StencilFunc;
    }
};

/// Depth stencil state description

/// [D3D11_DEPTH_STENCIL_DESC]: https://msdn.microsoft.com/en-us/library/windows/desktop/ff476110(v=vs.85).aspx
/// This structure describes the depth stencil state which is used in a call to 
/// IRenderDevice::CreatePipelineState() and generally mirrows [D3D11_DEPTH_STENCIL_DESC][]
/// structure.
struct DepthStencilStateDesc
{
    /// Enable depth-stencil operations. When it is set to False, 
    /// depth test always passes, depth writes are disabled,
    /// and no stencil operations are performed
    Bool                DepthEnable;

    /// Enable or disable writes to a depth buffer
    Bool                DepthWriteEnable;

    /// A function that compares depth data against existing depth data. 
    /// See Diligent::COMPARISON_FUNCTION for details.
    COMPARISON_FUNCTION DepthFunc;

    /// Enable stencil opertaions.
    Bool                StencilEnable;
    
    /// Identify which bits of the depth-stencil buffer are accessed when reading stencil data.
    Uint8               StencilReadMask;
    
    /// Identify which bits of the depth-stencil buffer are accessed when writing stencil data.
    Uint8               StencilWriteMask;

    /// Identify stencil operations for the front-facing triangles, see Diligent::StencilOpDesc.
    StencilOpDesc      FrontFace;

    /// Identify stencil operations for the back-facing triangles, see Diligent::StencilOpDesc.
    StencilOpDesc      BackFace;

    /// Initializes the structure members with default values

    /// Default values:
    /// Member              | Default value
    /// --------------------|--------------
    /// DepthEnable         | True
    /// DepthWriteEnable    | True
    /// DepthFunc           | COMPARISON_FUNC_LESS
    /// StencilEnable       | False
    /// StencilReadMask     | 0xFF
    /// StencilWriteMask    | 0xFF
    /// 
    /// Members of FrontFace and BackFace
    /// are initialized by StencilOpDesc::StencilOpDesc()
    DepthStencilStateDesc() : 
        DepthEnable     ( True ),
        DepthWriteEnable( True ),
        DepthFunc       ( COMPARISON_FUNC_LESS ),
        StencilEnable   ( False ),
        StencilReadMask ( 0xFF ),
        StencilWriteMask( 0xFF )
    {}

    /// Tests if two structures are equivalent

    /// \param [in] rhs - reference to the structure to perform comparison with
    /// \return 
    /// - True if all members of the two structures are equal.
    /// - False otherwise
    bool operator == (const DepthStencilStateDesc& rhs)const
    {
        return  DepthEnable      == rhs.DepthEnable      &&
                DepthWriteEnable == rhs.DepthWriteEnable &&
                DepthFunc        == rhs.DepthFunc        &&
                StencilEnable    == rhs.StencilEnable    &&
                StencilReadMask  == rhs.StencilReadMask  &&
                StencilWriteMask == rhs.StencilWriteMask &&
                FrontFace        == rhs.FrontFace        &&
                BackFace         == rhs.BackFace;     
    }
};

}
