#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>

#include "List.h"

int list_constructor(List* list) {

    assert (list != NULL);

    list->data = (int*)calloc(ListLen, sizeof(int));
    list->data[0] = ListPoison;

    list->next = (int*)calloc(ListLen, sizeof(int));
    list->next[0] = 0;
    for (int i = 1; i < ListLen; i++)
        list->next[i] = -1 * (i + 1);

    list->prev = (int*)calloc(ListLen, sizeof(int));
    list->prev[0] = 0;
    for (int i = 1; i < ListLen; i++)
        list->prev[i] = - 1;

    list->head = 0;
    list->tail = 0; //???
    list->free = 1; //???

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

int list_dump(const List* list, FILE* logfile) {

    fprintf(logfile, "head = %d\n", list->head);
    fprintf(logfile, "tail = %d\n", list->tail);
    fprintf(logfile, "free = %d\n\n", list->free);


    fprintf(logfile, "number: ");
    for (int i = 0; i < ListLen; i++)
        fprintf(logfile, "%04d ", i);
    fprintf(logfile, "\n\n");

    fprintf(logfile, "data:   ");
    for (int i = 0; i < ListLen; i++)
        fprintf(logfile, "%04d ", list->data[i]);
    fprintf(logfile, "\n\n");

    fprintf(logfile, "next:   ");
    for (int i = 0; i < ListLen; i++)
        fprintf(logfile, "%04d ", list->next[i]);
    fprintf(logfile, "\n\n");

    fprintf(logfile, "prev:   ");
    for (int i = 0; i < ListLen; i++)
        fprintf(logfile, "%04d ", list->prev[i]);
    fprintf(logfile, "\n\n");

    return NoErrors;
}

int list_push(List* list, int value, int previous, FILE* logfile) {

    if (list->prev[previous] == -1) {
        fprintf(logfile, "Can't find this element!\n\n");
        return IncorrectInput;
    }

    list->data[list->free] = value;

    int next_free = abs(list->next[list->free]);

    if (previous == 0) {      //������� ��� � ���� ������ ��� ����� �������
        list->next[list->free] = list->head;
        list->head = list->free;
    }

    else {
        list->next[list->free] = list->next[previous];
        list->next[previous] = list->free;
    }

    list->prev[list->free] = previous;
    list->prev[list->next[list->free]] = list->free;

    if (previous == list->tail) {
        list->tail = list->free;
        list->prev[list->next[list->free]] = 0;    //�� ��� ������� �������
    }

    list->free = next_free;

    list_verify(list, logfile);

    return NoErrors;
}

int list_pop(List* list, int previous, FILE* logfile) {

    if (list->prev[previous] == -1) {
        fprintf(logfile, "This element has been already deleted!\n\n");
        return IncorrectInput;
    }

    int next_free = list->next[previous];
    int deleted_elem = list->next[previous];

    if (previous == list->prev[list->head]) {
        deleted_elem = list->head;
        next_free = list->head;
        int head = list->next[list->head];
        list->head = head;
        list->prev[list->head] = 0;
    }

    if (previous == list->prev[list->tail]) {
        int tail = list->prev[list->tail];
        list->tail = tail;
    }

    if (list->prev[deleted_elem] != 0)                        //���� ������� �� ������ ����� next ���
        list->next[previous] = list->next[deleted_elem];      //�������� �������� �� ������� ???
    list->next[deleted_elem] = -1 * list->free;

    list->prev[list->next[previous]] = list->prev[deleted_elem];
    list->prev[deleted_elem] = -1;

    list->free = next_free;

    list_verify(list, logfile);

    return NoErrors;
}


int list_check(const List* list) {

    int ans = 0;
    //int next_prev = next_prev_check(list);

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


void list_verify(const List* list, FILE* logfile) {

    int err = list_check(list);

    if (err & ListNull)         fprintf(logfile, "ERROR! Pointer to list is NULL\n\n");
    if (err & DataNull)         fprintf(logfile, "ERROR! Pointer to list.data is NULL\n\n");
    if (err & NextNull)         fprintf(logfile, "ERROR! Pointer to list.next is NULL\n\n");
    if (err & PrevNull)         fprintf(logfile, "ERROR! Pointer to list.prev is NULL\n\n");
    if (err & NegativeHead)     fprintf(logfile, "ERROR! head < 0\n\n");
    if (err & NegativeTail)     fprintf(logfile, "ERROR! tail < 0\n\n");
    if (err & NegativeFree)     fprintf(logfile, "ERROR! free < 0\n\n");
    if (err & NotMatchNextPrev) fprintf(logfile, "ERROR! list.next or list.prev is incorrect\n\n");

}

