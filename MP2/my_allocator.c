/*
    File: my_allocator.c

    Author: Alejandro Suazo and Dimas Gonzales
            Department of Computer Science
            Texas A&M University
    Date  : 10/7/15

    Modified:

    This file contains the implementation of the module "MY_ALLOCATOR".

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "my_allocator.h"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

struct node{
    	unsigned int size;
    	bool is_free;
    	bool is_left;
    	struct node* next;
    };

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

Addr memory_start_address;
unsigned int memory_size;
unsigned int block_size;
struct node** free_list;
int free_list_size;

/*--------------------------------------------------------------------------*/
/* FUNCTIONS FOR MODULE MY_ALLOCATOR */
/*--------------------------------------------------------------------------*/

// ------------------- MATH Logic -------------------
double log_base_2( double n )
{
    return log( n ) / log( 2 );
}

unsigned int get_block_size(unsigned int v)
{ //We perform a bitwise or operations to get the correct block size for the given input.
	v--;
	v = v | v >> 1;
	v = v | v >> 2;
	v = v | v >> 4;
	v = v | v >> 8;
	v = v | v >> 16;
	v++;
	return v;
}

// ------------------ List Helpers ---------------
int add_node(struct node* new_node,int position)
{
	struct node* temp = free_list[position];
	struct node* prev = NULL;

	if(temp == NULL)
  { //Exit and return the first position subce the free list is most likely empty
		free_list[position] = new_node;
		new_node->size = (memory_size/pow(2,position));
		new_node->is_free = true;
		new_node->next = NULL;
		return 0;
	}

	while(temp < new_node && temp->next != NULL)
  { //Look for position to insert new node.
		prev = temp;
		temp = temp->next;
	}

	if(prev == NULL)
  { // Check if prev node is null to know what side to begin insertion
		if(temp > new_node)
    { // Check if the address of temp is after new node
			free_list[position] = new_node;
			free_list[position]->size = (memory_size / pow(2,position));
			free_list[position]->is_free = true;
			free_list[position]->next = temp;
		}
		else
    {
			free_list[position]->next = new_node;
			free_list[position]->next->size = (memory_size / pow(2,position));
			free_list[position]->next->is_free = true;
			free_list[position]->next->next = NULL;
		}
	}
	else
  {
		if(temp < new_node && temp->next == NULL)
    { // Check for the end of the list
			temp->next = new_node;
			temp->next->is_free = true;
			temp->next->size = (memory_size/pow(2,position));
			temp->next->next = NULL;

		}
		else
    { // if the temp address is after the new node then insert between prev and temp
  		prev->next = new_node;
  		prev->next->size = (memory_size/pow(2,position));
  		prev->next->is_free = true;
  		prev->next->next = temp;
		}
	}
	return 0;
}

int split_node (int position)
{ // This function breaks down large memory blocks to smaller memory blocks to reduce fragmentation.
	struct node* temp = free_list[position];
	struct node* prev = NULL;
  int returnVal = 1; //Fail by default

	while(temp != NULL && !temp->is_free)
  { // Find node
		prev = temp;
		temp = temp->next;
	}

	if(temp != NULL)
  { // If we are at the end of list then return fail case
  	if(prev == NULL)
    {
  		free_list[position] = temp->next;
  	}
  	else
    {
  		prev->next = temp->next;
  	}

    //Make the left node
  	temp->is_left = true;
  	add_node(temp,position+1);

    //Make the rignt node
  	struct node* right = (Addr)temp + (int)(memory_size/pow(2,position+1));
  	right->is_left = false;
  	add_node(right,position+1);
  	returnVal = 0;
  }

  return returnVal;
}

