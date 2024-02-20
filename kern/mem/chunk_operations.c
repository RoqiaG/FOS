/*
 * chunk_operations.c
 *
 *  Created on: Oct 12, 2022
 *      Author: HP
 */

#include <kern/trap/fault_handler.h>
#include <kern/disk/pagefile_manager.h>
#include "kheap.h"
#include "memory_manager.h"


/******************************/
/*[1] RAM CHUNKS MANIPULATION */
/******************************/

//===============================
// 1) CUT-PASTE PAGES IN RAM:
//===============================
//This function should cut-paste the given number of pages from source_va to dest_va
//if the page table at any destination page in the range is not exist, it should create it
//Hint: use ROUNDDOWN/ROUNDUP macros to align the addresses
int cut_paste_pages(uint32* page_directory, uint32 source_va, uint32 dest_va, uint32 num_of_pages)
{
	//TODO: [PROJECT MS2] [CHUNK OPERATIONS] cut_paste_pages
	// Write your code here, remove the panic and write your code
	//panic("cut_paste_pages() is not implemented yet...!!");

	uint32 size=num_of_pages*PAGE_SIZE;

    uint32 newsource =ROUNDDOWN(source_va,PAGE_SIZE);
	uint32 newsizesrc =ROUNDUP(source_va+size,PAGE_SIZE);

    uint32 newdest =ROUNDDOWN(dest_va,PAGE_SIZE);
	uint32 newsizedest =ROUNDUP(dest_va+size,PAGE_SIZE);

	uint32 *ptr_page_table = NULL ;
	struct FrameInfo *ptr_frame_info_dest=NULL;
	struct FrameInfo *ptr_frame_info_src=NULL;

	for(uint32 i=newdest ; i<newsizedest ; i=i+PAGE_SIZE){
		get_page_table(page_directory,i,&ptr_page_table);
		ptr_frame_info_dest = get_frame_info(page_directory, i, (&ptr_page_table));
		if (ptr_frame_info_dest != NULL){ return -1 ;}//edited
	}

	for(uint32 i=newdest,j=newsource ; i<newsizedest||j<newsizesrc ; i=i+PAGE_SIZE ,j=j+PAGE_SIZE){

		get_page_table(page_directory,i,&ptr_page_table);
			if (ptr_page_table==NULL )
			{
				create_page_table(page_directory, i);
			}


			ptr_frame_info_src = get_frame_info(page_directory,j , (&ptr_page_table));

			int perms =pt_get_page_permissions(page_directory,j);

		// map to RAM
			map_frame(page_directory, ptr_frame_info_src, i, perms) ;

			unmap_frame(page_directory, j);


	}

	return 0 ;


}

