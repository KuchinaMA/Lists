#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>

#include "Logfile.h"
#include "List.h"

int list_constructor(List* list) {

    assert (list != NULL);

    list->data = (int*)calloc(ListLen, sizeof(int));
    list->data[0] = ListPoison;

    list->next = (int*)calloc(ListLen, sizeof(int));
    for (size_t i = 0; i < ListLen; i++)
        list->next[i] = -1 * (i + 1);

    list->prev = (int*)calloc(ListLen, sizeof(int));
    for (size_t i = 0; i < ListLen; i++)
        list->prev[i] = - 1;

    list->head = 0;
    list->tail = 0; //???
    list->free = 1; //???

    list->next[0] = list->head;
    list->prev[0] = list->tail;

    return NoErrors;
}

int list_destructor(List* list) {

    assert (list != NULL);

    free(list->data);
    free(list->next);
    free(list->prev);

    list->head = ListPoison;
    list->tail = ListPoison;
    list->free = ListPoison;

    return NoErrors;
}

int list_dump(const List* list, const char *file, int line, const char *function) {

    fprintf(LOG_FILE, "list_dump from file: %s line %d function: %s\n\n",
                                             file, line, function);

    fprintf(LOG_FILE, "head = %d\n", list->head);
    fprintf(LOG_FILE, "tail = %d\n", list->tail);
    fprintf(LOG_FILE, "free = %d\n\n", list->free);


    fprintf(LOG_FILE, "number: ");
    for (size_t i = 0; i < ListLen; i++)
        fprintf(LOG_FILE, " %04lld |", i);
    fprintf(LOG_FILE, "\n\n");

    fprintf(LOG_FILE, "data:   ");
    for (size_t i = 0; i < ListLen; i++)
        fprintf(LOG_FILE, " %04d |", list->data[i]);
    fprintf(LOG_FILE, "\n\n");

    fprintf(LOG_FILE, "next:   ");
    for (size_t i = 0; i < ListLen; i++)
        fprintf(LOG_FILE, " %04d |", list->next[i]);
    fprintf(LOG_FILE, "\n\n");

    fprintf(LOG_FILE, "prev:   ");
    for (size_t i = 0; i < ListLen; i++)
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

    if (previous == list->prev[list->head])
        list->head = list->free;

    list->next[list->free] = list->next[previous];
    list->next[previous] = list->free;

    list->prev[list->free] = previous;
    list->prev[list->next[list->free]] = list->free;

    if (previous == list->tail)
        list->tail = list->free;

    list->free = next_free;

    list_verify(list);

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

    if (previous == list->prev[list->head]) {
        int head = list->next[list->head];
        list->head = head;
    }

    if (previous == list->prev[list->tail]) {
        int tail = list->prev[list->tail];
        list->tail = tail;
    }

    list->next[previous] = list->next[deleted_elem];
    list->next[deleted_elem] = -1 * list->free;

    list->prev[list->next[previous]] = list->prev[deleted_elem];
    list->prev[deleted_elem] = -1;

    list->free = next_free;

    list_verify(list);

    return NoErrors;
}


int list_check(const List* list) {

    int ans = 0;

    if (list == NULL)                                ans = ans | ListNull;

    if (list->data == NULL)                          ans = ans | DataNull;

    if (list->next == NULL)                          ans = ans | NextNull;

    if (list->prev == NULL)                          ans = ans | PrevNull;

    if (list->head < 0)                              ans = ans | NegativeHead;

    if (list->tail < 0)                              ans = ans | NegativeTail;

    if (list->free < 0)                              ans = ans | NegativeFree;

    ans = ans | next_prev_check(list);

    return ans;

}

