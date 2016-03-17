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
/// Blend state description

#include "BasicTypes.h"

namespace Diligent
{

/// Blend factors

/// [D3D11_BLEND]: https://msdn.microsoft.com/en-us/library/windows/desktop/ff476086(v=vs.85).aspx
/// [D3D12_BLEND]: https://msdn.microsoft.com/en-us/library/windows/desktop/dn770338(v=vs.85).aspx
/// [glBlendFuncSeparate]: https://www.opengl.org/wiki/GLAPI/glBlendFuncSeparate
/// This enumeration defines blend factors for alpha-blending. 
/// It generatlly mirrows [D3D11_BLEND][] and [D3D12_BLEND][] enumerations and is used by RenderTargetBlendDesc structure
/// to define source and destination blend factors for color and alpha channels.
/// \sa [D3D11_BLEND on MSDN][D3D11_BLEND], [D3D12_BLEND on MSDN][D3D12_BLEND], [glBlendFuncSeparate on OpenGL.org][glBlendFuncSeparate]
enum BLEND_FACTOR : Int32
{
    /// Undefined blend factor
    BLEND_FACTOR_UNDEFINED = 0,

    /// The blend factor is zero.\n
    /// Direct3D counterpart: D3D11_BLEND_ZERO/D3D12_BLEND_ZERO. OpenGL counterpart: GL_ZERO.
    BLEND_FACTOR_ZERO,

    /// The blend factor is one.\n
    /// Direct3D counterpart: D3D11_BLEND_ONE/D3D12_BLEND_ONE. OpenGL counterpart: GL_ONE.
    BLEND_FACTOR_ONE,

    /// The blend factor is RGB data from a pixel shader.\n
    /// Direct3D counterpart: D3D11_BLEND_SRC_COLOR/D3D12_BLEND_SRC_COLOR. OpenGL counterpart: GL_SRC_COLOR.
    BLEND_FACTOR_SRC_COLOR,

    /// The blend factor is 1-RGB, where RGB is the data from a pixel shader.\n
    /// Direct3D counterpart: D3D11_BLEND_INV_SRC_COLOR/D3D12_BLEND_INV_SRC_COLOR. OpenGL counterpart: GL_ONE_MINUS_SRC_COLOR.
    BLEND_FACTOR_INV_SRC_COLOR,

    /// The blend factor is alpha (A) data from a pixel shader.\n
    /// Direct3D counterpart: D3D11_BLEND_SRC_ALPHA/D3D12_BLEND_SRC_ALPHA. OpenGL counterpart: GL_SRC_ALPHA.
    BLEND_FACTOR_SRC_ALPHA,

    /// The blend factor is 1-A, where A is alpha data from a pixel shader.\n
    /// Direct3D counterpart: D3D11_BLEND_INV_SRC_ALPHA/D3D12_BLEND_INV_SRC_ALPHA. OpenGL counterpart: GL_ONE_MINUS_SRC_ALPHA.
    BLEND_FACTOR_INV_SRC_ALPHA,

    /// The blend factor is alpha (A) data from a render target.\n
    /// Direct3D counterpart: D3D11_BLEND_DEST_ALPHA/D3D12_BLEND_DEST_ALPHA. OpenGL counterpart: GL_DST_ALPHA.
    BLEND_FACTOR_DEST_ALPHA,

    /// The blend factor is 1-A, where A is alpha data from a render target.\n
    /// Direct3D counterpart: D3D11_BLEND_INV_DEST_ALPHA/D3D12_BLEND_INV_DEST_ALPHA. OpenGL counterpart: GL_ONE_MINUS_DST_ALPHA.
    BLEND_FACTOR_INV_DEST_ALPHA,

    /// The blend factor is RGB data from a render target.\n
    /// Direct3D counterpart: D3D11_BLEND_DEST_COLOR/D3D12_BLEND_DEST_COLOR. OpenGL counterpart: GL_DST_COLOR.
    BLEND_FACTOR_DEST_COLOR,

    /// The blend factor is 1-RGB, where RGB is the data from a render target.\n
    /// Direct3D counterpart: D3D11_BLEND_INV_DEST_COLOR/D3D12_BLEND_INV_DEST_COLOR. OpenGL counterpart: GL_ONE_MINUS_DST_COLOR.
    BLEND_FACTOR_INV_DEST_COLOR,

