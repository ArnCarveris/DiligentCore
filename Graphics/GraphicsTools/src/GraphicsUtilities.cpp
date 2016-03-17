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

#include "pch.h"
#include "GraphicsUtilities.h"
#include "DebugUtilities.h"
#include <algorithm>

namespace Diligent
{

Uint32 GetValueSize( VALUE_TYPE Val )
{
    static Uint32 Sizes[VT_NUM_TYPES] = {};
    static bool bIsInit = false;
    // Note that this implementation is thread-safe
    // Even if two threads try to call the function at the same time,
    // the worst thing that might happen is that the array will be
    // initialized multiple times. But the result will always be correct
    if( !bIsInit )
    {
        Sizes[VT_UNDEFINED] = 0;
#define INIT_VALUE_SIZE( ValType ) Sizes[ValType] = sizeof(VALUE_TYPE2CType<ValType>::CType)
        INIT_VALUE_SIZE( VT_INT8    );
        INIT_VALUE_SIZE( VT_INT16   );
        INIT_VALUE_SIZE( VT_INT32   );
        INIT_VALUE_SIZE( VT_UINT8   );
        INIT_VALUE_SIZE( VT_UINT16  );
        INIT_VALUE_SIZE( VT_UINT32  );
        INIT_VALUE_SIZE( VT_FLOAT16 );
        INIT_VALUE_SIZE( VT_FLOAT32 );
#undef  INIT_VALUE_SIZE
        static_assert(VT_NUM_TYPES == VT_FLOAT32 + 1, "Not all value type sizes initialized.");
        bIsInit = true;
    }

    if( Val > VT_UNDEFINED && Val < VT_NUM_TYPES )
    {
        return Sizes[Val];
    }
    else
    {
        UNEXPECTED( "Incorrect value type (", Val, ")" )
        return 0;
    }
}

const Char* GetValueTypeString( VALUE_TYPE Val )
{
    static const Char* ValueTypeStrings[VT_NUM_TYPES] = {};
    static bool bIsInit = false;
    if( !bIsInit )
    {
#define INIT_VALUE_TYPE_STR( ValType ) ValueTypeStrings[ValType] = #ValType
        INIT_VALUE_TYPE_STR( VT_UNDEFINED );
        INIT_VALUE_TYPE_STR( VT_INT8    );
        INIT_VALUE_TYPE_STR( VT_INT16   );
        INIT_VALUE_TYPE_STR( VT_INT32   );
        INIT_VALUE_TYPE_STR( VT_UINT8   );
        INIT_VALUE_TYPE_STR( VT_UINT16  );
        INIT_VALUE_TYPE_STR( VT_UINT32  );
        INIT_VALUE_TYPE_STR( VT_FLOAT16 );
        INIT_VALUE_TYPE_STR( VT_FLOAT32 );
#undef  INIT_VALUE_TYPE_STR
        static_assert(VT_NUM_TYPES == VT_FLOAT32 + 1, "Not all value type strings initialized.");
        bIsInit = true;
    }

    if( Val >= VT_UNDEFINED && Val < VT_NUM_TYPES )
    {
        return ValueTypeStrings[Val];
    }
    else
    {
        UNEXPECTED( "Incorrect value type (", Val, ")" )
        return "unknown value type";
    }
}

const TextureFormatAttribs& GetTextureFormatAttribs( TEXTURE_FORMAT Format )
{
    static TextureFormatAttribs FmtAttribs[TEX_FORMAT_NUM_FORMATS] = {};
    static bool bIsInit = false;
    // Note that this implementation is thread-safe
    // Even if two threads try to call the function at the same time,
    // the worst thing that might happen is that the array will be
    // initialized multiple times. But the result will always be correct.
    if( !bIsInit )
    {
#define INIT_TEX_FORMAT_INFO(TexFmt, ComponentSize, NumComponents, ComponentType) \
        FmtAttribs[ TexFmt ] = TextureFormatAttribs(TexFmt, #TexFmt, ComponentSize, NumComponents, ComponentType);

        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RGBA32_TYPELESS,         4, 4, COMPONENT_TYPE_UNDEFINED);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RGBA32_FLOAT,            4, 4, COMPONENT_TYPE_FLOAT );
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RGBA32_UINT,             4, 4, COMPONENT_TYPE_UINT);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RGBA32_SINT,             4, 4, COMPONENT_TYPE_SINT);

        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RGB32_TYPELESS,          4, 3, COMPONENT_TYPE_UNDEFINED);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RGB32_FLOAT,             4, 3, COMPONENT_TYPE_FLOAT);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RGB32_UINT,              4, 3, COMPONENT_TYPE_UINT);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RGB32_SINT,              4, 3, COMPONENT_TYPE_SINT);

        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RGBA16_TYPELESS,         2, 4, COMPONENT_TYPE_UNDEFINED);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RGBA16_FLOAT,            2, 4, COMPONENT_TYPE_FLOAT);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RGBA16_UNORM,            2, 4, COMPONENT_TYPE_UNORM);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RGBA16_UINT,             2, 4, COMPONENT_TYPE_UINT);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RGBA16_SNORM,            2, 4, COMPONENT_TYPE_SNORM);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RGBA16_SINT,             2, 4, COMPONENT_TYPE_SINT);

        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RG32_TYPELESS,           4, 2, COMPONENT_TYPE_UNDEFINED);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RG32_FLOAT,              4, 2, COMPONENT_TYPE_FLOAT);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RG32_UINT,               4, 2, COMPONENT_TYPE_UINT);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RG32_SINT,               4, 2, COMPONENT_TYPE_SINT);

        INIT_TEX_FORMAT_INFO( TEX_FORMAT_R32G8X24_TYPELESS,       4, 2, COMPONENT_TYPE_DEPTH_STENCIL);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_D32_FLOAT_S8X24_UINT,    4, 2, COMPONENT_TYPE_DEPTH_STENCIL);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_R32_FLOAT_X8X24_TYPELESS,4, 2, COMPONENT_TYPE_DEPTH_STENCIL);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_X32_TYPELESS_G8X24_UINT, 4, 2, COMPONENT_TYPE_DEPTH_STENCIL);

        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RGB10A2_TYPELESS,        4, 1, COMPONENT_TYPE_COMPOUND);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RGB10A2_UNORM,           4, 1, COMPONENT_TYPE_COMPOUND);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RGB10A2_UINT,            4, 1, COMPONENT_TYPE_COMPOUND);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_R11G11B10_FLOAT,         4, 1, COMPONENT_TYPE_COMPOUND);

        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RGBA8_TYPELESS,          1, 4, COMPONENT_TYPE_UNDEFINED);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RGBA8_UNORM,             1, 4, COMPONENT_TYPE_UNORM);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RGBA8_UNORM_SRGB,        1, 4, COMPONENT_TYPE_UNORM_SRGB);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RGBA8_UINT,              1, 4, COMPONENT_TYPE_UINT);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RGBA8_SNORM,             1, 4, COMPONENT_TYPE_SNORM);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RGBA8_SINT,              1, 4, COMPONENT_TYPE_SINT);

        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RG16_TYPELESS,           2, 2, COMPONENT_TYPE_UNDEFINED);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RG16_FLOAT,              2, 2, COMPONENT_TYPE_FLOAT);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RG16_UNORM,              2, 2, COMPONENT_TYPE_UNORM);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RG16_UINT,               2, 2, COMPONENT_TYPE_UINT);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RG16_SNORM,              2, 2, COMPONENT_TYPE_SNORM);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RG16_SINT,               2, 2, COMPONENT_TYPE_SINT);

        INIT_TEX_FORMAT_INFO( TEX_FORMAT_R32_TYPELESS,            4, 1, COMPONENT_TYPE_UNDEFINED);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_D32_FLOAT,               4, 1, COMPONENT_TYPE_DEPTH);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_R32_FLOAT,               4, 1, COMPONENT_TYPE_FLOAT);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_R32_UINT,                4, 1, COMPONENT_TYPE_UINT);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_R32_SINT,                4, 1, COMPONENT_TYPE_SINT);

        INIT_TEX_FORMAT_INFO( TEX_FORMAT_R24G8_TYPELESS,          4, 1, COMPONENT_TYPE_DEPTH_STENCIL);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_D24_UNORM_S8_UINT,       4, 1, COMPONENT_TYPE_DEPTH_STENCIL);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_R24_UNORM_X8_TYPELESS,   4, 1, COMPONENT_TYPE_DEPTH_STENCIL);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_X24_TYPELESS_G8_UINT,    4, 1, COMPONENT_TYPE_DEPTH_STENCIL);

        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RG8_TYPELESS,            1, 2, COMPONENT_TYPE_UNDEFINED);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RG8_UNORM,               1, 2, COMPONENT_TYPE_UNORM);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RG8_UINT,                1, 2, COMPONENT_TYPE_UINT);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RG8_SNORM,               1, 2, COMPONENT_TYPE_SNORM);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RG8_SINT,                1, 2, COMPONENT_TYPE_SINT);

        INIT_TEX_FORMAT_INFO( TEX_FORMAT_R16_TYPELESS,            2, 1, COMPONENT_TYPE_UNDEFINED);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_R16_FLOAT,               2, 1, COMPONENT_TYPE_FLOAT);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_D16_UNORM,               2, 1, COMPONENT_TYPE_DEPTH);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_R16_UNORM,               2, 1, COMPONENT_TYPE_UNORM);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_R16_UINT,                2, 1, COMPONENT_TYPE_UINT);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_R16_SNORM,               2, 1, COMPONENT_TYPE_SNORM);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_R16_SINT,                2, 1, COMPONENT_TYPE_SINT);

        INIT_TEX_FORMAT_INFO( TEX_FORMAT_R8_TYPELESS,             1, 1, COMPONENT_TYPE_UNDEFINED);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_R8_UNORM,                1, 1, COMPONENT_TYPE_UNORM);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_R8_UINT,                 1, 1, COMPONENT_TYPE_UINT);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_R8_SNORM,                1, 1, COMPONENT_TYPE_SNORM);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_R8_SINT,                 1, 1, COMPONENT_TYPE_SINT);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_A8_UNORM,                1, 1, COMPONENT_TYPE_UNORM);

        INIT_TEX_FORMAT_INFO( TEX_FORMAT_R1_UNORM,                1, 1, COMPONENT_TYPE_UNORM);

        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RGB9E5_SHAREDEXP,        4, 1, COMPONENT_TYPE_COMPOUND);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_RG8_B8G8_UNORM,          1, 4, COMPONENT_TYPE_UNORM);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_G8R8_G8B8_UNORM,         1, 4, COMPONENT_TYPE_UNORM);

        // http://www.g-truc.net/post-0335.html
        // http://renderingpipeline.com/2012/07/texture-compression/
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BC1_TYPELESS,            8,  3, COMPONENT_TYPE_COMPRESSED);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BC1_UNORM,               8,  3, COMPONENT_TYPE_COMPRESSED);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BC1_UNORM_SRGB,          8,  3, COMPONENT_TYPE_COMPRESSED);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BC2_TYPELESS,            16, 4, COMPONENT_TYPE_COMPRESSED);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BC2_UNORM,               16, 4, COMPONENT_TYPE_COMPRESSED);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BC2_UNORM_SRGB,          16, 4, COMPONENT_TYPE_COMPRESSED);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BC3_TYPELESS,            16, 4, COMPONENT_TYPE_COMPRESSED);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BC3_UNORM,               16, 4, COMPONENT_TYPE_COMPRESSED);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BC3_UNORM_SRGB,          16, 4, COMPONENT_TYPE_COMPRESSED);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BC4_TYPELESS,            8,  1, COMPONENT_TYPE_COMPRESSED);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BC4_UNORM,               8,  1, COMPONENT_TYPE_COMPRESSED);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BC4_SNORM,               8,  1, COMPONENT_TYPE_COMPRESSED);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BC5_TYPELESS,            16, 2, COMPONENT_TYPE_COMPRESSED);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BC5_UNORM,               16, 2, COMPONENT_TYPE_COMPRESSED);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BC5_SNORM,               16, 2, COMPONENT_TYPE_COMPRESSED);

        INIT_TEX_FORMAT_INFO( TEX_FORMAT_B5G6R5_UNORM,            2, 1, COMPONENT_TYPE_COMPOUND);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_B5G5R5A1_UNORM,          2, 1, COMPONENT_TYPE_COMPOUND);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BGRA8_UNORM,             1, 4, COMPONENT_TYPE_UNORM);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BGRX8_UNORM,             1, 4, COMPONENT_TYPE_UNORM);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_R10G10B10_XR_BIAS_A2_UNORM,  4, 1, COMPONENT_TYPE_COMPOUND);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BGRA8_TYPELESS,          1, 4, COMPONENT_TYPE_UNDEFINED);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BGRA8_UNORM_SRGB,        1, 4, COMPONENT_TYPE_UNORM_SRGB);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BGRX8_TYPELESS,          1, 4, COMPONENT_TYPE_UNDEFINED);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BGRX8_UNORM_SRGB,        1, 4, COMPONENT_TYPE_UNORM_SRGB);

        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BC6H_TYPELESS,           16, 3, COMPONENT_TYPE_COMPRESSED);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BC6H_UF16,               16, 3, COMPONENT_TYPE_COMPRESSED);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BC6H_SF16,               16, 3, COMPONENT_TYPE_COMPRESSED);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BC7_TYPELESS,            16, 4, COMPONENT_TYPE_COMPRESSED);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BC7_UNORM,               16, 4, COMPONENT_TYPE_COMPRESSED);
        INIT_TEX_FORMAT_INFO( TEX_FORMAT_BC7_UNORM_SRGB,          16, 4, COMPONENT_TYPE_COMPRESSED);
