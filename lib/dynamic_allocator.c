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
	//TODO: [PROJECT MS1] [DYNAMIC ALLOCATOR] initialize_MemBlocksList
	// Write your code here, remove the panic and write your code
	//panic("initialize_MemBlocksList() is not implemented yet...!!");

	 LIST_INIT(& AvailableMemBlocksList);


	 for(uint32 i=0 ; i<numOfBlocks ; i++){
	   LIST_INSERT_HEAD(&AvailableMemBlocksList, &(MemBlockNodes[i]));
	 }


}

//===============================
// [2] FIND BLOCK:
//===============================
struct MemBlock *find_block(struct MemBlock_List *blockList, uint32 va)
{
	//TODO: [PROJECT MS1] [DYNAMIC ALLOCATOR] find_block
	// Write your code here, remove the panic and write your code
	//panic("find_block() is not implemented yet...!!");

	struct MemBlock *element;
	LIST_FOREACH(element, &(*blockList))
	{
		if(element->sva == va){
			 		return element;
			 	}
	}

	return NULL;
}


//=========================================
// [3] INSERT BLOCK IN ALLOC LIST [SORTED]:
//=========================================
void insert_sorted_allocList(struct MemBlock *blockToInsert)
{
	//TODO: [PROJECT MS1] [DYNAMIC ALLOCATOR] insert_sorted_allocList
	// Write your code here, remove the panic and write your code
	//panic("insert_sorted_allocList() is not implemented yet...!!");
	  struct MemBlock *element;
	  struct MemBlock* lastElement = LIST_LAST(&AllocMemBlocksList);
	  int size=LIST_SIZE(&AllocMemBlocksList);
	  if((size==0)||(blockToInsert->sva==0)){
	    LIST_INSERT_HEAD(&AllocMemBlocksList,blockToInsert);

	  }
	  else if((lastElement->sva)<(blockToInsert->sva)){
	    LIST_INSERT_TAIL(&AllocMemBlocksList, blockToInsert);
	  }
	   else if((lastElement->sva)==(blockToInsert->sva)){
	    LIST_INSERT_AFTER(&AllocMemBlocksList,lastElement,blockToInsert);
	  }
	  else{
	      struct MemBlock* nextElement ;
	      LIST_FOREACH(element,&(AllocMemBlocksList)){
			  nextElement = element->prev_next_info.le_next;//LIST_NEXT(element);
			  if((blockToInsert->sva)>(element->sva)&&((blockToInsert->sva)<(nextElement->sva))){
				LIST_INSERT_AFTER(&AllocMemBlocksList,element,blockToInsert);
				break;
			  }
	    }
	  }

}

//=========================================
// [4] ALLOCATE BLOCK BY FIRST FIT:
//=========================================
struct MemBlock *alloc_block_FF(uint32 size)
{
	//TODO: [PROJECT MS1] [DYNAMIC ALLOCATOR] alloc_block_FF
	// Write your code here, remove the panic and write your code
	//panic("alloc_block_FF() is not implemented yet...!!");

	  struct MemBlock *temp;
	  struct MemBlock *element;
	  LIST_FOREACH(element, &(FreeMemBlocksList)){
	  if((element->size)==(size)){
			  temp=element;
			  LIST_REMOVE(&(FreeMemBlocksList) ,element);
			  return temp;
		}
	   else if ((element->size)>(size)){
		  uint32 new_size = (element->size)-(size);
		  // get new block from AvailableMemBlocksList
		  struct MemBlock *new_block=LIST_FIRST(&AvailableMemBlocksList);
		  LIST_REMOVE(&AvailableMemBlocksList ,new_block);
		  LIST_INSERT_TAIL(&(FreeMemBlocksList) ,new_block);
		  // setting the size & sva
		  new_block->size=size;
		  new_block->sva=element->sva;
		  //update size in list
		  element->size=new_size;
		  element->sva=(element->sva)+ size;
		  temp=new_block;
		  LIST_REMOVE(&(FreeMemBlocksList) ,new_block);
		  return temp;
		}
	  }
	  return NULL;
}

