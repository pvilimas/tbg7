/**********************************************************************************************
*
*   rmem - raylib memory pool and objects pool
*
*   A quick, efficient, and minimal free list and stack-based allocator
*
*   PURPOSE:
*     - Aquicker, efficient memory allocator alternative to 'malloc' and friends.
*     - Reduce the possibilities of memory leaks for beginner developers using Raylib.
*     - Being able to flexibly range check memory if necessary.
*
*   CONFIGURATION:
*
*   #define RMEM_IMPLEMENTATION
*       Generates the implementation of the library into the included file.
*       If not defined, the library is in header only mode and can be included in other headers
*       or source files without problems. But only ONE file should hold the implementation.
*
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2019 Kevin 'Assyrianic' Yonan (@assyrianic) and reviewed by Ramon Santamaria (@raysan5)
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

#ifndef RMEM_H
#define RMEM_H

#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
#if defined(_WIN32) && defined(BUILD_LIBTYPE_SHARED)
    #define RMEMAPI __declspec(dllexport)         // We are building library as a Win32 shared library (.dll)
#elif defined(_WIN32) && defined(USE_LIBTYPE_SHARED)
    #define RMEMAPI __declspec(dllimport)         // We are using library as a Win32 shared library (.dll)
#else
    #define RMEMAPI   // We are building or using library as a static library (or Linux shared library)
#endif
    
//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------

// Memory Pool
typedef struct MemNode MemNode;
struct MemNode {
    size_t size;
    MemNode *next, *prev;
};

typedef struct AllocList {
    MemNode *head, *tail;
    size_t len, maxNodes;
    bool autoDefrag : 1;
} AllocList;

typedef struct Stack {
    uint8_t *mem, *base;
    size_t size;
} Stack;

typedef struct MemPool {
    AllocList freeList;
    Stack stack;
} MemPool;

// Object Pool
typedef struct ObjPool {
    Stack stack;
    size_t objSize, freeBlocks;
} ObjPool;


#if defined(__cplusplus)
extern "C" {            // Prevents name mangling of functions
#endif

//------------------------------------------------------------------------------------
// Functions Declaration - Memory Pool
//------------------------------------------------------------------------------------
RMEMAPI MemPool CreateMemPool(size_t bytes);
RMEMAPI MemPool CreateMemPoolFromBuffer(void *buf, size_t bytes);
RMEMAPI void DestroyMemPool(MemPool *mempool);

RMEMAPI void *MemPoolAlloc(MemPool *mempool, size_t bytes);
RMEMAPI void *MemPoolRealloc(MemPool *mempool, void *ptr, size_t bytes);
RMEMAPI void MemPoolFree(MemPool *mempool, void *ptr);
RMEMAPI void MemPoolCleanUp(MemPool *mempool, void **ptrref);
RMEMAPI bool MemPoolDefrag(MemPool *mempool);

RMEMAPI size_t GetMemPoolFreeMemory(const MemPool mempool);
RMEMAPI void ToggleMemPoolAutoDefrag(MemPool *mempool);

//------------------------------------------------------------------------------------
// Functions Declaration - Object Pool
//------------------------------------------------------------------------------------
RMEMAPI ObjPool CreateObjPool(size_t objsize, size_t len);
RMEMAPI ObjPool CreateObjPoolFromBuffer(void *buf, size_t objsize, size_t len);
RMEMAPI void DestroyObjPool(ObjPool *objpool);

RMEMAPI void *ObjPoolAlloc(ObjPool *objpool);
RMEMAPI void ObjPoolFree(ObjPool *objpool, void *ptr);
RMEMAPI void ObjPoolCleanUp(ObjPool *objpool, void **ptrref);


#ifdef __cplusplus
}
#endif

#endif // RMEM_H

/***********************************************************************************
*
*   RMEM IMPLEMENTATION
*
************************************************************************************/

#if defined(RMEM_IMPLEMENTATION)

#include <stdio.h>          // Required for: 
#include <stdlib.h>         // Required for: 
#include <string.h>         // Required for: 

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------

// Make sure restrict type qualifier for pointers is defined
// NOTE: Not supported by C++, it is a C only keyword
#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__) || defined(_MSC_VER)
    #ifndef restrict
        #define restrict __restrict
    #endif
#endif

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
// ...

