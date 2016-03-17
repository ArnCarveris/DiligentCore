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
/// Defines Diligent::DefaultRawMemoryAllocator class

#include "BasicTypes.h"

namespace Diligent
{

template <typename T, typename AllocatorType>
struct STDAllocator
{
    typedef T value_type;

    STDAllocator(AllocatorType& Allocator, const Char* dbgDescription, const Char* dbgFileName, const  Int32 dbgLineNumber) : 
        m_Allocator(Allocator),
        m_dbgDescription(dbgDescription),
        m_dbgFileName(dbgFileName),
        m_dbgLineNumber(dbgLineNumber)
    {
    }

    template <class U> 
    STDAllocator(const STDAllocator<U, AllocatorType>& other) : 
        m_Allocator(other.m_Allocator),
        m_dbgDescription(other.m_dbgDescription),
        m_dbgFileName(other.m_dbgFileName),
        m_dbgLineNumber(other.m_dbgLineNumber)
    {
    }

    T* allocate(std::size_t count)
    {
        return reinterpret_cast<T*>( m_Allocator.Allocate(count * sizeof(T), m_dbgDescription, m_dbgFileName, m_dbgLineNumber ) );
    }

    void deallocate(T* p, std::size_t count)
    {
        m_Allocator.Free(p);
    }

    AllocatorType &m_Allocator;
    const Char* m_dbgDescription;
    const Char* m_dbgFileName;
    const Int32 m_dbgLineNumber;
};

#define STD_ALLOCATOR(Type, AllocatorType, Allocator, Description) STDAllocator<Type, AllocatorType>(Allocator, Description, __FILE__, __LINE__)

template <class T, class U, class A>
bool operator==(const STDAllocator<T, A>&left, const STDAllocator<U, A>&right)
{
    return &left.m_Allocator == &right.m_Allocator;
}

template <class T, class U, class A>
bool operator!=(const STDAllocator<T, A> &left, const STDAllocator<U, A> &right)
{
    return !(left == right);
}

template<class T> using STDAllocatorRawMem = STDAllocator<T, IMemoryAllocator>; 
#define STD_ALLOCATOR_RAW_MEM(Type, Allocator, Description) STDAllocatorRawMem<Type>(Allocator, Description, __FILE__, __LINE__)

template< class T, typename AllocatorType > 
struct STDDeleter
{
    STDDeleter(AllocatorType &Allocator) : 
        m_Allocator(Allocator)
    {}
        
    void operator()(T *ptr)
    {
        ptr->~T();
        m_Allocator.Free(ptr);
    }

    AllocatorType &m_Allocator;
};
template<class T> using STDDeleterRawMem = STDDeleter<T, IMemoryAllocator>; 

}