//===============================
// 2) COPY-PASTE RANGE IN RAM:
//===============================
//This function should copy-paste the given size from source_va to dest_va
//if the page table at any destination page in the range is not exist, it should create it
//Hint: use ROUNDDOWN/ROUNDUP macros to align the addresses
int copy_paste_chunk(uint32* page_directory, uint32 source_va, uint32 dest_va, uint32 size)
{
	//TODO: [PROJECT MS2] [CHUNK OPERATIONS] copy_paste_chunk
	// Write your code here, remove the panic and write your code
	//panic("copy_paste_chunk() is not implemented yet...!!");

	uint32 newsource =source_va;
	uint32 newsizesrc =source_va+size;

	uint32 newdest =dest_va;
	uint32 newsizedest =dest_va+size;


	uint32 *ptr_page_table = NULL ;
	struct FrameInfo *ptr_frame_info_dest=NULL;
	struct FrameInfo *ptr_frame_info_src=NULL;

	for(uint32 i=newdest ; i< newsizedest; i=i+PAGE_SIZE){
	//if destination pages exist and read only deny process and return -1
	  get_page_table(page_directory,i,&ptr_page_table);
	  ptr_frame_info_dest = get_frame_info(page_directory, i, (&ptr_page_table));
	  int perms_dettt=ptr_page_table[PTX(i)] & (PERM_WRITEABLE);
	  if (ptr_frame_info_dest != NULL && perms_dettt != (PERM_WRITEABLE))
	  { return -1 ;}

	  }


	for(uint32 i=newdest,j=newsource ; i<newsizedest||j<newsizesrc ; i=i+PAGE_SIZE,j=j+PAGE_SIZE){
	  //if page table not exist create it
	   get_page_table(page_directory,i,&ptr_page_table);
	   if (ptr_page_table==NULL){create_page_table(page_directory, i);}


	   ptr_frame_info_dest = get_frame_info(page_directory, i, (&ptr_page_table));
		 //destination pages not exist create it
		 if (ptr_frame_info_dest == NULL){

		 ptr_frame_info_src = get_frame_info(page_directory,j , (&ptr_page_table));

		 allocate_frame(&ptr_frame_info_dest) ;

		int perms= ptr_page_table[PTX(j)] & 0x00000fff;
		map_frame(page_directory, ptr_frame_info_dest, i, perms | PERM_WRITEABLE) ;
		}
	}

	  uint8 *ptr_src = (uint8 *)(source_va) ;
	  uint8 *ptr_dest = (uint8 *) dest_va;

		   //copy and paste byte by byte
	  for(int k =0 ; k< size; k++)
	  {
		 *ptr_dest=*ptr_src;
		  ptr_src++ ;
		  ptr_dest++ ;
	  }

	return 0 ;

}

//===============================
// 3) SHARE RANGE IN RAM:
//===============================
//This function should share the given size from dest_va with the source_va
//Hint: use ROUNDDOWN/ROUNDUP macros to align the addresses
int share_chunk(uint32* page_directory, uint32 source_va,uint32 dest_va, uint32 size, uint32 perms)
{
	//TODO: [PROJECT MS2] [CHUNK OPERATIONS] share_chunk
	// Write your code here, remove the panic and write your code
	//panic("share_chunk() is not implemented yet...!!");

	    uint32 newsource =ROUNDDOWN(source_va,PAGE_SIZE);
		uint32 newsizesrc =ROUNDUP(source_va+size,PAGE_SIZE);

	    uint32 newdest =ROUNDDOWN(dest_va,PAGE_SIZE);
		uint32 newsizedest =ROUNDUP(dest_va+size,PAGE_SIZE);

		uint32 *ptr_page_table = NULL ;
		struct FrameInfo *ptr_frame_info_dest=NULL;
		struct FrameInfo *ptr_frame_info_src=NULL;

		for(uint32 i=newdest ; i<newsizedest ; i=i+PAGE_SIZE){
			get_page_table(page_directory,i,&ptr_page_table);
			ptr_frame_info_dest = get_frame_info(page_directory, i, (&ptr_page_table));
			if (ptr_frame_info_dest != NULL){ return -1 ;}//edited
		}

		for(uint32 i=newdest,j=newsource ; i<newsizedest||j<newsizesrc ; i=i+PAGE_SIZE ,j=j+PAGE_SIZE){

			get_page_table(page_directory,i,&ptr_page_table);
				if (ptr_page_table==NULL && newsizedest >= (4*1024*1024))
				{
					create_page_table(page_directory, i);
				}


				ptr_frame_info_src = get_frame_info(page_directory,j , (&ptr_page_table));

			// map to RAM
				map_frame(page_directory, ptr_frame_info_src, i, perms) ;

		}

		return 0 ;

}

