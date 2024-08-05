#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;

    struct list_head *front, *back;
    for (front = head->next, back = head->prev; front != back;
         back = back->prev) {
        front = front->next;
        if (front == back)
            break;
    }
    list_del(front);
    q_release_element(list_entry(front, element_t, list));
    return true;
}

/* Delete all nodes that have duplicate string */
// cppcheck-suppress constParameterPointer
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head))
        return false;
    if (list_is_singular(head))
        return true;
    // Consider the situation where elements of queue have not been sorted.
    bool del = false;
    element_t *first, *first_safe, *second, *second_safe;
    list_for_each_entry_safe (first, first_safe, head, list) {
        for (second = list_entry(first->list.next, element_t, list),
            second_safe = list_entry(second->list.next, element_t, list);
             &second->list != head; second = second_safe,
            second_safe = list_entry(second_safe->list.next, element_t, list)) {
            if (!strcmp(first->value, second->value)) {
                if (first_safe == second) {
                    first_safe =
                        list_entry(first_safe->list.next, element_t, list);
                }
                list_del(&second->list);
                q_release_element(second);
                del = true;
            }
        }
        if (del) {
            list_del(&first->list);
            q_release_element(first);
            del = false;
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (list_empty(head))
        return;
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    struct list_head *interater;
    for (interater = head->next; interater->next != head;
         interater = interater->next) {
        struct list_head *first = interater, *second = interater->next;
        // swap
        first->next = second->next;
        second->prev = first->prev;
        first->prev = second;
        second->next = first;
        first->next->prev = first;
        second->prev->next = second;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head *cur, *safe;
    list_for_each_safe (cur, safe, head) {
        list_move(cur, head);
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    LIST_HEAD(new_head);
    struct list_head *cur, *safe, *front = head;
    int count = 0;
    list_for_each_safe (cur, safe, head) {
        count++;
        if (count == k) {
            list_cut_position(&new_head, front, cur);
            q_reverse(&new_head);
            list_splice_init(&new_head, front);
            count = 0;
            front = safe->prev;
        }
    }
}

struct list_head *MergeTwo(struct list_head *left,
                           struct list_head *right,
                           bool descend)
{
    struct list_head new_head;
    struct list_head *tail = &new_head;
    if (!left && !right)
        return NULL;
    while (left && right) {
        if (descend) {
            if (strcmp(list_entry(left, element_t, list)->value,
                       list_entry(right, element_t, list)->value) >= 0) {
                tail->next = left;
                left = left->next;
                tail = tail->next;
            } else {
                tail->next = right;
                right = right->next;
                tail = tail->next;
            }
        } else {
            if (strcmp(list_entry(left, element_t, list)->value,
                       list_entry(right, element_t, list)->value) <= 0) {
                tail->next = left;
                left = left->next;
                tail = tail->next;
            } else {
                tail->next = right;
                right = right->next;
                tail = tail->next;
            }
        }
    }

    if (left)
        tail->next = left;
    else if (right)
        tail->next = right;
    return new_head.next;
}

struct list_head *MergeSort(struct list_head *head, bool descend)
{
    if (!head->next)
        return head;

    struct list_head *fast, *slow;
    for (fast = head, slow = head; fast && fast->next;
         fast = fast->next->next, slow = slow->next)
        ;
    struct list_head *mid = slow;
    mid->prev->next = NULL;
    struct list_head *left = MergeSort(head, descend);
    struct list_head *right = MergeSort(mid, descend);

    return MergeTwo(left, right, descend);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_is_singular(head) || list_empty(head))
        return;

    head->prev->next = NULL;
    head->next = MergeSort(head->next, descend);

    struct list_head *front = head, *cur = head->next;
    while (cur) {
        cur->prev = front;
        // front = cur;
        cur = cur->next;
        front = front->next;
    }
    front->next = head;
    head->prev = front;
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;

    element_t *cur = list_entry(head->prev->prev, element_t, list);
    element_t *small = list_entry(head->prev, element_t, list);
    while (&cur->list != head) {
        if (strcmp(small->value, cur->value) >= 0) {
            small = cur;
            cur = list_entry(cur->list.prev, element_t, list);
        } else {
            list_del(&cur->list);
            q_release_element(cur);
            cur = list_entry(small->list.prev, element_t, list);
        }
    }
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;

    element_t *cur = list_entry(head->prev->prev, element_t, list);
    element_t *large = list_entry(head->prev, element_t, list);
    while (&cur->list != head) {
        if (strcmp(large->value, cur->value) <= 0) {
            large = cur;
            cur = list_entry(cur->list.prev, element_t, list);
        } else {
            list_del(&cur->list);
            q_release_element(cur);
            cur = list_entry(large->list.prev, element_t, list);
        }
    }
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