#undef  INIT_TEX_FORMAT_INFO
        static_assert(TEX_FORMAT_NUM_FORMATS == TEX_FORMAT_BC7_UNORM_SRGB + 1, "Not all texture formats initialized.");

#ifdef _DEBUG
        for( Uint32 Fmt = TEX_FORMAT_UNKNOWN; Fmt < TEX_FORMAT_NUM_FORMATS; ++Fmt )
            VERIFY(FmtAttribs[Fmt].Format == static_cast<TEXTURE_FORMAT>(Fmt), "Uninitialized format");
#endif

        bIsInit = true;
    }

    if( Format >= TEX_FORMAT_UNKNOWN && Format < TEX_FORMAT_NUM_FORMATS )
    {
        const auto &Attribs = FmtAttribs[Format];
        VERIFY( Attribs.Format == Format, "Unexpected format" );
        return Attribs;
    }
    else
    {
        UNEXPECTED( "Texture format (", Format, ") is out of allowed range [0, ", TEX_FORMAT_NUM_FORMATS-1, "]" );
        return FmtAttribs[0];
    }
}


const Char *GetTexViewTypeLiteralName( TEXTURE_VIEW_TYPE ViewType )
{
    static const Char *TexViewLiteralNames[TEXTURE_VIEW_NUM_VIEWS] = {};
    static bool bIsInit = false;
    // Note that this implementation is thread-safe
    // Even if two threads try to call the function at the same time,
    // the worst thing that might happen is that the array will be
    // initialized multiple times. But the result will always be correct.
    if( !bIsInit )
    {
#define INIT_TEX_VIEW_TYPE_NAME(ViewType)TexViewLiteralNames[ViewType] = #ViewType
        INIT_TEX_VIEW_TYPE_NAME( TEXTURE_VIEW_UNDEFINED );
        INIT_TEX_VIEW_TYPE_NAME( TEXTURE_VIEW_SHADER_RESOURCE );
        INIT_TEX_VIEW_TYPE_NAME( TEXTURE_VIEW_RENDER_TARGET );
        INIT_TEX_VIEW_TYPE_NAME( TEXTURE_VIEW_DEPTH_STENCIL );
        INIT_TEX_VIEW_TYPE_NAME( TEXTURE_VIEW_UNORDERED_ACCESS );
#undef  INIT_TEX_VIEW_TYPE_NAME
        static_assert(TEXTURE_VIEW_NUM_VIEWS == TEXTURE_VIEW_UNORDERED_ACCESS + 1, "Not all texture views names initialized.");

        bIsInit = true;
    }

    if( ViewType >= TEXTURE_VIEW_UNDEFINED && ViewType < TEXTURE_VIEW_NUM_VIEWS )
    {
        return TexViewLiteralNames[ViewType];
    }
    else
    {
        UNEXPECTED("Texture view type (", ViewType, ") is out of allowed range [0, ", TEXTURE_VIEW_NUM_VIEWS-1, "]" );
        return "<Unknown texture view type>";
    }
}

