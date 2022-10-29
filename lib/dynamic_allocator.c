/*
 * dyn_block_management.c
 *
 *  Created on: Sep 21, 2022
 *      Author: HP
 */
#include <inc/assert.h>
#include <inc/string.h>
#include "../inc/dynamic_allocator.h"


//==================================================================================//
//============================== GIVEN FUNCTIONS ===================================//
//==================================================================================//

//===========================
// PRINT MEM BLOCK LISTS:
//===========================

void print_mem_block_lists()
{
	cprintf("\n=========================================\n");
	struct MemBlock* blk ;
	struct MemBlock* lastBlk = NULL ;
	cprintf("\nFreeMemBlocksList:\n");
	uint8 sorted = 1 ;
	LIST_FOREACH(blk, &FreeMemBlocksList)
	{
		if (lastBlk && blk->sva < lastBlk->sva + lastBlk->size)
			sorted = 0 ;
		cprintf("[%x, %x)-->", blk->sva, blk->sva + blk->size) ;
		lastBlk = blk;
	}
	if (!sorted)	cprintf("\nFreeMemBlocksList is NOT SORTED!!\n") ;

	lastBlk = NULL ;
	cprintf("\nAllocMemBlocksList:\n");
	sorted = 1 ;
	LIST_FOREACH(blk, &AllocMemBlocksList)
	{
		if (lastBlk && blk->sva < lastBlk->sva + lastBlk->size)
			sorted = 0 ;
		cprintf("[%x, %x)-->", blk->sva, blk->sva + blk->size) ;
		lastBlk = blk;
	}
	if (!sorted)	cprintf("\nAllocMemBlocksList is NOT SORTED!!\n") ;
	cprintf("\n=========================================\n");

}

//********************************************************************************//
//********************************************************************************//

//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//

//===============================
// [1] INITIALIZE AVAILABLE LIST:
//===============================
void initialize_MemBlocksList(uint32 numOfBlocks)
{
	// TODO: [PROJECT MS1] [DYNAMIC ALLOCATOR] initialize_MemBlocksList
	//  Write your code here, remove the panic and write your code
	LIST_INIT(&AvailableMemBlocksList);
	for (int i = 0; i < numOfBlocks; i++)
	{
		LIST_INSERT_HEAD(&AvailableMemBlocksList, &(MemBlockNodes[i]));
	}
}

//===============================
// [2] FIND BLOCK:
//===============================
struct MemBlock *find_block(struct MemBlock_List *blockList, uint32 va)
{
	// TODO: [PROJECT MS1] [DYNAMIC ALLOCATOR] find_block
	struct MemBlock *it;
	LIST_FOREACH(it, blockList)
	{
		if (it->sva == va)
			return it;
	}
	return NULL;
}

//=========================================
// [3] INSERT BLOCK IN ALLOC LIST [SORTED]:
//=========================================
void insert_sorted_allocList(struct MemBlock *blockToInsert)
{
	// TODO: [PROJECT MS1] [DYNAMIC ALLOCATOR] insert_sorted_allocList
	//  Write your code here, remove the panic and write your code
	if (LIST_SIZE(&AllocMemBlocksList) == 0)
	{
		LIST_INSERT_HEAD(&AllocMemBlocksList, blockToInsert);
		return;
	}

	struct MemBlock *it;
	LIST_FOREACH(it, &AllocMemBlocksList)
	{
		if (blockToInsert->sva < it->sva)
		{
			LIST_INSERT_BEFORE(&AllocMemBlocksList, it, blockToInsert);
			return;
		}
	}

	LIST_INSERT_TAIL(&AllocMemBlocksList, blockToInsert);
}

//=========================================
// [4] ALLOCATE BLOCK BY FIRST FIT:
//=========================================
struct MemBlock *alloc_block_FF(uint32 size)
{
	// TODO: [PROJECT MS1] [DYNAMIC ALLOCATOR] alloc_block_FF
	struct MemBlock *it;
	LIST_FOREACH(it, &FreeMemBlocksList)
	{
		if (it->size == size)
		{
			LIST_REMOVE(&FreeMemBlocksList, it);
			return it;
		}
		else if (it->size > size)
		{
			struct MemBlock *newBlock = LIST_FIRST(&AvailableMemBlocksList);
			newBlock->size = size;
			newBlock->sva = it->sva;
			it->size -= size;
			it->sva += size;
			LIST_REMOVE(&AvailableMemBlocksList, newBlock);
			return newBlock;
		}
	}
	return NULL;
}

//=========================================
// [5] ALLOCATE BLOCK BY BEST FIT:
//=========================================
struct MemBlock *alloc_block_BF(uint32 size)
{
	// TODO: [PROJECT MS1] [DYNAMIC ALLOCATOR] alloc_block_BF
	//  Write your code here, remove the panic and write your code
	struct MemBlock *minBlock = NULL;
	struct MemBlock *it;
	LIST_FOREACH(it, &FreeMemBlocksList)
	{
		if (it->size >= size)
		{
			if (minBlock == NULL)
				minBlock = it;
			else if (minBlock->size > it->size)
				minBlock = it;
		}
	}

	if (minBlock != NULL)
	{
		if(minBlock->size == size)
		{
			LIST_REMOVE(&FreeMemBlocksList, minBlock);
			return minBlock;
		}
		else
		{
			struct MemBlock *newBlock = LIST_FIRST(&AvailableMemBlocksList);
			newBlock->size = size;
			newBlock->sva = minBlock->sva;
			minBlock->size -= size;
			minBlock->sva += size;
			LIST_REMOVE(&AvailableMemBlocksList, newBlock);
			return newBlock;
		}
	}

	return NULL;
}

//=========================================
// [7] ALLOCATE BLOCK BY NEXT FIT:
//=========================================
struct MemBlock *alloc_block_NF(uint32 size)
{
	//TODO: [PROJECT MS1 - BONUS] [DYNAMIC ALLOCATOR] alloc_block_NF
	// Write your code here, remove the panic and write your code
	panic("alloc_block_NF() is not implemented yet...!!");

}

//===================================================
// [8] INSERT BLOCK (SORTED WITH MERGE) IN FREE LIST:
//===================================================
void insert_sorted_with_merge_freeList(struct MemBlock *blockToInsert)
{
	//cprintf("BEFORE INSERT with MERGE: insert [%x, %x)\n=====================\n", blockToInsert->sva, blockToInsert->sva + blockToInsert->size);
	//print_mem_block_lists() ;

	//TODO: [PROJECT MS1] [DYNAMIC ALLOCATOR] insert_sorted_with_merge_freeList
	// Write your code here, remove the panic and write your code
	panic("insert_sorted_with_merge_freeList() is not implemented yet...!!");



	//cprintf("\nAFTER INSERT with MERGE:\n=====================\n");
	//print_mem_block_lists();

}

