#pragma once
#include <malloc.h>
#include <stdio.h>
#define PAGE_SIZE 16384

typedef struct node
{
	short int start;
	int freeSize;
	node * next;
};

/*--------------------------------------------------Globals----------------------------------------------------------*/
void * pages[12];
node * linkedLists[12];
/*------------------------------------------------Globals End--------------------------------------------------------*/

void initMemory()
{
	for (int i = 0; i < 12; i++)
	{
		pages[i] = NULL;
		linkedLists[i] = NULL;
	}
}

void * azMalloc(short int size)
{
	if (size % 2 == 1)
		size++; 

	if (size > PAGE_SIZE - 2)
	{
		printf("\nToo Large to allocate\n");
		return NULL;
	}

	for (int i = 0; i < 12; i++)
	{
		if (pages[i] == NULL)
		{
			pages[i] = malloc(PAGE_SIZE);
			*((short int *)pages[i]) = size;
			node * freeBlock = (node *)malloc(sizeof(node));
			freeBlock->next = NULL;
			freeBlock->freeSize = PAGE_SIZE - (size + sizeof(short int));
			freeBlock->start = size + sizeof(short int);
			linkedLists[i] = freeBlock;
			short int * ptr = (short int *)pages[i];
			return (ptr + 1);
		}
		node * tmp = linkedLists[i];
		while (tmp != NULL)
		{
			if (tmp->freeSize >= size + 2)
			{
				tmp->freeSize -= (size + sizeof(short int));
				short int * ptr = (short int *)pages[i];
				*(ptr + tmp->start / 2) = size;
				void * x = (void *)(ptr + tmp->start / 2 + 1);
				tmp->start += (size + sizeof(short int));
				return x;
			}
			tmp = tmp->next;
		}
	}
	printf("\nStack overflow\n");
	return NULL;
}

int searchAddressInPages(void *address)
{
	for (int i = 0; i < 12; i++)
	{
		if (((short int *)address >(short int *)pages[i]) && ((short int *)address <= (short int *)pages[i] + 512))
			return i;
	}
	return -1;
}

void azFree(void *address)
{
	int ind = searchAddressInPages(address);
	if (ind == -1)
	{
		printf("\nInvlaid address\n");
		return;
	}
	short int * ptr1 = (short int*)address, *ptr2 = (short int*)(pages[ind]);
	short int size = *(ptr1 - 1) + 2;
	short int start = (ptr1 - ptr2 - 1) * 2;
	node * prev = linkedLists[ind];
	node * curr = prev->next;

	if (start + size < prev->start)
	{
		node * newNode = (node *)malloc(sizeof(node));
		newNode->freeSize = size;
		newNode->start = start;
		newNode->next = prev;
		linkedLists[ind] = newNode;
		return;
	}
	else if (start + size == prev->start)
	{
		prev->freeSize += size;
		if (prev->freeSize == PAGE_SIZE)
		{
			free(pages[ind]);
			pages[ind] = NULL;
			linkedLists[ind] = NULL;
			return;
		}
		prev->start = start;
		return;
	}
	while (curr != NULL)
	{
		if (prev->start + prev->freeSize == start && start + size == curr->start)
		{
			prev->freeSize += size + curr->freeSize;
			if (prev->freeSize == PAGE_SIZE)
			{
				free(pages[ind]);
				pages[ind] = NULL;
				linkedLists[ind] = NULL;
				return;
			}
			prev->next = curr->next;
			return;
		}
		if (curr->start == start + size)
		{
			curr->freeSize += size;
			curr->start = start;
			return;
		}
		if (prev->start + prev->freeSize == start)
		{
			prev->freeSize += size;
			return;
		}
		if (prev->start + prev->freeSize < start && start + size < curr->start)
		{
			node * newNode = (node *)malloc(sizeof(node));
			newNode->freeSize = size;
			newNode->start = start;
			newNode->next = curr;
			prev->next = newNode;
		}
		prev = curr;
		curr = curr->next;
	}
	if (prev->start + prev->freeSize == start)
	{
		prev->freeSize += size;
		if (prev->freeSize == PAGE_SIZE)
		{
			free(pages[ind]);
			pages[ind] = NULL;
			linkedLists[ind] = NULL;
			return;
		}
		return;
	}
	node * newNode = (node *)malloc(sizeof(node));
	newNode->freeSize = size;
	newNode->start = start;
	newNode->next = NULL;
	prev->next = newNode;

	return;
}

void showMemory()
{
	printf("\n_______________________________________________________________\n");
	for (int i = 0; i < 12; i++)
	{
		printf("\n\n<---------------page no: %d--------------->\n", i + 1);
		node * tmp = linkedLists[i];
		while (tmp != NULL)
		{
			printf("(%d, %d)---> ", tmp->start, tmp->freeSize);
			tmp = tmp->next;
		}
	}
}

void destroy()
{
	for (int i = 0; i < 12; i++)
	{
		if (pages[i] != NULL)
		{
			free(pages[i]);
		}
	}
}