const Char *GetBufferViewTypeLiteralName( BUFFER_VIEW_TYPE ViewType )
{
    static const Char *BuffViewLiteralNames[BUFFER_VIEW_NUM_VIEWS] = {};
    static bool bIsInit = false;
    // Note that this implementation is thread-safe
    // Even if two threads try to call the function at the same time,
    // the worst thing that might happen is that the array will be
    // initialized multiple times. But the result will always be correct.
    if( !bIsInit )
    {
#define INIT_BUFF_VIEW_TYPE_NAME(ViewType)BuffViewLiteralNames[ViewType] = #ViewType
        INIT_BUFF_VIEW_TYPE_NAME( BUFFER_VIEW_UNDEFINED );
        INIT_BUFF_VIEW_TYPE_NAME( BUFFER_VIEW_SHADER_RESOURCE );
        INIT_BUFF_VIEW_TYPE_NAME( BUFFER_VIEW_UNORDERED_ACCESS );
 #undef INIT_BUFF_VIEW_TYPE_NAME
        static_assert(BUFFER_VIEW_NUM_VIEWS == BUFFER_VIEW_UNORDERED_ACCESS + 1, "Not all buffer views names initialized.");

        bIsInit = true;
    }

    if( ViewType >= BUFFER_VIEW_UNDEFINED && ViewType < BUFFER_VIEW_NUM_VIEWS )
    {
        return BuffViewLiteralNames[ViewType];
    }
    else
    {
        UNEXPECTED( "Buffer view type (", ViewType, ") is out of allowed range [0, ", BUFFER_VIEW_NUM_VIEWS-1, "]" )
        return "<Unknown buffer view type>";
    }
}

