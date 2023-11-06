#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Logfile.h"
#include "List.h"

char PICTURENUM[] = "1";

int list_constructor(List* list) {

    assert (list != NULL);

    list->data = (int*)calloc(INITIALLISTLEN, sizeof(int));
    list->data[0] = LISTPOISON;

    list->next = (int*)calloc(INITIALLISTLEN, sizeof(int));
    for (size_t i = 0; i < INITIALLISTLEN; i++)
        list->next[i] = -1 * (i + 1);

    list->prev = (int*)calloc(INITIALLISTLEN, sizeof(int));
    for (size_t i = 0; i < INITIALLISTLEN; i++)
        list->prev[i] = - 1;

    list->free = 1;
    list->capacity = INITIALLISTLEN;
    list->size = 0;

    list->next[0] = 0;
    list->prev[0] = 0;

    ctor_dump(list);

    return NoErrors;
}

int list_destructor(List* list) {

    assert (list != NULL);

    for (int i = 0; i < list->capacity; i++){
        list->data[i] = LISTPOISON;
        list->next[i] = LISTPOISON;
        list->prev[i] = LISTPOISON;
    }

    free(list->data);
    free(list->next);
    free(list->prev);

    list->free = LISTPOISON;

    return NoErrors;
}

int list_dump(const List* list, const char *file, int line, const char *function) {

    fprintf(LOG_FILE, "list_dump from file: %s line %d function: %s\n\n",
                                             file, line, function);

    fprintf(LOG_FILE, "head = %d\n", get_head(list));
    fprintf(LOG_FILE, "tail = %d\n", get_tail(list));
    fprintf(LOG_FILE, "free = %d\n\n", list->free);


    fprintf(LOG_FILE, "number: ");
    for (size_t i = 0; i < list->capacity; i++)
        fprintf(LOG_FILE, " %04lld |", i);
    fprintf(LOG_FILE, "\n\n");

    fprintf(LOG_FILE, "data:   ");
    for (size_t i = 0; i < list->capacity; i++)
        fprintf(LOG_FILE, " %04d |", list->data[i]);
    fprintf(LOG_FILE, "\n\n");

    fprintf(LOG_FILE, "next:   ");
    for (size_t i = 0; i < list->capacity; i++)
        fprintf(LOG_FILE, " %04d |", list->next[i]);
    fprintf(LOG_FILE, "\n\n");

    fprintf(LOG_FILE, "prev:   ");
    for (size_t i = 0; i < list->capacity; i++)
        fprintf(LOG_FILE, " %04d |", list->prev[i]);
    fprintf(LOG_FILE, "\n\n");

    fprintf(LOG_FILE, "---------------------------------------------------------------------------------------------------------------");
    fprintf(LOG_FILE, "\n\n");

    return NoErrors;
}

int list_push(List* list, int value, int previous) {

    list_verify(list);


    if (list->prev[previous] == -1) {
        fprintf(LOG_FILE, "Can't find this element!\n\n");
        return IncorrectInput;
    }


    list->data[list->free] = value;

    int next_free = abs(list->next[list->free]);

    list->next[list->free] = list->next[previous];
    list->next[previous] = list->free;

    list->prev[list->free] = previous;
    list->prev[list->next[list->free]] = list->free;

    list->free = next_free;

    list->size += 1;

    if (list->size >= list->capacity - 1)
        list_realloc(list, list->capacity * REALLOCCOEFF);

    list_verify(list);

    push_dump(list, value, previous);

    return NoErrors;
}

int list_pop(List* list, int previous) {

    list_verify(list);

    if (list->prev[previous] == -1) {
        fprintf(LOG_FILE, "This element has been already deleted!\n\n");
        return IncorrectInput;
    }

    int next_free = list->next[previous];
    int deleted_elem = list->next[previous];


    list->next[previous] = list->next[deleted_elem];
    list->next[deleted_elem] = -1 * list->free;

    list->prev[list->next[previous]] = list->prev[deleted_elem];
    list->prev[deleted_elem] = -1;

    list->free = next_free;

    list->size -= 1;

    list_verify(list);

    pop_dump(list, previous);

    return NoErrors;
}


int list_realloc(List* list, size_t new_capacity) {

    list_verify(list);

    size_t needed_capacity = new_capacity * sizeof(int);

    int* temp_data = (int*)realloc(list->data, needed_capacity);

    if (temp_data != NULL) {
        list->data = temp_data;
        for (size_t i = list->capacity; i < new_capacity; i++)
            list->data[i] = 0;
    }
    else
        return ReallocError;


    int* temp_next = (int*)realloc(list->next, needed_capacity);

    if (temp_next != NULL) {
        list->next = temp_next;
        for (size_t i = list->capacity; i < new_capacity; i++)
            list->next[i] = -1 * (i + 1);
    }
    else
        return ReallocError;


    int* temp_prev = (int*)realloc(list->prev, needed_capacity);

    if (temp_prev != NULL) {
        list->prev = temp_prev;
        for (size_t i = list->capacity; i < new_capacity; i++)
            list->prev[i] = -1;
    }
    else
        return ReallocError;


    //list->free = list->capacity;
    list->capacity = new_capacity;

    list_verify(list);

    return NoErrors;
}