int next_prev_check(const List* list) {

    int NextData[ListLen] = {};
    int PrevData[ListLen] = {};

    int NextCount = 0;
    int NextCur = list->head;

    while (NextCur != 0) {

        NextData[NextCount] = list->data[NextCur];
        NextCur = list->next[NextCur];
        NextCount ++;
    }

    /*printf("NEXT\n");
    for (int a = 0; a < NextCount; a ++)
        printf("%04d ", NextData[a]);
    printf("\n\n");*/

    int PrevCount = 0;
    int PrevCur = list->tail;


    while (PrevCur != 0) {

        PrevData[PrevCount] = list->data[PrevCur];
        PrevCur = list->prev[PrevCur];
        PrevCount ++;
    }

    /*printf("PREV\n");
    for (int b = 0; b < PrevCount; b ++)
        printf("%04d ", PrevData[b]);
    printf("\n\n");*/

    if (NextCount != PrevCount)
        return NotMatchNextPrev;

    else {
        for (int i = 0; i < PrevCount; i++) {
            if (NextData[i] != PrevData[PrevCount - 1 - i])
                return NotMatchNextPrev;
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
    if (err & NegativeHead)     fprintf(LOG_FILE, "ERROR! head < 0\n\n");
    if (err & NegativeTail)     fprintf(LOG_FILE, "ERROR! tail < 0\n\n");
    if (err & NegativeFree)     fprintf(LOG_FILE, "ERROR! free < 0\n\n");
    if (err & NotMatchNextPrev) fprintf(LOG_FILE, "ERROR! list.next or list.prev is incorrect\n\n");

}

int list_dump_picture(const List* list) {

    FILE* dotfile = fopen("ListPicture.dot", "w");

    fprintf(dotfile, "digraph {\n");
    fprintf(dotfile, "  rankdir = LR;\n");
    fprintf(dotfile, "  node [shape = Mrecord, color = \"#006400\", style = filled, fillcolor = \"#D5FFD5\"];\n");
    fprintf(dotfile, "{");


    fprintf(dotfile, "  el0[color = \"#333333\", style = filled, fillcolor = \"#E9E9E9\", label = \"0 | Reserved | <d0> value: %d | <n0> next: %d | <p0> prev: %d\"];\n",
                                    list->data[0], list->next[0], list->prev[0]);

    for (int i = 1; i < ListLen; i++) {

        if (list->prev[i] != -1) {

            if (i == list->head)
                fprintf(dotfile, "  el%d[label = \"%d | Head | <d%d> value: %d | <n%d> next: %d | <p%d> prev: %d\"];\n",
                                  i, i, i, list->data[i], i, list->next[i], i, list->prev[i]);

            else if (i == list->tail)
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

    //for (int i = 0; i < ListLen - 1; i++)
        //fprintf(dotfile, "  el%d: <d%d> -> el%d: <d%d> [weight = 10000, style = \"bold\", arrowhead = \"none\", color = \"#FFFFFF\"];\n", i, i, i+1, i+1);

    fprintf(dotfile, "  ");
    for (int i = 0; i < ListLen - 1; i++)
        fprintf(dotfile, "el%d: <d%d> ->", i, i);
    fprintf(dotfile, "el%lld: <d%lld> [weight = 100000, style = \"bold\", arrowhead = \"none\", color = \"#FFFFFF\"];\n", ListLen - 1, ListLen - 1);

    int nCur = list->head;
    while (nCur != 0) {
        int NextnCur = list->next[nCur]; //куда ведёт стрелка
        fprintf(dotfile, "  el%d: <n%d> -> el%d: <n%d> [color = \"#006400\"];\n", nCur, nCur, NextnCur, NextnCur);
        nCur = NextnCur;
    }
    int NextnCur = list->next[nCur];     //рисуем стрелку из фиктивного элемента
    fprintf(dotfile, "  el%d: <n%d> -> el%d: <n%d> [color = \"#006400\"];\n", nCur, nCur, NextnCur, NextnCur);


    int pCur = list->tail;
    while (pCur != 0) {
        int NextpCur = list->prev[pCur]; //куда ведёт стрелка
        fprintf(dotfile, "  el%d: <p%d> -> el%d: <p%d> [color = \"#006400\", style = \"dashed\"];\n", pCur, pCur, NextpCur, NextpCur);
        pCur = NextpCur;
    }
    int NextpCur = list->prev[pCur];     //рисуем стрелку из фиктивного элемента
    fprintf(dotfile, "  el%d: <p%d> -> el%d: <p%d> [color = \"#006400\", style = \"dashed\"];\n", pCur, pCur, NextpCur, NextpCur);


    int fCur = list->free;
    while (fCur != ListLen - 1) {
        int NextfCur = abs(list->next[fCur]); //куда ведёт стрелка
        fprintf(dotfile, "  el%d: <n%d> -> el%d: <n%d> [color = \"#000066\"];\n", fCur, fCur, NextfCur, NextfCur);
        fCur = NextfCur;
    }

    fprintf(dotfile, "}");


    fprintf(dotfile, "{");

    fprintf(dotfile, "  general[color = \"#800000\", style = filled, fillcolor = \"#FFD5D5\", label = \"General information |  \
    Capasity: %lld | Head: %d | Tail: %d | Free: %d\"];\n", ListLen, list->head, list->tail, list->free);

    fprintf(dotfile, "}");





    fprintf(dotfile, "}");

    fclose(dotfile);

    return NoErrors;
}