const Char *GetShaderTypeLiteralName( SHADER_TYPE ShaderType )
{
    switch( ShaderType )
    {
#define RETURN_SHADER_TYPE_NAME(ShaderType)\
        case ShaderType: return #ShaderType;

        RETURN_SHADER_TYPE_NAME( SHADER_TYPE_UNKNOWN )
        RETURN_SHADER_TYPE_NAME( SHADER_TYPE_VERTEX  )
        RETURN_SHADER_TYPE_NAME( SHADER_TYPE_PIXEL   )
        RETURN_SHADER_TYPE_NAME( SHADER_TYPE_GEOMETRY)
        RETURN_SHADER_TYPE_NAME( SHADER_TYPE_HULL    )
        RETURN_SHADER_TYPE_NAME( SHADER_TYPE_DOMAIN  )
        RETURN_SHADER_TYPE_NAME( SHADER_TYPE_COMPUTE )
#undef  RETURN_SHADER_TYPE_NAME

        default: UNEXPECTED( "Unknown shader type" ); return "<Unknown shader type>";
    }
}

const Char *GetShaderVariableTypeLiteralName(SHADER_VARIABLE_TYPE VarType, bool bGetFullName)
{
    static const Char* ShortVarTypeNameStrings[SHADER_VARIABLE_TYPE_NUM_TYPES];
    static const Char* FullVarTypeNameStrings[SHADER_VARIABLE_TYPE_NUM_TYPES];
    static bool bVarTypeStrsInit = false;
    if( !bVarTypeStrsInit )
    {
        ShortVarTypeNameStrings[SHADER_VARIABLE_TYPE_STATIC]  = "static";
        ShortVarTypeNameStrings[SHADER_VARIABLE_TYPE_MUTABLE] = "mutable";
        ShortVarTypeNameStrings[SHADER_VARIABLE_TYPE_DYNAMIC] = "dynamic";
        FullVarTypeNameStrings[SHADER_VARIABLE_TYPE_STATIC]  = "SHADER_VARIABLE_TYPE_STATIC";
        FullVarTypeNameStrings[SHADER_VARIABLE_TYPE_MUTABLE] = "SHADER_VARIABLE_TYPE_MUTABLE";
        FullVarTypeNameStrings[SHADER_VARIABLE_TYPE_DYNAMIC] = "SHADER_VARIABLE_TYPE_DYNAMIC";

        static_assert(SHADER_VARIABLE_TYPE_NUM_TYPES == SHADER_VARIABLE_TYPE_DYNAMIC + 1, "Not all shader variable types initialized.");

        bVarTypeStrsInit = true;
    }
    if( VarType >= SHADER_VARIABLE_TYPE_STATIC && VarType < SHADER_VARIABLE_TYPE_NUM_TYPES )
        return (bGetFullName ? FullVarTypeNameStrings : ShortVarTypeNameStrings)[VarType];
    else
    {
        UNEXPECTED("Unknow shader variable type")
        return "unknow";
    }
}