//===============================
// 4) ALLOCATE CHUNK IN RAM:
//===============================
//This function should allocate in RAM the given range [va, va+size)
//Hint: use ROUNDDOWN/ROUNDUP macros to align the addresses
int allocate_chunk(uint32* page_directory, uint32 va, uint32 size, uint32 perms)
{
	//TODO: [PROJECT MS2] [CHUNK OPERATIONS] allocate_chunk
	// Write your code here, remove the panic and write your code
	//panic("allocate_chunk() is not implemented yet...!!");

	  uint32 newva =ROUNDDOWN(va,PAGE_SIZE);
	  uint32 newsize =ROUNDUP(va+size,PAGE_SIZE);
	  struct FrameInfo *ptr_frame_info;
	  uint32 *ptr_page_table = NULL ;
	  for(uint32 i=newva ; i<newsize ; i=i+PAGE_SIZE)
	  {
	    get_page_table(page_directory,i,&ptr_page_table);
	    ptr_frame_info = get_frame_info(page_directory, i, (&ptr_page_table));
	      if (ptr_frame_info != NULL){ return -1 ;}//edited

	  }

	  ptr_page_table=NULL;
	  for(uint32 i=newva ; i<(newsize) ; i=i+PAGE_SIZE)
	  {

	    get_page_table(page_directory,i,&ptr_page_table);
	    if (ptr_page_table==NULL && newsize>= 4*1024*1024 ){create_page_table(page_directory, i);}
	    // allocate frame
	    struct FrameInfo *ptr_frame_info2=NULL;
	    allocate_frame(&ptr_frame_info2) ;
	    // map to RAM
	    map_frame(page_directory, ptr_frame_info2, i, perms) ;
	    uint32 frame =to_frame_number(ptr_frame_info2);
	    frames_info[frame].va=i;
	  }

	  return 0 ;

}

/*BONUS*/
//=====================================
// 5) CALCULATE ALLOCATED SPACE IN RAM:
//=====================================
void calculate_allocated_space(uint32* page_directory, uint32 sva, uint32 eva, uint32 *num_tables, uint32 *num_pages)
{
	//TODO: [PROJECT MS2 - BONUS] [CHUNK OPERATIONS] calculate_allocated_space
	// Write your code here, remove the panic and write your code
	//panic("calculate_allocated_space() is not implemented yet...!!");

		uint32 newsva =ROUNDDOWN(sva,PAGE_SIZE);
		uint32 neweva =ROUNDUP(eva,PAGE_SIZE);

		uint32 *ptr_page_table = NULL ;
		struct FrameInfo *ptr_frame_info=NULL;

		for(uint32 i=newsva ; i<neweva ; i=i+PAGE_SIZE){
			get_page_table(page_directory,i,&ptr_page_table);
			//ptr_frame_info = get_frame_info(page_directory, i, (&ptr_page_table));
			//if (ptr_frame_info != NULL){num_tables++;}
			if (ptr_page_table!=NULL){num_tables++;}
		}


}

/*BONUS*/
//=====================================
// 6) CALCULATE REQUIRED FRAMES IN RAM:
//=====================================
// calculate_required_frames:
// calculates the new allocation size required for given address+size,
// we are not interested in knowing if pages or tables actually exist in memory or the page file,
// we are interested in knowing whether they are allocated or not.
uint32 calculate_required_frames(uint32* page_directory, uint32 sva, uint32 size)
{
	//TODO: [PROJECT MS2 - BONUS] [CHUNK OPERATIONS] calculate_required_frames
	// Write your code here, remove the panic and write your code
	//panic("calculate_required_frames() is not implemented yet...!!");

			uint32 newsva =ROUNDDOWN(sva,PAGE_SIZE);
			uint32 neweva =ROUNDUP(sva+size,PAGE_SIZE);

			uint32 num_tables=0;

			uint32 *ptr_page_table = NULL ;
			struct FrameInfo *ptr_frame_info=NULL;

			for(uint32 i=newsva ; i<neweva ; i=i+PAGE_SIZE){
				get_page_table(page_directory,i,&ptr_page_table);
				ptr_frame_info = get_frame_info(page_directory, i, (&ptr_page_table));
				if (ptr_frame_info != NULL){num_tables++;}
				//if (ptr_page_table==NULL){num_tables++;}
			}
			return num_tables;
}