int combine_nodes()
{ //This is a cleanup for the free_list to ensure that nodes that can be combined are merged.
	for(int i = free_list_size - 1; i > -1; --i){
		struct node* temp = free_list[i];
		struct node* two_nodes_ahead = NULL;
		struct node* prev = NULL;

		while(temp != NULL){
			if(temp->next == NULL)
      { // Exit at the end of the list, DO NOT PUT THIS CHECK INTO THE WHILE LOOP CONDITIONAL
				break;
      }

      //Look two nodes ahead since we will be removing the next node.
			two_nodes_ahead = temp->next->next;
			if(temp->is_free && temp->next->is_free && temp->is_left && !temp->next->is_left)
      {
				if(prev == NULL)
        { // At start of list, so move down two entries.
					free_list[i] = two_nodes_ahead;
        }
				else
        { // Save the next node.
					prev->next = two_nodes_ahead;
        }

				add_node(temp,i-1);
				temp = two_nodes_ahead;
			}
			else
      {
				prev = temp;
				temp = temp->next;
			}
		}
	}
	return 0;
}

void print_free_nodes()
{
	for(int i = 0;i < free_list_size; ++i)
  { //I terate through each list
		struct node* temp = free_list[i];
    if (temp != NULL)
    { //Get list position
      printf("List Position: %d \n",i);
    }

		while(temp != NULL)
    { // Print Size of the list, number of the free nodes, and the starting address of the list
			printf("List Size: %u Free Nodes: %d Address:%p \n",temp->size,temp->is_free,temp);
      printf("-------------------------------- \n");
			temp = temp->next;
		}
	}
	printf("\n");
}

Addr get_free_node(int index)
{
	struct node* temp = free_list[index];
  Addr return_addr = NULL;

	while(temp != NULL && !temp->is_free)
  { // iterate through the list until a free node is found.
		temp = temp->next;
	}

	if(temp != NULL)
  { // if the node is not null, save the address to return_addr and update node state.
		temp->is_free = false;
		return_addr = (Addr)temp+sizeof(*temp);
	}

		return return_addr;
}

// ---------------- Allocator ----------------
unsigned int init_allocator(unsigned int _basic_block_size,unsigned int _length)
{
	memory_size = get_block_size(_length);
	if(memory_size == _length * 2)
  {
		memory_size = memory_size/2;
  }

  // Program memory allocation
	memory_start_address = malloc(memory_size + 100000);
	block_size = get_block_size(_basic_block_size);
	free_list_size = 1;

  unsigned int size = memory_size;
  while(size > block_size)
  { //Find the number of blocks
		free_list_size++;
		size = size/2;
	}

  // Allocate memory for our free list and initialize the list
	free_list = malloc(free_list_size * sizeof(struct node *));
	free_list[0] = (struct node *)memory_start_address;
	free_list[0]->size = memory_size;
	free_list[0]->is_free = true;
	free_list[0]->next = NULL;

  // Assign null to each list to complete initialization.
	for(int i = 1; i < free_list_size; ++i)
  {
		free_list[i] = NULL;
	}

	return memory_size;
}

int release_allocator()
{ //This frees the whole program memory and the free list.
	free(memory_start_address);
	free(free_list);
	return 0;
}


extern Addr my_malloc(unsigned int _length)
{
  combine_nodes(); // This cleans up the lists before we allocate anything.

  // Get alloc_size without header information
  int alloc_size = get_block_size(_length);
	if(alloc_size  == _length*2)
  {
		alloc_size = memory_size/2;
  }

  // Get the alloc_size after including the header size
	alloc_size = get_block_size(alloc_size + sizeof(struct node));

	int index = log_base_2(memory_size / alloc_size);
	int offset = 0;
	Addr return_addr = NULL;
	while(return_addr == NULL)
  {
  	return_addr = get_free_node(index);
  	if(return_addr != NULL || offset == index)
    { //Break when we have a node address or when the offset equals the index to avoid seg faults by out-of-range errors.
  		break;
  	}

  	split_node(offset);
  	offset++;

  	if(offset >= free_list_size)
    {
  		break;
  	}
	}

  return return_addr;
  //return malloc((size_t)_length);
}

extern int my_free(Addr _a)
{
  int returnVal = 0;
  if(_a != NULL)
  {
    struct node* temp = (_a - sizeof(struct node));
    temp->is_free = true;
  }
  else
  {
    returnVal = 1;
  }

  return returnVal;
}