//=========================================
// [5] ALLOCATE BLOCK BY BEST FIT:
//=========================================
struct MemBlock *alloc_block_BF(uint32 size)
{
	//TODO: [PROJECT MS1] [DYNAMIC ALLOCATOR] alloc_block_BF
	// Write your code here, remove the panic and write your code
	//panic("alloc_block_BF() is not implemented yet...!!");

	 int suitable = 0;

	 uint32 big_size= (4294967295);//(4*1024*1024*1024)-1
	 struct MemBlock *temp;
	 struct MemBlock *ptr;//= LIST_FIRST(&(FreeMemBlocksList));
	// new var as intial size
	 struct MemBlock *element;
	 LIST_FOREACH(element, &(FreeMemBlocksList))
	 {
	  if ((element->size) == (size)) {
	   temp = element;
	   LIST_REMOVE(&(FreeMemBlocksList), element);
	   return temp;
	  }
	  else if ((element->size) > (size))
	  {
	   if (element->size < big_size  )
	   { // new var
	    //update new var
	    big_size=element->size;
	    ptr = element;
	    suitable =1;
	   }
	  }
	 }

	 if (suitable == 1)
	 {
	  uint32 new_size = (ptr->size) - (size);
	  // get new block from AvailableMemBlocksList
	  struct MemBlock *new_block = LIST_FIRST(&AvailableMemBlocksList);
	  LIST_REMOVE(&AvailableMemBlocksList, new_block);
	  // setting the size & sva
	  new_block->size = size;
	  new_block->sva = ptr->sva;
	  //update size in list
	  ptr->size = new_size;
	  ptr->sva += size;
	  temp = new_block;
	  return temp;
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

/*	struct MemBlock *last_alloc = LIST_FIRST(&FreeMemBlocksList);
	struct MemBlock *alloc_block_NF(uint32 size)
	{
	 struct MemBlock *temp;
	 uint32 new_size = (last_alloc->size) - (size);
	 if (size == last_alloc->size ) {
	  temp = last_alloc;
	  LIST_REMOVE(&(FreeMemBlocksList), temp);
	  last_alloc = LIST_NEXT(last_alloc);
	  return temp;
	 }
	 else if (size < last_alloc->size) {
	  // get new block from AvailableMemBlocksList
	  struct MemBlock *new_block = LIST_FIRST(&AvailableMemBlocksList);
	  LIST_REMOVE(&AvailableMemBlocksList, new_block);
	  // setting the size & sva
	  new_block->size = size;
	  new_block->sva = last_alloc->sva;
	  //update size in list
	  last_alloc->size = new_size;
	  last_alloc->sva += size;
	  temp = new_block;
	  last_alloc = LIST_NEXT(last_alloc);
	  return temp;
	 }
	 else if (size > last_alloc->size) {

	  struct MemBlock *element = last_alloc;
	  LIST_FOREACH(element, &(FreeMemBlocksList))
	  {
	   if ((element->size) > (size)) {
	    // get new block from AvailableMemBlocksList
	    struct MemBlock *new_block = LIST_FIRST(
	      &AvailableMemBlocksList);
	    LIST_REMOVE(&AvailableMemBlocksList, new_block);
	    // setting the size & sva
	    new_block->size = size;
	    new_block->sva = last_alloc->sva;
	    //update size in list
	    last_alloc->size = new_size;
	    last_alloc->sva += size;
	    temp = new_block;
	    last_alloc = LIST_NEXT(last_alloc);
	    return temp;
	   }

	  }
	 }
	  //return 0;
	}
	 return 0;

*/
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
	//panic("insert_sorted_with_merge_freeList() is not implemented yet...!!");
	  struct MemBlock* lastElement = LIST_LAST(&FreeMemBlocksList);
	  struct MemBlock* firstElement = LIST_FIRST(&FreeMemBlocksList);
	  struct MemBlock *element;
	  int size=LIST_SIZE(&FreeMemBlocksList);
	  //(LIST_EMPTY(&FreeMemBlocksList))
	  //empty list
	  if(size==0){
	    LIST_INSERT_HEAD(&FreeMemBlocksList,blockToInsert);
	  }
	  //insert blockToInsert before first block without merge
	  else if((lastElement->sva + lastElement->size)<(blockToInsert->sva)){
	    LIST_INSERT_TAIL(&FreeMemBlocksList, blockToInsert);
	  }
	  //merge with last block
	  else if((lastElement->sva + lastElement->size)==(blockToInsert->sva)){
	    lastElement->size+=blockToInsert->size;
	    blockToInsert->sva=0;
	    blockToInsert->size=0;
	    LIST_INSERT_HEAD(&AvailableMemBlocksList, blockToInsert);
	  }
	  //insert blockToInsert before first block without merge
	  else if((blockToInsert->sva +blockToInsert->size)<(firstElement->sva)){
	      LIST_INSERT_HEAD(&FreeMemBlocksList,blockToInsert);
	  }
	  else if((blockToInsert->sva + blockToInsert->size)==(firstElement->sva)){
	     //merge with first block
	    firstElement->size+=blockToInsert->size;
	    firstElement->sva=blockToInsert->sva;
	     blockToInsert->sva=0;
	     blockToInsert->size=0;
	     LIST_INSERT_HEAD(&AvailableMemBlocksList, blockToInsert);
	  }
	  else{
	    //struct MemBlock* nextElement ;
	      LIST_FOREACH(element,&(FreeMemBlocksList)){
	      struct MemBlock* nextElement =LIST_NEXT(element); //element->prev_next_info.le_next;
	      //merge with previous and next
	      if((element->sva+element->size)==(blockToInsert->sva)&&(blockToInsert->sva + blockToInsert->size)==(nextElement->sva)){
	        //merge element and blockToInsert and nextElement
	        element->size += blockToInsert->size +nextElement->size;
	        blockToInsert->sva=0;
	        blockToInsert->size=0;
	        nextElement->size=0;
	        nextElement->sva=0;
	        LIST_REMOVE(&FreeMemBlocksList, nextElement);
	        LIST_INSERT_HEAD(&AvailableMemBlocksList, blockToInsert);
	        LIST_INSERT_HEAD(&AvailableMemBlocksList, nextElement);
	        break;
	    }
	      //merge with next
	    else if((element->sva+element->size)<(blockToInsert->sva)&&(blockToInsert->sva + blockToInsert->size)==(nextElement->sva)){
	      //merge nextElement and blockToInsert
	        nextElement->size += blockToInsert->size;
	        nextElement->sva=blockToInsert->sva;
	        blockToInsert->sva=0;
	        blockToInsert->size=0;
	        LIST_INSERT_HEAD(&AvailableMemBlocksList, blockToInsert);
	        break;
	      }
	      //merge with previous
	    else if((element->sva+element->size)==(blockToInsert->sva)&&(blockToInsert->sva + blockToInsert->size)<(nextElement->sva)){
	      //merge element and blockToInsert
	      element->size +=blockToInsert->size;
	      blockToInsert->sva=0;
	      blockToInsert->size=0;
	      LIST_INSERT_HEAD(&AvailableMemBlocksList, blockToInsert);
	      break;
	    }
	      //insert blockToInsert between to blocks without merge
	    else if((element->sva+element->size)<(blockToInsert->sva)&&(blockToInsert->sva + blockToInsert->size)<(nextElement->sva)){
	      LIST_INSERT_AFTER(&FreeMemBlocksList,element,blockToInsert);
	      break;
	    }
	  }
	  }

	  //cprintf("\nAFTER INSERT with MERGE:\n=====================\n");
	 // print_mem_block_lists();





}