//----------------------------------------------------------------------------------
// Module specific Functions Declaration
//----------------------------------------------------------------------------------
static inline size_t __AlignSize(const size_t size, const size_t align) 
{ 
    return (size + (align - 1)) & -align;
}

static void __RemoveNode(MemNode **const node)
{
    ((*node)->prev != NULL)? ((*node)->prev->next = (*node)->next) : (*node = (*node)->next);
    ((*node)->next != NULL)? ((*node)->next->prev = (*node)->prev) : (*node = (*node)->prev);
}

//----------------------------------------------------------------------------------
// Module Functions Definition - Memory Pool
//----------------------------------------------------------------------------------

MemPool CreateMemPool(const size_t size)
{
    MemPool mempool = { 0 };
    
    if (size == 0UL) return mempool;
    else 
    {
        // Align the mempool size to at least the size of an alloc node.
        mempool.stack.size = size;
        mempool.stack.mem = malloc(1 + mempool.stack.size*sizeof *mempool.stack.mem);

        if (mempool.stack.mem==NULL)
        {
            mempool.stack.size = 0UL;
            return mempool;
        }
        else 
        {
            mempool.stack.base = mempool.stack.mem + mempool.stack.size;
            return mempool;
        }
    }
}

MemPool CreateMemPoolFromBuffer(void *buf, const size_t size)
{
    MemPool mempool = { 0 };
    
    if ((size == 0UL) || (buf == NULL) || (size <= sizeof(MemNode))) return mempool;
    else 
    {
        mempool.stack.size = size;
        mempool.stack.mem = buf;
        mempool.stack.base = mempool.stack.mem + mempool.stack.size;
        return mempool;
    }
}

void DestroyMemPool(MemPool *const mempool)
{
    if ((mempool == NULL) || (mempool->stack.mem == NULL)) return;
    else 
    {
        free(mempool->stack.mem);
        *mempool = (MemPool){ 0 };
    }
}

void *MemPoolAlloc(MemPool *const mempool, const size_t size)
{
    if ((mempool == NULL) || (size == 0UL) || (size > mempool->stack.size)) return NULL;
    else
    {
        MemNode *new_mem = NULL;
        const size_t ALLOC_SIZE = __AlignSize(size + sizeof *new_mem, sizeof(intptr_t));
        
        if (mempool->freeList.head != NULL)
        {
            const size_t MEM_SPLIT_THRESHOLD = sizeof(intptr_t);
            
            // If the freelist is valid, let's allocate FROM the freelist then!
            for (MemNode **inode = &mempool->freeList.head; *inode != NULL; inode = &(*inode)->next)
            {
                if ((*inode)->size < ALLOC_SIZE) continue;
                else if ((*inode)->size <= (ALLOC_SIZE + MEM_SPLIT_THRESHOLD))
                {
                    // Close in size - reduce fragmentation by not splitting.
                    new_mem = *inode;
                    __RemoveNode(inode);
                    mempool->freeList.len--;
                    new_mem->next = new_mem->prev = NULL;
                    break;
                }
                else
                {
                    // Split the memory chunk.
                    new_mem = (MemNode *)((uint8_t *)*inode + ((*inode)->size - ALLOC_SIZE));
                    (*inode)->size -= ALLOC_SIZE;
                    new_mem->size = ALLOC_SIZE;
                    new_mem->next = new_mem->prev = NULL;
                    break;
                }
            }
        }
        
        if (new_mem == NULL)
        {
            // not enough memory to support the size!
            if ((mempool->stack.base - ALLOC_SIZE) < mempool->stack.mem) return NULL;
            else
            {
                // Couldn't allocate from a freelist, allocate from available mempool.
                // Subtract allocation size from the mempool.
                mempool->stack.base -= ALLOC_SIZE;
                
                // Use the available mempool space as the new node.
                new_mem = (MemNode *)mempool->stack.base;
                new_mem->size = ALLOC_SIZE;
                new_mem->next = new_mem->prev = NULL;
            }
        }
        
        // Visual of the allocation block.
        // --------------
        // | mem size   | lowest addr of block
        // | next node  |
        // --------------
        // |   alloc'd  |
        // |   memory   |
        // |   space    | highest addr of block
        // --------------
        uint8_t *const final_mem = (uint8_t *)new_mem + sizeof *new_mem;
        memset(final_mem, 0, new_mem->size - sizeof *new_mem);
        return final_mem;
    }
}