int list_check(const List* list) {

    int ans = 0;

    if (list == NULL)                                ans = ans | ListNull;

    if (list->data == NULL)                          ans = ans | DataNull;

    if (list->next == NULL)                          ans = ans | NextNull;

    if (list->prev == NULL)                          ans = ans | PrevNull;

    if (list->free < 0)                              ans = ans | NegativeFree;

    ans = ans | next_prev_check(list);

    return ans;

}

int next_prev_check(const List* list) {

    int next_data[MAXLISTLEN] = {};
    int prev_data[MAXLISTLEN] = {};

    int next_count = 0;
    int next_cur = get_head(list);

    while (next_cur != 0) {

        next_data[next_count] = list->data[next_cur];
        next_cur = list->next[next_cur];
        next_count ++;
    }

    int prev_count = 0;
    int prev_cur = get_tail(list);


    while (prev_cur != 0) {

        prev_data[prev_count] = list->data[prev_cur];
        prev_cur = list->prev[prev_cur];
        prev_count ++;
    }


    if (next_count != prev_count)
        return NotMatchNextPrev;

    else {
        for (int i = 0; i < prev_count; i++) {
            if (next_data[i] != prev_data[prev_count - 1 - i]) {
                fprintf(LOG_FILE, "ERROR! For %d element next is %d, but %d is previous for %d\n", i, next_data[i],
                                                                 prev_count - 1 - i, prev_data[prev_count - 1 - i]);
                return NotMatchNextPrev;
            }
        }
    }

    return NoErrors;
}


void list_verify(const List* list) {

    int err = list_check(list);

    if (err & ListNull)         fprintf(LOG_FILE, "ERROR! Pointer to list is NULL\n\n");
    if (err & DataNull)         fprintf(LOG_FILE, "ERROR! Pointer to list.data is NULL\n\n");
    if (err & NextNull)         fprintf(LOG_FILE, "ERROR! Pointer to list.next is NULL\n\n");
    if (err & PrevNull)         fprintf(LOG_FILE, "ERROR! Pointer to list.prev is NULL\n\n");
    if (err & NegativeFree)     fprintf(LOG_FILE, "ERROR! free < 0\n\n");
    if (err & NotMatchNextPrev) fprintf(LOG_FILE, "ERROR! list.next or list.prev is incorrect\n\n");

}