    /// The blend factor is (f,f,f,1), where f = min(As, 1-Ad), 
    /// As is alpha data from a pixel shader, and Ad is alpha data from a render target.\n
    /// Direct3D counterpart: D3D11_BLEND_SRC_ALPHA_SAT/D3D12_BLEND_SRC_ALPHA_SAT. OpenGL counterpart: GL_SRC_ALPHA_SATURATE.
    BLEND_FACTOR_SRC_ALPHA_SAT,

    /// The blend factor is the constant blend factor set with IDeviceContext::SetBlendFactors().\n
    /// Direct3D counterpart: D3D11_BLEND_BLEND_FACTOR/D3D12_BLEND_BLEND_FACTOR. OpenGL counterpart: GL_CONSTANT_COLOR.
    BLEND_FACTOR_BLEND_FACTOR,

    /// The blend factor is one minus constant blend factor set with IDeviceContext::SetBlendFactors().\n
    /// Direct3D counterpart: D3D11_BLEND_INV_BLEND_FACTOR/D3D12_BLEND_INV_BLEND_FACTOR. OpenGL counterpart: GL_ONE_MINUS_CONSTANT_COLOR.
    BLEND_FACTOR_INV_BLEND_FACTOR,

    /// The blend factor is the second RGB data output from a pixel shader.\n
    /// Direct3D counterpart: D3D11_BLEND_SRC1_COLOR/D3D12_BLEND_SRC1_COLOR. OpenGL counterpart: GL_SRC1_COLOR.
    BLEND_FACTOR_SRC1_COLOR,

    /// The blend factor is 1-RGB, where RGB is the second RGB data output from a pixel shader.\n
    /// Direct3D counterpart: D3D11_BLEND_INV_SRC1_COLOR/D3D12_BLEND_INV_SRC1_COLOR. OpenGL counterpart: GL_ONE_MINUS_SRC1_COLOR.
    BLEND_FACTOR_INV_SRC1_COLOR,

    /// The blend factor is the second alpha (A) data output from a pixel shader.\n
    /// Direct3D counterpart: D3D11_BLEND_SRC1_ALPHA/D3D12_BLEND_SRC1_ALPHA. OpenGL counterpart: GL_SRC1_ALPHA.
    BLEND_FACTOR_SRC1_ALPHA,

    /// The blend factor is 1-A, where A is the second alpha data output from a pixel shader.\n
    /// Direct3D counterpart: D3D11_BLEND_INV_SRC1_ALPHA/D3D12_BLEND_INV_SRC1_ALPHA. OpenGL counterpart: GL_ONE_MINUS_SRC1_ALPHA.
    BLEND_FACTOR_INV_SRC1_ALPHA, 

    /// Helper value that stores the total number of blend factors in the enumeration.
    BLEND_FACTOR_NUM_FACTORS
};

/// Blending operation

/// [D3D11_BLEND_OP]: https://msdn.microsoft.com/en-us/library/windows/desktop/ff476088(v=vs.85).aspx
/// [D3D12_BLEND_OP]: https://msdn.microsoft.com/en-us/library/windows/desktop/dn770340(v=vs.85).aspx
/// [glBlendEquationSeparate]: https://www.opengl.org/wiki/GLAPI/glBlendEquationSeparate
/// This enumeration describes blending operation for RGB or Alpha channels and generally mirrows
/// [D3D11_BLEND_OP][] and [D3D12_BLEND_OP][] enums. It is used by RenderTargetBlendDesc structure to define RGB and Alpha 
/// blending operations
/// \sa [D3D11_BLEND_OP on MSDN][D3D11_BLEND_OP], [D3D12_BLEND_OP on MSDN][D3D12_BLEND_OP], [glBlendEquationSeparate on OpenGL.org][glBlendEquationSeparate]
enum BLEND_OPERATION : Int32
{
    /// Undefined blend operation
    BLEND_OPERATION_UNDEFINED = 0,
    
    /// Add source and destination color components.\n
    /// Direct3D counterpart: D3D11_BLEND_OP_ADD/D3D12_BLEND_OP_ADD. OpenGL counterpart: GL_FUNC_ADD.
    BLEND_OPERATION_ADD,

    /// Subtract destination color components from source color components.\n
    /// Direct3D counterpart: D3D11_BLEND_OP_SUBTRACT/D3D12_BLEND_OP_SUBTRACT. OpenGL counterpart: GL_FUNC_SUBTRACT.
    BLEND_OPERATION_SUBTRACT,

