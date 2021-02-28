#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
queue_t *q_new()
{
    queue_t *q = malloc(sizeof(queue_t));
    if (q) {
        q->head = NULL;
        q->size = 0;
        q->tail = &(q->head);
    }
    return q;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    if (!q)
        return;
    while (q->head) {
        list_ele_t *tmp = q->head;
        q->head = q->head->next;
        free(tmp->value);
        free(tmp);
    }
    free(q);
}

/*
 * Helper function for insertion
 */
static inline list_ele_t *ele_new(char *s)
{
    if (!s)
        return NULL;
    list_ele_t *newe = malloc(sizeof(list_ele_t));
    if (!newe)
        return NULL;
    size_t slen = strnlen(s, QUEUE_STRLEN_MAX);
    char *news = malloc((slen + 1) * sizeof(char));
    if (!news) {
        free(newe);
        return NULL;
    }
    strncpy(news, s, slen);
    news[slen] = '\0';
    newe->value = news;
    return newe;
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(queue_t *q, char *s)
{
    if (!q)
        return false;
    list_ele_t *newh = ele_new(s);
    if (!newh)
        return false;
    newh->next = q->head;
    if (!q->head)
        q->tail = &(newh->next);
    q->head = newh;
    q->size += 1;
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(queue_t *q, char *s)
{
    if (!q)
        return false;
    list_ele_t *newt = ele_new(s);
    if (!newt)
        return false;
    newt->next = NULL;
    *(q->tail) = newt;
    q->tail = &(newt->next);
    q->size += 1;
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return true if successful.
 * Return false if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 * The space used by the list element and the string should be freed.
 */
bool q_remove_head(queue_t *q, char *sp, size_t bufsize)
{
    if (!q || !q->head)
        return false;
    if (sp && bufsize) {
        strncpy(sp, q->head->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    list_ele_t *tmp = q->head;
    q->head = q->head->next;
    if (!q->head)
        q->tail = &(q->head);
    q->size -= 1;
    free(tmp->value);
    free(tmp);
    return true;
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    return q ? q->size : 0;
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(queue_t *q)
{
    if (!q || !q->head)
        return;
    list_ele_t *newh = NULL;
    list_ele_t *p = q->head;
    while (p) {
        list_ele_t *tmp = p;
        p = p->next;
        tmp->next = newh;
        newh = tmp;
    }
    q->tail = &(q->head->next);
    q->head = newh;
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */

/*
 * Helper function for merge_sort()
 */
static list_ele_t *merge(list_ele_t *p1, list_ele_t *p2)
{
    if (!p1 && !p2)
        return NULL;
    else if (!p1)
        return p2;
    else if (!p2)
        return p1;

    list_ele_t *res = NULL, **pp = &res;
    while (p1 || p2) {
        if (!p2 || (p1 && strcmp(p1->value, p2->value) <= 0)) {
            *pp = p1;
            p1 = p1->next;
        } else {
            *pp = p2;
            p2 = p2->next;
        }
        pp = &((*pp)->next);
    }
    return res;
}

/*
 * Sorting algorithm implemention of q_sort()
 */
static void merge_sort(list_ele_t **list)
{
    if (!(*list) || !(*list)->next)
        return;
    // Split into two halves
    list_ele_t **mid = list, **tail = list;
    while (*tail) {
        tail = &((*tail)->next);  // Move tail twice
        if (*tail)
            tail = &((*tail)->next);
        mid = &((*mid)->next);  // Move mid once
    }
    list_ele_t *L = *list, *R = *mid;
    *mid = NULL;

    merge_sort(&L);
    merge_sort(&R);
    *list = merge(L, R);
}

void q_sort(queue_t *q)
{
    if (!q)
        return;

    merge_sort(&(q->head));
    q->tail = &(q->head);
    while (*(q->tail))
        q->tail = &((*(q->tail))->next);
}
