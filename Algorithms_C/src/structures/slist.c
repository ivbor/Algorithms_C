#include "algorithms_c/structures/slist.h"
#include <stdlib.h>
#include <string.h>

/**
 * The Python ``LinkedList`` class emphasises readability over raw performance.
 * This C translation keeps the same teaching style by decomposing the logic
 * into bite-sized helpers, documenting each branch, and preferring explicit
 * control flow even when terser idioms exist.  The prose intentionally
 * outweighs the code so that the design decisions remain obvious to readers who
 * compare the two repositories side by side.
 */

/** Concrete node layout hidden from users of the public header. */
struct ac_slist_node {
    struct ac_slist_node *next;
    unsigned char data[];
};

/** Copy ``element`` bytes into ``node`` while handling nullable pointers. */
static void ac_slist_copy_payload(
    const ac_slist *list,
    ac_slist_node *node,
    const void *element
) {
    if (list->element_size == 0) {
        return;
    }

    if (element != NULL) {
        memcpy(node->data, element, list->element_size);
    } else {
        memset(node->data, 0, list->element_size);
    }
}

/** Allocate a node large enough to hold one element. */
static ac_slist_node *
ac_slist_allocate_node(const ac_slist *list, const void *element) {
    if (list == NULL || list->element_size == 0) {
        return NULL;
    }

    ac_slist_node *node = malloc(sizeof(*node) + list->element_size);
    if (node == NULL) {
        return NULL;
    }

    node->next = NULL;
    ac_slist_copy_payload(list, node, element);
    return node;
}

/** Remove ``node`` and advance the traversal pointer. */
static ac_slist_node *ac_slist_free_node(ac_slist_node *node) {
    if (node == NULL) {
        return NULL;
    }
    ac_slist_node *next = node->next;
    free(node);
    return next;
}

static int ac_slist_validate_inputs(const ac_slist *list, const void *value) {
    if (list == NULL || list->element_size == 0 || value == NULL) {
        return AC_SLIST_ERR_ARGUMENT;
    }
    return AC_SLIST_OK;
}

int ac_slist_init(ac_slist *list, size_t element_size) {
    if (list == NULL || element_size == 0) {
        return AC_SLIST_ERR_ARGUMENT;
    }

    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    list->element_size = element_size;
    return AC_SLIST_OK;
}