    /// Subtract source color components from destination color components.\n
    /// Direct3D counterpart: D3D11_BLEND_OP_REV_SUBTRACT/D3D12_BLEND_OP_REV_SUBTRACT. OpenGL counterpart: GL_FUNC_REVERSE_SUBTRACT.
    BLEND_OPERATION_REV_SUBTRACT,

    /// Compute the minimum of source and destination color components.\n
    /// Direct3D counterpart: D3D11_BLEND_OP_MIN/D3D12_BLEND_OP_MIN. OpenGL counterpart: GL_MIN.
    BLEND_OPERATION_MIN,

    /// Compute the maximum of source and destination color components.\n
    /// Direct3D counterpart: D3D11_BLEND_OP_MAX/D3D12_BLEND_OP_MAX. OpenGL counterpart: GL_MAX.
    BLEND_OPERATION_MAX,

    /// Helper value that stores the total number of blend operations in the enumeration.
    BLEND_OPERATION_NUM_OPERATIONS
};

/// Color component write flags

/// These flags are used by RenderTargetBlendDesc structure to define 
/// writable components of the render target
enum COLOR_MASK : Int32
{
    /// Allow data to be stored in the red component.
    COLOR_MASK_RED	    = 1,

    /// Allow data to be stored in the green component.
    COLOR_MASK_GREEN	= 2,

    /// Allow data to be stored in the blue component.
    COLOR_MASK_BLUE	    = 4,

    /// Allow data to be stored in the alpha component.
    COLOR_MASK_ALPHA	= 8,

    /// Allow data to be stored in all components.
    COLOR_MASK_ALL	= ( ( ( COLOR_MASK_RED | COLOR_MASK_GREEN )  | COLOR_MASK_BLUE )  | COLOR_MASK_ALPHA ) 
};

enum LOGIC_OPERATION
{
    LOGIC_OP_CLEAR = 0,
    LOGIC_OP_SET,
    LOGIC_OP_COPY,
    LOGIC_OP_COPY_INVERTED,
    LOGIC_OP_NOOP,
    LOGIC_OP_INVERT,
    LOGIC_OP_AND,
    LOGIC_OP_NAND,
    LOGIC_OP_OR,
    LOGIC_OP_NOR,
    LOGIC_OP_XOR,
    LOGIC_OP_EQUIV,
    LOGIC_OP_AND_REVERSE,
    LOGIC_OP_AND_INVERTED,
    LOGIC_OP_OR_REVERSE,
    LOGIC_OP_OR_INVERTED,
	LOGIC_OP_NUM_OPERATIONS
};

/// Describes a blend state for a single render target

/// This structure is used by BlendStateDesc to describe 
/// blend states for render targets
struct RenderTargetBlendDesc
{
    /// Enable or disable blending for this render target.
    Bool            BlendEnable;

    /// Enable or disable a logical operation for this render target.
	Bool			LogicOperationEnable;

    /// Specifies the blend factor to apply to the RGB value output from the pixel shader
    BLEND_FACTOR    SrcBlend;

    /// Specifies the blend factor to apply to the RGB value in the render target
    BLEND_FACTOR    DestBlend;

    /// Defines how to combine the source and destination RGB values
    /// after applying the SrcBlend and DestBlend factors.
    BLEND_OPERATION BlendOp;

    /// Specifies the blend factor to apply to the alpha value output from the pixel shader.
    /// Blend factors that end in _COLOR are not allowed. 
    BLEND_FACTOR    SrcBlendAlpha;

    /// Specifies the blend factor to apply to the alpha value in the render target.
    /// Blend factors that end in _COLOR are not allowed. 
    BLEND_FACTOR    DestBlendAlpha;

    /// Defines how to combine the source and destination alpha values
    /// after applying the SrcBlendAlpha and DestBlendAlpha factors.
    BLEND_OPERATION BlendOpAlpha;

    /// Defines logical operation for the render target.
	LOGIC_OPERATION LogicOp;

    /// Render target write mask
    Uint32          RenderTargetWriteMask;

    /// Constructor initializes structure members with default values