int list_dump_picture(const List* list) {

    FILE* dotfile = fopen("ListPicture.dot", "w");

    fprintf(dotfile, "digraph {\n");
    fprintf(dotfile, "  rankdir = LR;\n");


    int head = get_head(list);
    int tail = get_tail(list);


    fprintf(dotfile, "  node [shape = Mrecord, color = \"#006400\", style = filled, fillcolor = \"#D5FFD5\"];\n");
    fprintf(dotfile, "{");


    fprintf(dotfile, "  el0[color = \"#333333\", style = filled, fillcolor = \"#E9E9E9\", label = \"0 | Reserved | <d0> value: %d | <n0> next: %d | <p0> prev: %d\"];\n",
                                    list->data[0], list->next[0], list->prev[0]);

    for (int i = 1; i < list->capacity; i++) {

        if (list->prev[i] != -1) {

            if (i == head)
                fprintf(dotfile, "  el%d[label = \"%d | Head | <d%d> value: %d | <n%d> next: %d | <p%d> prev: %d\"];\n",
                                  i, i, i, list->data[i], i, list->next[i], i, list->prev[i]);

            else if (i == tail)
                fprintf(dotfile, "  el%d[label = \"%d | Tail | <d%d> value: %d | <n%d> next: %d | <p%d> prev: %d\"];\n",
                                  i, i, i, list->data[i], i, list->next[i], i, list->prev[i]);
            else
                fprintf(dotfile, "  el%d[label = \"%d | | <d%d> value: %d | <n%d> next: %d | <p%d> prev: %d\"];\n",
                                  i, i, i, list->data[i], i, list->next[i], i, list->prev[i]);
        }

        else
            fprintf(dotfile, "  el%d[color = \"#000066\", style = filled, fillcolor = \"#D5EAFF\", label = \"%d | | <d%d> value: %d | <n%d> next: %d | <p%d> prev: %d\"];\n",
                                  i, i, i, list->data[i], i, list->next[i], i, list->prev[i]);

    }


    fprintf(dotfile, "  ");
    for (int i = 0; i < list->capacity - 1; i++)
        fprintf(dotfile, "el%d: <d%d> ->", i, i);
    fprintf(dotfile, "el%lld: <d%lld> [weight = 1000, style = \"bold\", arrowhead = \"none\", color = \"#FFFFFF\"];\n", list->capacity - 1, list->capacity - 1);

    int nCur = head;
    while (nCur != 0) {
        int NextnCur = list->next[nCur]; //куда ведёт стрелка
        fprintf(dotfile, "  el%d: <n%d> -> el%d: <n%d> [constraint = false, weight = 1, color = \"#006400\"];\n", nCur, nCur, NextnCur, NextnCur);
        nCur = NextnCur;
    }
    int NextnCur = list->next[nCur];     //рисуем стрелку из фиктивного элемента
    fprintf(dotfile, "  el%d: <n%d> -> el%d: <n%d> [constraint = false, weight = 1, color = \"#006400\"];\n", nCur, nCur, NextnCur, NextnCur);


    int pCur = tail;
    while (pCur != 0) {
        int NextpCur = list->prev[pCur]; //куда ведёт стрелка
        fprintf(dotfile, "  el%d: <p%d> -> el%d: <p%d> [constraint = false, weight = 1, color = \"#006400\", style = \"dashed\"];\n", pCur, pCur, NextpCur, NextpCur);
        pCur = NextpCur;
    }
    int NextpCur = list->prev[pCur];     //рисуем стрелку из фиктивного элемента
    fprintf(dotfile, "  el%d: <p%d> -> el%d: <p%d> [constraint = false, weight = 1, color = \"#006400\", style = \"dashed\"];\n", pCur, pCur, NextpCur, NextpCur);


    int fCur = list->free;
    while (fCur != list->capacity - 1) {
        int NextfCur = abs(list->next[fCur]); //куда ведёт стрелка
        fprintf(dotfile, "  el%d: <n%d> -> el%d: <n%d> [constraint = false, weight = 1, color = \"#000066\", style = \"dashed\"];\n", fCur, fCur, NextfCur, NextfCur);
        fCur = NextfCur;
    }

    fprintf(dotfile, "}");


    fprintf(dotfile, "{");

    fprintf(dotfile, "  general[color = \"#800000\", style = filled, fillcolor = \"#FFD5D5\", label = \"General information |  \
    Capacity: %lld | Size: %d | Head: %d | Tail: %d | Free: %d\"];\n", list->capacity - 1, list->size, head, tail, list->free);

    fprintf(dotfile, "}");





    fprintf(dotfile, "}");

    fclose(dotfile);

    create_picture();

    return NoErrors;
}



int get_head(const List* list) {

    int ans = list->next[0];
    return ans;
}

int get_tail(const List* list) {

    int ans = list->prev[0];
    return ans;
}

void push_dump(const List* list, int value, int previous) {

    fprintf(LOG_FILE, "Push %d after element with index %d\n\n", value, previous);
    fprintf(LOG_FILE, "---------------------------------------------------------------------------------------------------------------");
    fprintf(LOG_FILE, "\n\n");

    list_dump_picture(list);

    list_dump(list, __FILE__, __LINE__, __func__);

}

void pop_dump(const List* list, int previous) {

    fprintf(LOG_FILE, "Pop after element with index %d\n\n", previous);
    fprintf(LOG_FILE, "---------------------------------------------------------------------------------------------------------------");
    fprintf(LOG_FILE, "\n\n");

    list_dump_picture(list);

    list_dump(list, __FILE__, __LINE__, __func__);

}

void ctor_dump(const List* list) {

    fprintf(LOG_FILE, "List after initializing\n\n");
    fprintf(LOG_FILE, "---------------------------------------------------------------------------------------------------------------");
    fprintf(LOG_FILE, "\n\n");

    list_dump_picture(list);

    list_dump(list, __FILE__, __LINE__, __func__);

}


void create_picture() {

    char command0[1000] = "dot ListPicture.dot -T png -o";
    char command1[1000] = "C:\\Users\\admin\\Documents\\GitHub\\Lists\\Pictures\\ListPicture";
    char command2[] = ".png";

    strcat(command1, PICTURENUM);
    strcat(command1, command2);

    strcat(command0, command1);
    system(command0);

    fprintf(LOG_FILE, "<img src = \"%s\" width = 70%>\n", command1);

    snprintf(PICTURENUM, 2, "%d", (1 + atoi(PICTURENUM)));

}











