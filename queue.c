#include "queue.h"
#include <stdlib.h>

queue_t * init_queue()
{
		queue_t * p_queue = (queue_t *)malloc(sizeof(queue_t));
		if (p_queue != NULL)
		{
				p_queue->front 	= NULL;
				p_queue->rear 	= NULL;
				p_queue->size 	= 0;
		}
		return p_queue;
}
void destroy_queue(queue_t * p_queue)
{
		if (is_empty(p_queue) != 1)
		{
				clear_queue(p_queue);
		}
		free(p_queue);
}

void clear_queue(queue_t * p_queue)
{
		while (is_empty(p_queue) != 1)
		{
				pop_queue(p_queue, NULL);
		}
}

int is_empty(queue_t * p_queue)
{
		if (p_queue->front == NULL &&
				p_queue->rear == NULL &&
				p_queue->size == 0)
		{
				return 1;
		} else
		{
				return 0;
		}
}

uint8_t get_size(queue_t * p_queue)
{
		return p_queue->size;
}

p_node_t get_front(queue_t * p_queue, item_t * p_item)
{
		if (is_empty(p_queue) != 1 && p_item != NULL)
		{
				*p_item = p_queue->front->data;
		}
		return p_queue->front;
}

p_node_t get_rear(queue_t * p_queue, item_t * p_item)
{
		if (is_empty(p_queue) != 1 && p_item != NULL)
		{
				*p_item = p_queue->rear->data;
		}
		return p_queue->rear;
}

p_node_t push_queue(queue_t * p_queue, item_t item)
{
		p_node_t p_node = (p_node_t)malloc(sizeof(node_t));
		if (p_node != NULL)
		{
				p_node->data = item;
				p_node->next = NULL;
			
				if (is_empty(p_queue))
				{
						p_queue->front = p_node;
				}
				else
				{
						p_queue->rear->next = p_node;
				}
				p_queue->rear = p_node;
				p_queue->size++;
		}
		return p_node;
}

p_node_t pop_queue(queue_t * p_queue, item_t * p_item)
{
		p_node_t p_node = p_queue->front;
		if (is_empty(p_queue) != 1 && p_node != NULL)
		{
				if (p_item != NULL)
				{
						*p_item = p_node->data;
				}
				p_queue->size--;
				p_queue->front = p_node->next;
				free(p_node);
				if (p_queue->size == 0)
				{
						p_queue->rear = NULL;
				}
		}
		return p_queue->front;
}

void queue_traverse(queue_t * p_queue, void (*visit)())
{
		p_node_t p_node = p_queue->front;
		uint8_t i = p_queue->size;
		while(i--)
		{
				visit(p_node->data);
				p_node = p_node->next;
		}
}