void *MemPoolRealloc(MemPool *const restrict mempool, void *ptr, const size_t size)
{
    if ((mempool == NULL) || (size > mempool->stack.size)) return NULL;
    // NULL ptr should make this work like regular Allocation.
    else if (ptr == NULL) return MemPoolAlloc(mempool, size);
    else if ((uintptr_t)ptr <= (uintptr_t)mempool->stack.mem) return NULL;
    else
    {
        MemNode *node = (MemNode *)((uint8_t *)ptr - sizeof *node);
        const size_t NODE_SIZE = sizeof *node;
        uint8_t *resized_block = MemPoolAlloc(mempool, size);
        
        if (resized_block == NULL) return NULL;
        else
        {
            MemNode *resized = (MemNode *)(resized_block - sizeof *resized);
            memmove(resized_block, ptr, (node->size > resized->size)? (resized->size - NODE_SIZE) : (node->size - NODE_SIZE));
            MemPoolFree(mempool, ptr);
            return resized_block;
        }
    }
}

void MemPoolFree(MemPool *const restrict mempool, void *ptr)
{
    if ((mempool == NULL) || (ptr == NULL) || ((uintptr_t)ptr <= (uintptr_t)mempool->stack.mem)) return;
    else 
    {
        // Behind the actual pointer data is the allocation info.
        MemNode *mem_node = (MemNode *)((uint8_t *)ptr - sizeof *mem_node);
        
        // Make sure the pointer data is valid.
        if (((uintptr_t)mem_node < (uintptr_t)mempool->stack.base) || 
            (((uintptr_t)mem_node - (uintptr_t)mempool->stack.mem) > mempool->stack.size) || 
            (mem_node->size == 0UL) || 
            (mem_node->size > mempool->stack.size)) return;
        // If the mem_node is right at the stack base ptr, then add it to the stack.
        else if ((uintptr_t)mem_node == (uintptr_t)mempool->stack.base)
        {
            mempool->stack.base += mem_node->size;
        }
        // Otherwise, we add it to the free list.
        // We also check if the freelist already has the pointer so we can prevent double frees.
        else if ((mempool->freeList.len == 0UL) || ((uintptr_t)mempool->freeList.head >= (uintptr_t)mempool->stack.mem && (uintptr_t)mempool->freeList.head - (uintptr_t)mempool->stack.mem < mempool->stack.size))
        {
            for (MemNode *n = mempool->freeList.head; n != NULL; n = n->next) if (n == mem_node) return;
            
            // This code inserts at head.
            /*
            ( mempool->freeList.head==NULL)? (mempool->freeList.tail = mem_node) : (mempool->freeList.head->prev = mem_node);
            mem_node->next = mempool->freeList.head;
            mempool->freeList.head = mem_node;
            mempool->freeList.len++;
            */
            
            // This code insertion sorts where largest size is first.
            if (mempool->freeList.head == NULL)
            {
                mempool->freeList.head = mempool->freeList.tail = mem_node;
                mempool->freeList.len++;
            } 
            else if (mempool->freeList.head->size <= mem_node->size)
            {
                mem_node->next = mempool->freeList.head;
                mem_node->next->prev = mem_node;
                mempool->freeList.head = mem_node;
                mempool->freeList.len++;
            }
            else if (mempool->freeList.tail->size > mem_node->size)
            {
                mem_node->prev = mempool->freeList.tail;
                mempool->freeList.tail->next = mem_node;
                mempool->freeList.tail = mem_node;
                mempool->freeList.len++;
            } 
            else
            {
                MemNode *n = mempool->freeList.head;
                while ((n->next != NULL) && (n->next->size > mem_node->size)) n = n->next;
                
                mem_node->next = n->next;
                if (n->next != NULL) mem_node->next->prev = mem_node;
                
                n->next = mem_node;
                mem_node->prev = n;
                mempool->freeList.len++;
            }
            
            if (mempool->freeList.autoDefrag && (mempool->freeList.maxNodes != 0UL) && (mempool->freeList.len > mempool->freeList.maxNodes)) MemPoolDefrag(mempool);
        }
    }
}