/// Returns the string containing the usage
const Char* GetUsageString( USAGE Usage )
{
    static const Char* UsageStrings[4];
    static bool bUsageStringsInit = false;
    if( !bUsageStringsInit )
    {
#define INIT_USGAGE_STR(Usage)UsageStrings[Usage] = #Usage
        INIT_USGAGE_STR( USAGE_STATIC );
        INIT_USGAGE_STR( USAGE_DEFAULT );
        INIT_USGAGE_STR( USAGE_DYNAMIC );
        INIT_USGAGE_STR( USAGE_CPU_ACCESSIBLE );
#undef  INIT_USGAGE_STR
        bUsageStringsInit = true;
    }
    if( Usage >= USAGE_STATIC && Usage <= USAGE_CPU_ACCESSIBLE )
        return UsageStrings[Usage];
    else
    {
        UNEXPECTED("Unknow usage" )
        return "Unknown usage";
    }
}

const Char* GetResourceDimString( RESOURCE_DIMENSION TexType )
{
    static const Char* TexTypeStrings[RESOURCE_DIM_NUM_DIMENSIONS];
    static bool bTexTypeStrsInit = false;
    if( !bTexTypeStrsInit )
    {
        TexTypeStrings[RESOURCE_DIM_UNDEFINED]      = "Undefined";
        TexTypeStrings[RESOURCE_DIM_BUFFER]         = "Buffer";
        TexTypeStrings[RESOURCE_DIM_TEX_1D]         = "Tex 1D";
        TexTypeStrings[RESOURCE_DIM_TEX_1D_ARRAY]   = "Tex 1D Array";
        TexTypeStrings[RESOURCE_DIM_TEX_2D]         = "Tex 2D";
        TexTypeStrings[RESOURCE_DIM_TEX_2D_ARRAY]   = "Tex 2D Array";
        TexTypeStrings[RESOURCE_DIM_TEX_3D]         = "Tex 3D";
        TexTypeStrings[RESOURCE_DIM_TEX_2D_ARRAY]   = "Tex Cube";
        TexTypeStrings[RESOURCE_DIM_TEX_3D]         = "Tex Cube Array";
        static_assert(RESOURCE_DIM_NUM_DIMENSIONS == RESOURCE_DIM_TEX_CUBE_ARRAY + 1, "Not all texture type strings initialized.");

        bTexTypeStrsInit = true;
    }
    if( TexType >= RESOURCE_DIM_UNDEFINED && TexType < RESOURCE_DIM_NUM_DIMENSIONS )
        return TexTypeStrings[TexType];
    else
    {
        UNEXPECTED("Unknow texture type")
        return "Unknow texture type";
    }
}