//=================================================================================//
//===========================END RAM CHUNKS MANIPULATION ==========================//
//=================================================================================//

/*******************************/
/*[2] USER CHUNKS MANIPULATION */
/*******************************/

//======================================================
/// functions used for USER HEAP (malloc, free, ...)
//======================================================

//=====================================
// 1) ALLOCATE USER MEMORY:
//=====================================
void allocate_user_mem(struct Env* e, uint32 virtual_address, uint32 size)
{
	// Write your code here, remove the panic and write your code
	panic("allocate_user_mem() is not implemented yet...!!");
}

//=====================================
// 2) FREE USER MEMORY:
//=====================================
void free_user_mem(struct Env* e, uint32 virtual_address, uint32 size)
{
	//TODO: [PROJECT MS3] [USER HEAP - KERNEL SIDE] free_user_mem
	// Write your code here, remove the panic and write your code
	//panic("free_user_mem() is not implemented yet...!!");

		uint32 va = ROUNDDOWN(virtual_address, PAGE_SIZE);
		uint32 endaddress = ROUNDUP((size + virtual_address), PAGE_SIZE);

		for (uint32 var = va; var < endaddress; var += PAGE_SIZE) {

				int read = pf_read_env_page(e, (void*) var);

				if (read != E_PAGE_NOT_EXIST_IN_PF) {
					pf_remove_env_page(e, var);
				}

				for (int v = 0; v < e->page_WS_max_size ; v++) {
					uint32 t = env_page_ws_get_virtual_address(e, v);
					if (t>=va && t< endaddress) {
						env_page_ws_clear_entry(e, v);
						unmap_frame(e->env_page_directory,t );
						}
				}

		}
		uint32 *ptr_page_table2 = NULL;
			for (uint32 var = va; var < endaddress; var += PAGE_SIZE) {
			get_page_table(e->env_page_directory, var, &ptr_page_table2);
			if (ptr_page_table2!=NULL){
			int c = 0;
			for (int n = 0; n < 1024; n++) {
				//if (ptr_page_table2[n]  != 0)
				if((ptr_page_table2[n]&PERM_PRESENT)==PERM_PRESENT)
				{
					c = 1;
					break;
				}

			}
			if (c == 0) {
				pd_clear_page_dir_entry(e->env_page_directory,var);
				kfree(ptr_page_table2);

			}
		}
	}

			//This function should:
			//1. Free ALL pages of the given range from the Page File
			//2. Free ONLY pages that are resident in the working set from the memory
			//3. Removes ONLY the empty page tables (i.e. not used) (no pages are mapped in the table)


}

//=====================================
// 2) FREE USER MEMORY (BUFFERING):
//=====================================
void __free_user_mem_with_buffering(struct Env* e, uint32 virtual_address, uint32 size)
{
	// your code is here, remove the panic and write your code
	panic("__free_user_mem_with_buffering() is not implemented yet...!!");

	//This function should:
	//1. Free ALL pages of the given range from the Page File
	//2. Free ONLY pages that are resident in the working set from the memory
	//3. Free any BUFFERED pages in the given range
	//4. Removes ONLY the empty page tables (i.e. not used) (no pages are mapped in the table)
}

//=====================================
// 3) MOVE USER MEMORY:
//=====================================
void move_user_mem(struct Env* e, uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
{
	//TODO: [PROJECT MS3 - BONUS] [USER HEAP - KERNEL SIDE] move_user_mem
	//your code is here, remove the panic and write your code
	panic("move_user_mem() is not implemented yet...!!");

	// This function should move all pages from "src_virtual_address" to "dst_virtual_address"
	// with the given size
	// After finished, the src_virtual_address must no longer be accessed/exist in either page file
	// or main memory

	/**/
}

//=================================================================================//
//========================== END USER CHUNKS MANIPULATION =========================//
//=================================================================================//

