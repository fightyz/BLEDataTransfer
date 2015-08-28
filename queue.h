#ifndef Queue_H
#define Queue_H

#include <stdint.h>

typedef char* item_t;
typedef struct node * p_node_t;
typedef struct node
{
	item_t data;
	p_node_t next;
}node_t;

typedef struct
{
	p_node_t front;
	p_node_t rear;
	uint8_t size;
}queue_t;

/* 构造一个空队列 */
queue_t * init_queue(void);

/* 销毁一个队列 */
void destroy_queue(queue_t *p_queue);

/* 清空一个队列 */
void clear_queue(queue_t *p_queue);

/* 判断一个队列是否为空 */
int is_empty(queue_t *p_queue);

/* 返回队列大小 */
uint8_t get_size(queue_t *p_queue);

/* 返回队头元素 */
p_node_t get_front(queue_t *p_queue, item_t *p_item);

/* 返回队尾元素 */
p_node_t get_rear(queue_t *p_queue, item_t *p_item);

/* 将新元素入队 */
p_node_t push_queue(queue_t *p_queue, item_t item);

/* 将队头元素出队 */
p_node_t pop_queue(queue_t *p_queue, item_t *p_item);

/* 遍历队列并对各数据项调用visit函数 */
void queue_traverse(queue_t *p_queue, void(*visit)());

#endif