void MemPoolCleanUp(MemPool *const restrict mempool, void **ptrref)
{
    if ((mempool == NULL) || (ptrref == NULL) || (*ptrref == NULL)) return;
    else 
    {
        MemPoolFree(mempool, *ptrref);
        *ptrref = NULL;
    }
}

size_t GetMemPoolFreeMemory(const MemPool mempool)
{
    size_t total_remaining = (uintptr_t)mempool.stack.base - (uintptr_t)mempool.stack.mem;
    
    for (MemNode *n=mempool.freeList.head; n != NULL; n = n->next) total_remaining += n->size;
    
    return total_remaining;
}

bool MemPoolDefrag(MemPool *const mempool)
{
    if (mempool == NULL) return false;
    else
    {
        // If the memory pool has been entirely released, fully defrag it.
        if (mempool->stack.size == GetMemPoolFreeMemory(*mempool))
        {
            memset(&mempool->freeList, 0, sizeof mempool->freeList);
            mempool->stack.base = mempool->stack.mem + mempool->stack.size;
            return true;
        } 
        else
        {
            const size_t PRE_DEFRAG_LEN = mempool->freeList.len;
            MemNode **node = &mempool->freeList.head;
            
            while (*node != NULL)
            {
                if ((uintptr_t)*node == (uintptr_t)mempool->stack.base) 
                {
                    // If node is right at the stack, merge it back into the stack.
                    mempool->stack.base += (*node)->size;
                    (*node)->size = 0UL;
                    __RemoveNode(node);
                    mempool->freeList.len--;
                    node = &mempool->freeList.head;
                } 
                else if (((uintptr_t)*node + (*node)->size) == (uintptr_t)(*node)->next)
                {
                    // Next node is at a higher address.
                    (*node)->size += (*node)->next->size;
                    (*node)->next->size = 0UL;
                    
                    // <-[P Curr N]-> <-[P Next N]-> <-[P NextNext N]->
                    // 
                    //           |--------------------|
                    // <-[P Curr N]-> <-[P Next N]-> [P NextNext N]->
                    if ((*node)->next->next != NULL) (*node)->next->next->prev = *node;
                    
                    // <-[P Curr N]-> <-[P NextNext N]->
                    (*node)->next = (*node)->next->next;
                    
                    mempool->freeList.len--;
                    node = &mempool->freeList.head;
                }
                else if ((((uintptr_t)*node + (*node)->size) == (uintptr_t)(*node)->prev) && ((*node)->prev->prev != NULL))
                {
                    // Prev node is at a higher address.
                    (*node)->size += (*node)->prev->size;
                    (*node)->prev->size = 0UL;
                    
                    // <-[P PrevPrev N]-> <-[P Prev N]-> <-[P Curr N]->
                    //
                    //               |--------------------|
                    // <-[P PrevPrev N] <-[P Prev N]-> <-[P Curr N]->
                    (*node)->prev->prev->next = *node;
                    
                    // <-[P PrevPrev N]-> <-[P Curr N]->
                    (*node)->prev = (*node)->prev->prev;
                    
                    mempool->freeList.len--;
                    node = &mempool->freeList.head;
                }
                else if ((*node)->prev != NULL && (*node)->next != NULL && (uintptr_t)*node - (*node)->next->size == (uintptr_t)(*node)->next)
                {
                    // Next node is at a lower address.
                    (*node)->next->size += (*node)->size;
                    
                    (*node)->size = 0UL;
                    (*node)->next->prev = (*node)->prev;
                    (*node)->prev->next = (*node)->next;
                    
                    mempool->freeList.len--;
                    node = &mempool->freeList.head;
                }
                else if ((*node)->prev != NULL && (*node)->next != NULL && (uintptr_t)*node - (*node)->prev->size == (uintptr_t)(*node)->prev)
                {
                    // Prev node is at a lower address.
                    (*node)->prev->size += (*node)->size;
                    
                    (*node)->size = 0UL;
                    (*node)->next->prev = (*node)->prev;
                    (*node)->prev->next = (*node)->next;
                    
                    mempool->freeList.len--;
                    node = &mempool->freeList.head;
                } 
                else
                {
                    node = &(*node)->next;
                }
            }
            
            return PRE_DEFRAG_LEN > mempool->freeList.len;
        }
    }
}

