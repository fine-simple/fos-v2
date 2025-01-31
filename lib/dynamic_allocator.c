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
	// TODO: [PROJECT MS1 - BONUS] [DYNAMIC ALLOCATOR] alloc_block_NF
	//  Write your code here, remove the panic and write your code
	static struct MemBlock *lastAllocated;
	if(lastAllocated == NULL)
		lastAllocated = LIST_FIRST(&FreeMemBlocksList);

	struct MemBlock *it = lastAllocated;
	while(LIST_NEXT(it) != lastAllocated)
	{
		if (it->size == size)
		{
			lastAllocated = LIST_NEXT(it);
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
			lastAllocated = it;
			LIST_REMOVE(&AvailableMemBlocksList, newBlock);
			return newBlock;
		}

		if(LIST_NEXT(it) == NULL)
		{
			if(LIST_FIRST(&FreeMemBlocksList) == lastAllocated)
				break;
			else
				it = LIST_FIRST(&FreeMemBlocksList);
		}
		else
			it = LIST_NEXT(it);
	}

	return NULL;
}

//===================================================
// [8] INSERT BLOCK (SORTED WITH MERGE) IN FREE LIST:
//===================================================
void insert_sorted_with_merge_freeList(struct MemBlock *blockToInsert)
{
	struct MemBlock *element;
	struct MemBlock *temp = NULL;

	if (FreeMemBlocksList.size == 0)
	{
		LIST_INSERT_HEAD(&FreeMemBlocksList, blockToInsert);
	}
	else
	{
		LIST_FOREACH(element, &FreeMemBlocksList)
		{
			if (element->sva < blockToInsert->sva)
			{
				temp = element;
			}
			else
			{
				break;
			}
		}

		if (temp == NULL)
		{
			uint32 endOfBlock = blockToInsert->sva + blockToInsert->size;
			struct MemBlock *newBlock = LIST_FIRST(&FreeMemBlocksList);
			if (endOfBlock == newBlock->sva)
			{
				newBlock->sva = blockToInsert->sva;
				newBlock->size = blockToInsert->size + newBlock->size;
				blockToInsert->size = 0;
				blockToInsert->sva = 0;
				LIST_INSERT_HEAD(&AvailableMemBlocksList, blockToInsert);
			}
			else
			{
				LIST_INSERT_HEAD(&FreeMemBlocksList, blockToInsert);
			}
		}
		else if (temp->prev_next_info.le_next != NULL)
		{
			struct MemBlock *after = temp->prev_next_info.le_next;
			uint32 endOfPrev = temp->sva + temp->size;
			uint32 endOfNext = after->sva + after->size;
			uint32 endOfBlock = blockToInsert->sva + blockToInsert->size;
			if (blockToInsert->sva == endOfPrev && endOfBlock == after->sva)
			{
				temp->size = temp->size + blockToInsert->size + after->size;
				LIST_REMOVE(&FreeMemBlocksList, after);
				blockToInsert->sva = 0;
				blockToInsert->size = 0;
				LIST_INSERT_HEAD(&AvailableMemBlocksList, blockToInsert);
				after->sva = 0;
				after->size = 0;
				LIST_INSERT_HEAD(&AvailableMemBlocksList, after);
			}
			else if (blockToInsert->sva == endOfPrev)
			{
				temp->size += blockToInsert->size;
				blockToInsert->sva = 0;
				blockToInsert->size = 0;
				LIST_INSERT_HEAD(&AvailableMemBlocksList, blockToInsert);
			}
			else if (endOfBlock == after->sva)
			{
				after->sva = blockToInsert->sva;
				after->size += blockToInsert->size;
				blockToInsert->sva = 0;
				blockToInsert->size = 0;
				LIST_INSERT_HEAD(&AvailableMemBlocksList, blockToInsert);
			}
			else
			{
				after->prev_next_info.le_prev = blockToInsert;
				blockToInsert->prev_next_info.le_next = after;
				blockToInsert->prev_next_info.le_prev = temp;
				temp->prev_next_info.le_next = blockToInsert;
				FreeMemBlocksList.size++;
			}
		}
		else
		{
			uint32 endOfPrev = temp->sva + temp->size;

			if (blockToInsert->sva == endOfPrev)
			{
				temp->size += blockToInsert->size;
				blockToInsert->sva = 0;
				blockToInsert->size = 0;
				LIST_INSERT_HEAD(&AvailableMemBlocksList, blockToInsert);
			}
			else
			{
				LIST_INSERT_TAIL(&FreeMemBlocksList, blockToInsert);
			}
		}
	}
}