static const Char* GetSingleBindFlagString( Uint32 BindFlag )
{
    VERIFY( (BindFlag & (BindFlag - 1)) == 0, "More than one bind flag specified" );
    switch( BindFlag )
    {
#define BIND_FLAG_STR_CASE(Flag) case Flag: return #Flag;
        BIND_FLAG_STR_CASE( BIND_VERTEX_BUFFER )
	    BIND_FLAG_STR_CASE( BIND_INDEX_BUFFER  )
	    BIND_FLAG_STR_CASE( BIND_UNIFORM_BUFFER  )
	    BIND_FLAG_STR_CASE( BIND_SHADER_RESOURCE )
	    BIND_FLAG_STR_CASE( BIND_STREAM_OUTPUT )
	    BIND_FLAG_STR_CASE( BIND_RENDER_TARGET )
	    BIND_FLAG_STR_CASE( BIND_DEPTH_STENCIL )
	    BIND_FLAG_STR_CASE( BIND_UNORDERED_ACCESS )
        BIND_FLAG_STR_CASE( BIND_INDIRECT_DRAW_ARGS )
#undef  BIND_FLAG_STR_CASE
        default: UNEXPECTED( "Unexpected bind flag ", BindFlag ); return "";
    }
}

String GetBindFlagsString( Uint32 BindFlags )
{
    if( BindFlags == 0 )
        return "0";
    String Str;
    for( Uint32 Flag = BIND_VERTEX_BUFFER; BindFlags && Flag <= BIND_INDIRECT_DRAW_ARGS; Flag <<= 1 )
    {
        if( BindFlags&Flag )
        {
            if( Str.length() )
                Str += '|';
            Str += GetSingleBindFlagString( Flag );
            BindFlags &= ~Flag;
        }
    }
    VERIFY( BindFlags == 0, "Unknown bind flags left" );
    return Str;
}


