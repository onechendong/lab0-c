

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "list.h"

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 *but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/* Create a new element */
element_t *new_element(const char *str)
{
    if (!str)
        return NULL;
    element_t *new_e = malloc(sizeof(element_t));
    if (!new_e)
        return NULL;
    int len = strlen(str);
    new_e->value = malloc(sizeof(char) * (len + 1));
    if (!(new_e->value)) {
        free(new_e);
        return NULL;
    }
    strlcpy(new_e->value, str, len + 1);
    return new_e;
}

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (!head)
        return NULL;
    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;
    element_t *iterator, *safe;
    list_for_each_entry_safe (iterator, safe, head, list)
        q_release_element(iterator);
    free(head);
}

/* Insert an element at head of queue */
// cppcheck-suppress constParameterPointer
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *e = new_element(s);
    if (!e)
        return false;
    list_add(&e->list, head);
    return true;
}

/* Insert an element at tail of queue */
// cppcheck-suppress constParameterPointer
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *e = new_element(s);
    if (!e)
        return false;
    list_add_tail(&e->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *e = list_entry(head->next, element_t, list);
    if (sp) {
        strlcpy(sp, e->value, bufsize);
    }
    list_del(head->next);
    return e;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *e = list_entry(head->prev, element_t, list);
    if (sp) {
        strlcpy(sp, e->value, bufsize);
    }
    list_del(head->prev);
    return e;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;
    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;
    struct list_head *tmp;
    int num_of_element = 0;
    list_for_each (tmp, head) {
        num_of_element++;
    }

    struct list_head *cur = head->next;
    int mid_ele_index = num_of_element / 2;
    while (mid_ele_index) {
        cur = cur->next;
        mid_ele_index--;
    }
    list_del(cur);
    q_release_element(list_entry(cur, element_t, list));
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head) {}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