void ac_slist_clear(ac_slist *list) {
    if (list == NULL) {
        return;
    }

    ac_slist_node *node = list->head;
    while (node != NULL) {
        node = ac_slist_free_node(node);
    }

    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

void ac_slist_destroy(ac_slist *list) {
    if (list == NULL) {
        return;
    }

    ac_slist_clear(list);
    list->element_size = 0;
}

size_t ac_slist_size(const ac_slist *list) {
    return list != NULL ? list->size : 0U;
}

bool ac_slist_empty(const ac_slist *list) {
    return ac_slist_size(list) == 0U;
}

size_t ac_slist_element_size(const ac_slist *list) {
    return list != NULL ? list->element_size : 0U;
}

int ac_slist_append(ac_slist *list, const void *value) {
    int validation = ac_slist_validate_inputs(list, value);
    if (validation != AC_SLIST_OK) {
        return validation;
    }

    ac_slist_node *node = ac_slist_allocate_node(list, value);
    if (node == NULL) {
        return AC_SLIST_ERR_ALLOCATION;
    }

    if (list->tail == NULL) {
        list->head = node;
        list->tail = node;
    } else {
        list->tail->next = node;
        list->tail = node;
    }

    list->size++;
    return AC_SLIST_OK;
}

int ac_slist_prepend(ac_slist *list, const void *value) {
    int validation = ac_slist_validate_inputs(list, value);
    if (validation != AC_SLIST_OK) {
        return validation;
    }

    ac_slist_node *node = ac_slist_allocate_node(list, value);
    if (node == NULL) {
        return AC_SLIST_ERR_ALLOCATION;
    }

    node->next = list->head;
    list->head = node;
    if (list->tail == NULL) {
        list->tail = node;
    }
    list->size++;
    return AC_SLIST_OK;
}

static int ac_slist_insert_after(
    ac_slist *list,
    ac_slist_node *previous,
    const void *value
) {
    ac_slist_node *node = ac_slist_allocate_node(list, value);
    if (node == NULL) {
        return AC_SLIST_ERR_ALLOCATION;
    }

    if (previous == NULL) {
        node->next = list->head;
        list->head = node;
        if (list->tail == NULL) {
            list->tail = node;
        }
    } else {
        node->next = previous->next;
        previous->next = node;
        if (previous == list->tail) {
            list->tail = node;
        }
    }

    list->size++;
    return AC_SLIST_OK;
}

int ac_slist_insert(ac_slist *list, size_t index, const void *value) {
    int validation = ac_slist_validate_inputs(list, value);
    if (validation != AC_SLIST_OK) {
        return validation;
    }

    if (index > list->size) {
        return AC_SLIST_ERR_INDEX;
    }

    if (index == 0) {
        return ac_slist_prepend(list, value);
    }

    ac_slist_node *prev = list->head;
    for (size_t i = 1; i < index; ++i) {
        prev = prev->next;
    }

    return ac_slist_insert_after(list, prev, value);
}

static int ac_slist_remove_head(ac_slist *list, void *out_value) {
    if (list == NULL || list->head == NULL) {
        return AC_SLIST_ERR_EMPTY;
    }

    ac_slist_node *node = list->head;
    if (out_value != NULL) {
        memcpy(out_value, node->data, list->element_size);
    }

    list->head = node->next;
    if (list->head == NULL) {
        list->tail = NULL;
    }
    free(node);
    list->size--;
    return AC_SLIST_OK;
}

int ac_slist_pop_front(ac_slist *list, void *out_value) {
    return ac_slist_remove_head(list, out_value);
}

int ac_slist_pop_back(ac_slist *list, void *out_value) {
    if (list == NULL || list->head == NULL) {
        return AC_SLIST_ERR_EMPTY;
    }

    if (list->head == list->tail) {
        return ac_slist_remove_head(list, out_value);
    }

    ac_slist_node *prev = list->head;
    while (prev->next != list->tail) {
        prev = prev->next;
    }

    if (out_value != NULL && list->tail != NULL) {
        memcpy(out_value, list->tail->data, list->element_size);
    }
    free(list->tail);
    prev->next = NULL;
    list->tail = prev;
    list->size--;
    return AC_SLIST_OK;
}

int ac_slist_erase(ac_slist *list, size_t index, void *out_value) {
    if (list == NULL) {
        return AC_SLIST_ERR_ARGUMENT;
    }
    if (list->size == 0) {
        return AC_SLIST_ERR_EMPTY;
    }
    if (index >= list->size) {
        return AC_SLIST_ERR_INDEX;
    }

    if (index == 0) {
        return ac_slist_remove_head(list, out_value);
    }

    ac_slist_node *prev = list->head;
    for (size_t i = 1; i < index; ++i) {
        prev = prev->next;
    }

    ac_slist_node *node = prev->next;
    prev->next = node->next;
    if (node == list->tail) {
        list->tail = prev;
    }
    if (out_value != NULL) {
        memcpy(out_value, node->data, list->element_size);
    }
    free(node);
    list->size--;
    return AC_SLIST_OK;
}

int ac_slist_get(const ac_slist *list, size_t index, void *out_value) {
    if (list == NULL || out_value == NULL) {
        return AC_SLIST_ERR_ARGUMENT;
    }
    if (index >= list->size) {
        return AC_SLIST_ERR_INDEX;
    }

    ac_slist_node *node = list->head;
    for (size_t i = 0; i < index; ++i) {
        node = node->next;
    }
    memcpy(out_value, node->data, list->element_size);
    return AC_SLIST_OK;
}

int ac_slist_front(const ac_slist *list, void *out_value) {
    if (list == NULL || out_value == NULL) {
        return AC_SLIST_ERR_ARGUMENT;
    }
    if (list->head == NULL) {
        return AC_SLIST_ERR_EMPTY;
    }

    memcpy(out_value, list->head->data, list->element_size);
    return AC_SLIST_OK;
}

int ac_slist_back(const ac_slist *list, void *out_value) {
    if (list == NULL || out_value == NULL) {
        return AC_SLIST_ERR_ARGUMENT;
    }
    if (list->tail == NULL) {
        return AC_SLIST_ERR_EMPTY;
    }

    memcpy(out_value, list->tail->data, list->element_size);
    return AC_SLIST_OK;
}

bool ac_slist_contains(
    const ac_slist *list,
    const void *needle,
    ac_slist_compare_fn compare,
    void *user_data
) {
    if (list == NULL || compare == NULL) {
        return false;
    }

    for (ac_slist_node *node = list->head; node != NULL; node = node->next) {
        if (compare(needle, node->data, user_data) == 0) {
            return true;
        }
    }
    return false;
}

int ac_slist_for_each(
    ac_slist *list,
    ac_slist_visit_fn visit,
    void *user_data
) {
    if (list == NULL || visit == NULL) {
        return AC_SLIST_ERR_ARGUMENT;
    }

    for (ac_slist_node *node = list->head; node != NULL; node = node->next) {
        int status = visit(node->data, user_data);
        if (status != 0) {
            return status;
        }
    }

    return AC_SLIST_OK;
}

int ac_slist_for_each_const(
    const ac_slist *list,
    ac_slist_const_visit_fn visit,
    void *user_data
) {
    if (list == NULL || visit == NULL) {
        return AC_SLIST_ERR_ARGUMENT;
    }

    for (const ac_slist_node *node = list->head; node != NULL;
         node = node->next) {
        int status = visit(node->data, user_data);
        if (status != 0) {
            return status;
        }
    }

    return AC_SLIST_OK;
}