static const Char* GetSingleCPUAccessFlagString( Uint32 CPUAccessFlag )
{
    VERIFY( (CPUAccessFlag & (CPUAccessFlag - 1)) == 0, "More than one access flag specified" );
    switch( CPUAccessFlag )
    {
#define CPU_ACCESS_FLAG_STR_CASE(Flag) case Flag: return #Flag;
        CPU_ACCESS_FLAG_STR_CASE( CPU_ACCESS_READ )
	    CPU_ACCESS_FLAG_STR_CASE( CPU_ACCESS_WRITE  )
#undef  CPU_ACCESS_FLAG_STR_CASE
        default: UNEXPECTED( "Unexpected CPU access flag ", CPUAccessFlag ); return "";
    }
}

String GetCPUAccessFlagsString( Uint32 CpuAccessFlags )
{
    if( CpuAccessFlags == 0 )
        return "0";
    String Str;
    for( Uint32 Flag = CPU_ACCESS_READ; CpuAccessFlags && Flag <= CPU_ACCESS_WRITE; Flag <<= 1 )
    {
        if( CpuAccessFlags&Flag )
        {
            if( Str.length() )
                Str += '|';
            Str += GetSingleCPUAccessFlagString( Flag );
            CpuAccessFlags &= ~Flag;
        }
    }
    VERIFY( CpuAccessFlags == 0, "Unknown CPU access flags left" );
    return Str;
}

// There is a nice standard function to_string, but there is a bug on gcc
// and it does not work
template<typename T>
String ToString( T Val )
{
    std::stringstream ss;
    ss << Val;
    return ss.str();
}

String GetTextureDescString( const TextureDesc &Desc )
{
    String Str = "Type: ";
    Str += GetResourceDimString(Desc.Type);
    Str += "; size: ";
    Str += ToString(Desc.Width);
    if( Desc.Type == RESOURCE_DIM_TEX_2D || Desc.Type == RESOURCE_DIM_TEX_2D_ARRAY || Desc.Type == RESOURCE_DIM_TEX_3D)
    {
        Str += "x";
        Str += ToString( Desc.Height );
    }

    if( Desc.Type == RESOURCE_DIM_TEX_3D )
    {
        Str += "x";
        Str += ToString( Desc.Depth );
    }
    
    if( Desc.Type == RESOURCE_DIM_TEX_1D_ARRAY || Desc.Type == RESOURCE_DIM_TEX_2D_ARRAY )
    {
        Str += "; Num Slices: ";
        Str += ToString( Desc.ArraySize );
    }

    auto FmtName = GetTextureFormatAttribs( Desc.Format ).Name;
    Str += "; Format: ";
    Str += FmtName;

    Str += "; Mip levels: ";
    Str += ToString( Desc.MipLevels );

    Str += "; Sample Count: ";
    Str += ToString( Desc.SampleCount );

    Str += "; Usage: ";
    Str += GetUsageString(Desc.Usage);

    Str += "; Bind Flags: ";
    Str += GetBindFlagsString(Desc.BindFlags);

    Str += "; CPU access: ";
    Str += GetCPUAccessFlagsString(Desc.CPUAccessFlags);

    return Str;
}