    /// Member                | Default value
    /// ----------------------|--------------
    /// BlendEnable           | False
	/// LogicOperationEnable  | False
    /// SrcBlend              | BLEND_FACTOR_ONE
    /// DestBlend             | BLEND_FACTOR_ZERO
    /// BlendOp               | BLEND_OPERATION_ADD
    /// SrcBlendAlpha         | BLEND_FACTOR_ONE
    /// DestBlendAlpha        | BLEND_FACTOR_ZERO
    /// BlendOpAlpha          | BLEND_OPERATION_ADD
	/// LogicOp				  | LOGIC_OP_NOOP
    /// RenderTargetWriteMask | COLOR_MASK_ALL
    RenderTargetBlendDesc() :
        BlendEnable     ( False ),
		LogicOperationEnable(False),
        SrcBlend        ( BLEND_FACTOR_ONE ),
        DestBlend       ( BLEND_FACTOR_ZERO ),
        BlendOp         ( BLEND_OPERATION_ADD ),
        SrcBlendAlpha   ( BLEND_FACTOR_ONE ),
        DestBlendAlpha  ( BLEND_FACTOR_ZERO ),
        BlendOpAlpha    ( BLEND_OPERATION_ADD ),
		LogicOp			( LOGIC_OP_NOOP ),
        RenderTargetWriteMask( COLOR_MASK_ALL )
    {}


    /// Comparison operator tests if two structures are equivalent

    /// \param [in] rhs - reference to the structure to perform comparison with
    /// \return 
    /// - True if all members of the two structures are equal.
    /// - False otherwise
    bool operator == (const RenderTargetBlendDesc& rhs)const
    {
        return BlendEnable           == rhs.BlendEnable    &&
			   LogicOperationEnable	 == rhs.LogicOperationEnable &&
               SrcBlend              == rhs.SrcBlend       &&
               DestBlend             == rhs.DestBlend      &&
               BlendOp               == rhs.BlendOp        &&
               SrcBlendAlpha         == rhs.SrcBlendAlpha  &&
               DestBlendAlpha        == rhs.DestBlendAlpha &&
               BlendOpAlpha          == rhs.BlendOpAlpha   &&
			   LogicOp               == rhs.LogicOp		   &&
               RenderTargetWriteMask == rhs.RenderTargetWriteMask;
    }
};

/// Blend state description

/// This structure describes the blend state which is used in a call to 
/// IRenderDevice::CreatePipelineState() to create a pipeline state object
struct BlendStateDesc
{
    /// Specifies whether to use alpha-to-coverage as a multisampling technique 
    /// when setting a pixel to a render target. Default value: False.
    Bool AlphaToCoverageEnable;

    /// Specifies whether to enable independent blending in simultaneous render targets. 
    /// If set to False, only RenderTargets[0] is used. Default value: False.
    Bool IndependentBlendEnable;

    /// Constant member defining the maximum number of render targets
    static const int MaxRenderTargets = 8;

    /// An array of RenderTargetBlendDesc structures that describe the blend 
    /// states for render targets
    RenderTargetBlendDesc RenderTargets[MaxRenderTargets];

    /// Constructor initializes structure members with default values

    /// Member                | Default value
    /// ----------------------|--------------
    /// AlphaToCoverageEnable | False
    /// IndependentBlendEnable| False
    /// 
    /// Members of RenderTargets[] are initialized with default values by 
    /// RenderTargetBlendDesc::RenderTargetBlendDesc()
    BlendStateDesc() : 
        AlphaToCoverageEnable(False),
        IndependentBlendEnable(False)
    {
    }
    
    /// Comparison operator tests if two structures are equivalent

    /// \param [in] RHS - reference to the structure to perform comparison with
    /// \return 
    /// - True if all members are of the two structures equal. 
    ///   \note The operator performs *bitwise comparison* of the two structures. 
    ///   That is if for instance both structures have IndependentBlendEnable set to False, 
    ///   but differ in render target other than 0, the operator will return False 
    ///   even though the two blend states created from these structures will be identical.
    /// - False otherwise
    bool operator == (const BlendStateDesc& RHS)const
    {
        bool bRTsEqual = true;
        for( int i = 0; i < MaxRenderTargets; ++i )
            if( !(RenderTargets[i] == RHS.RenderTargets[i]) )
            {
                bRTsEqual = false;
                break;
            }

        return  bRTsEqual &&
                AlphaToCoverageEnable == RHS.AlphaToCoverageEnable &&
                IndependentBlendEnable== RHS.IndependentBlendEnable;
    }
};

}