void ToggleMemPoolAutoDefrag(MemPool *const mempool)
{
    if (mempool == NULL) return;
    else mempool->freeList.autoDefrag ^= true;
}

//----------------------------------------------------------------------------------
// Module Functions Definition - Object Pool
//----------------------------------------------------------------------------------
union ObjInfo {
    uint8_t *const byte;
    size_t *const size;
};

ObjPool CreateObjPool(const size_t objsize, const size_t len)
{
    ObjPool objpool = { 0 };
    
    if ((len == 0UL) || (objsize == 0UL)) return objpool;
    else
    {
        objpool.objSize = __AlignSize(objsize, sizeof(size_t));
        objpool.stack.size = objpool.freeBlocks = len;
        objpool.stack.mem = calloc(objpool.stack.size, objpool.objSize);
        
        if (objpool.stack.mem == NULL)
        {
            objpool.stack.size = 0UL;
            return objpool;
        } 
        else
        {
            for (size_t i=0; i<objpool.freeBlocks; i++)
            {
                union ObjInfo block = { .byte = &objpool.stack.mem[i*objpool.objSize] };
                *block.size = i + 1;
            }
            
            objpool.stack.base = objpool.stack.mem;
            return objpool;
        }
    }
}

ObjPool CreateObjPoolFromBuffer(void *const buf, const size_t objsize, const size_t len)
{
    ObjPool objpool = { 0 };
    
    // If the object size isn't large enough to align to a size_t, then we can't use it.
    if ((buf == NULL) || (len == 0UL) || (objsize < sizeof(size_t)) || (objsize*len != __AlignSize(objsize, sizeof(size_t))*len)) return objpool;
    else
    {
        objpool.objSize = __AlignSize(objsize, sizeof(size_t));
        objpool.stack.size = objpool.freeBlocks = len;
        objpool.stack.mem = buf;
        
        for (size_t i=0; i<objpool.freeBlocks; i++)
        {
            union ObjInfo block = { .byte = &objpool.stack.mem[i*objpool.objSize] };
            *block.size = i + 1;
        }
        
        objpool.stack.base = objpool.stack.mem;
        return objpool;
    }
}

void DestroyObjPool(ObjPool *const objpool)
{
    if ((objpool == NULL) || (objpool->stack.mem == NULL)) return;
    else
    {
        free(objpool->stack.mem);
        *objpool = (ObjPool){0};
    }
}

void *ObjPoolAlloc(ObjPool *const objpool)
{
    if (objpool == NULL) return NULL;
    else
    {
        if (objpool->freeBlocks > 0UL)
        {
            // For first allocation, head points to the very first index.
            // Head = &pool[0];
            // ret = Head == ret = &pool[0];
            union ObjInfo ret = { .byte = objpool->stack.base };
            objpool->freeBlocks--;
            
            // after allocating, we set head to the address of the index that *Head holds.
            // Head = &pool[*Head * pool.objsize];
            objpool->stack.base = (objpool->freeBlocks != 0UL)? objpool->stack.mem + (*ret.size*objpool->objSize) : NULL;
            memset(ret.byte, 0, objpool->objSize);
            return ret.byte;
        }
        else return NULL;
    }
}

void ObjPoolFree(ObjPool *const restrict objpool, void *ptr)
{
    union ObjInfo p = { .byte = ptr };
    if ((objpool == NULL) || (ptr == NULL) || (p.byte < objpool->stack.mem) || (p.byte > objpool->stack.mem + objpool->stack.size*objpool->objSize)) return;
    else
    {
        // When we free our pointer, we recycle the pointer space to store the previous index and then we push it as our new head.
        // *p = index of Head in relation to the buffer;
        // Head = p;
        *p.size = (objpool->stack.base != NULL)? (objpool->stack.base - objpool->stack.mem)/objpool->objSize : objpool->stack.size;
        objpool->stack.base = p.byte;
        objpool->freeBlocks++;
    }
}

void ObjPoolCleanUp(ObjPool *const restrict objpool, void **ptrref)
{
    if ((objpool == NULL) || (ptrref == NULL) || (*ptrref == NULL)) return;
    else
    {
        ObjPoolFree(objpool, *ptrref);
        *ptrref = NULL;
    }
}

#endif  // RMEM_IMPLEMENTATION