static const Char* GetBufferModeString( BUFFER_MODE Mode )
{
    static const Char* BufferModeStrings[BUFFER_MODE_NUM_MODES];
    static bool bBuffModeStringsInit = false;
    if( !bBuffModeStringsInit )
    {
#define INIT_BUFF_MODE_STR(Mode)BufferModeStrings[Mode] = #Mode
        INIT_BUFF_MODE_STR( BUFFER_MODE_UNDEFINED );
        INIT_BUFF_MODE_STR( BUFFER_MODE_FORMATED );
        INIT_BUFF_MODE_STR( BUFFER_MODE_STRUCTURED );
#undef  INIT_BUFF_MODE_STR
        static_assert(BUFFER_MODE_NUM_MODES == BUFFER_MODE_STRUCTURED + 1, "Not all buffer mode strings initialized.");
        bBuffModeStringsInit = true;
    }
    if( Mode >= BUFFER_MODE_UNDEFINED && Mode < BUFFER_MODE_NUM_MODES )
        return BufferModeStrings[Mode];
    else
    {
        UNEXPECTED( "Unknown buffer mode" );
        return "Unknown buffer mode";
    }
}

String GetBufferDescString( const BufferDesc &Desc )
{
    String Str;
    Str += "Size: ";
    bool bIsLarge = false;
    if( Desc.uiSizeInBytes > (1 << 20) )
    {
        Str += ToString( Desc.uiSizeInBytes / (1<<20) );
        Str += " Mb (";
        bIsLarge = true;
    }
    else if( Desc.uiSizeInBytes > (1 << 10) )
    {
        Str += ToString( Desc.uiSizeInBytes / (1<<10) );
        Str += " Kb (";
        bIsLarge = true;
    }

    Str += ToString( Desc.uiSizeInBytes );
    Str += " bytes";
    if( bIsLarge )
        Str += ')';
    
    Str += "; Mode: ";
    Str += GetBufferModeString(Desc.Mode);
    if( Desc.Mode == BUFFER_MODE_FORMATED )
    {
        Str += "; Format: ";
        Str += GetValueTypeString( Desc.Format.ValueType );
        if( Desc.Format.IsNormalized )
            Str += " norm";
        Str += " x ";
        Str += ToString(Desc.Format.NumComponents);
    }

    Str += "; Usage: ";
    Str += GetUsageString(Desc.Usage);

    Str += "; Bind Flags: ";
    Str += GetBindFlagsString(Desc.BindFlags);

    Str += "; CPU access: ";
    Str += GetCPUAccessFlagsString(Desc.CPUAccessFlags);

    Str += "; stride: ";
    Str += ToString( Desc.ElementByteStride );
    Str += " bytes";

    return Str;
}

Uint32 ComputeMipLevelsCount( Uint32 Width )
{
    Uint32 MipLevels = 0;
    while( (Width >> MipLevels) > 1 )
        ++MipLevels;
    return MipLevels;
}

Uint32 ComputeMipLevelsCount( Uint32 Width, Uint32 Height )
{
    return ComputeMipLevelsCount( std::max( Width, Height ) );
}

Uint32 ComputeMipLevelsCount( Uint32 Width, Uint32 Height, Uint32 Depth )
{
    return ComputeMipLevelsCount( std::max(std::max( Width, Height ), Depth) );
}

void CreateUniformBuffer( IRenderDevice *pDevice, Uint32 Size, IBuffer **ppBuffer, USAGE Usage, Uint32 BindFlag, Uint32 CPUAccessFlags)
{
    BufferDesc CBDesc;
    CBDesc.uiSizeInBytes = Size;
    CBDesc.Usage = Usage;
    CBDesc.BindFlags = BindFlag;
    CBDesc.CPUAccessFlags = CPUAccessFlags;
    pDevice->CreateBuffer( CBDesc, BufferData(), ppBuffer );
}

}
