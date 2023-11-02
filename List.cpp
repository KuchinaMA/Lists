#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Logfile.h"
#include "List.h"

char PictureNum[] = "1";

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

    list->free = 1;

    list->next[0] = 0;
    list->prev[0] = 0;

    ctor_dump(list);

    return NoErrors;
}

int list_destructor(List* list) {

    assert (list != NULL);

    free(list->data);
    free(list->next);
    free(list->prev);

    list->free = ListPoison;

    return NoErrors;
}

int list_dump(const List* list, const char *file, int line, const char *function) {

    fprintf(LOG_FILE, "list_dump from file: %s line %d function: %s\n\n",
                                             file, line, function);

    fprintf(LOG_FILE, "head = %d\n", get_head(list));
    fprintf(LOG_FILE, "tail = %d\n", get_tail(list));
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

    list->next[list->free] = list->next[previous];
    list->next[previous] = list->free;

    list->prev[list->free] = previous;
    list->prev[list->next[list->free]] = list->free;

    list->free = next_free;

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

    list_verify(list);

    pop_dump(list, previous);

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

    int NextData[ListLen] = {};
    int PrevData[ListLen] = {};

    int NextCount = 0;
    int NextCur = get_head(list);

    while (NextCur != 0) {

        NextData[NextCount] = list->data[NextCur];
        NextCur = list->next[NextCur];
        NextCount ++;
    }

    int PrevCount = 0;
    int PrevCur = get_tail(list);


    while (PrevCur != 0) {

        PrevData[PrevCount] = list->data[PrevCur];
        PrevCur = list->prev[PrevCur];
        PrevCount ++;
    }


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
    if (err & NegativeFree)     fprintf(LOG_FILE, "ERROR! free < 0\n\n");
    if (err & NotMatchNextPrev) fprintf(LOG_FILE, "ERROR! list.next or list.prev is incorrect\n\n");

}

int list_dump_picture(const List* list) {

    FILE* dotfile = fopen("ListPicture.dot", "w");

    fprintf(dotfile, "digraph {\n");
    fprintf(dotfile, "  rankdir = LR;\n");
    //fprintf(dotfile, "  overlap=false\n");
    //fprintf(dotfile, "  splines = true\n");
    //fprintf(dotfile, "  graph [splines = ortho];\n");
    //fprintf(dotfile, "  splines = false\n");
    //fprintf(dotfile, "  splines = curved\n");
    //fprintf(dotfile, "  splines = polyline\n");
    //fprintf(dotfile, "  nodesep = 1\n");

    int head = get_head(list);
    int tail = get_tail(list);


    fprintf(dotfile, "  node [shape = Mrecord, color = \"#006400\", style = filled, fillcolor = \"#D5FFD5\"];\n");
    fprintf(dotfile, "{");


    fprintf(dotfile, "  el0[color = \"#333333\", style = filled, fillcolor = \"#E9E9E9\", label = \"0 | Reserved | <d0> value: %d | <n0> next: %d | <p0> prev: %d\"];\n",
                                    list->data[0], list->next[0], list->prev[0]);

    for (int i = 1; i < ListLen; i++) {

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
    for (int i = 0; i < ListLen - 1; i++)
        fprintf(dotfile, "el%d: <d%d> ->", i, i);
    fprintf(dotfile, "el%lld: <d%lld> [weight = 1000, style = \"bold\", arrowhead = \"none\", color = \"#FFFFFF\"];\n", ListLen - 1, ListLen - 1);

    int nCur = head;
    while (nCur != 0) {
        int NextnCur = list->next[nCur]; //куда ведЄт стрелка
        fprintf(dotfile, "  el%d: <n%d> -> el%d: <n%d> [constraint = false, weight = 1, color = \"#006400\"];\n", nCur, nCur, NextnCur, NextnCur);
        nCur = NextnCur;
    }
    int NextnCur = list->next[nCur];     //рисуем стрелку из фиктивного элемента
    fprintf(dotfile, "  el%d: <n%d> -> el%d: <n%d> [constraint = false, weight = 1, color = \"#006400\"];\n", nCur, nCur, NextnCur, NextnCur);


    int pCur = tail;
    while (pCur != 0) {
        int NextpCur = list->prev[pCur]; //куда ведЄт стрелка
        fprintf(dotfile, "  el%d: <p%d> -> el%d: <p%d> [constraint = false, weight = 1, color = \"#006400\", style = \"dashed\"];\n", pCur, pCur, NextpCur, NextpCur);
        pCur = NextpCur;
    }
    int NextpCur = list->prev[pCur];     //рисуем стрелку из фиктивного элемента
    fprintf(dotfile, "  el%d: <p%d> -> el%d: <p%d> [constraint = false, weight = 1, color = \"#006400\", style = \"dashed\"];\n", pCur, pCur, NextpCur, NextpCur);


    int fCur = list->free;
    while (fCur != ListLen - 1) {
        int NextfCur = abs(list->next[fCur]); //куда ведЄт стрелка
        fprintf(dotfile, "  el%d: <n%d> -> el%d: <n%d> [constraint = false, weight = 1, color = \"#000066\", style = \"dashed\"];\n", fCur, fCur, NextfCur, NextfCur);
        fCur = NextfCur;
    }

    fprintf(dotfile, "}");


    fprintf(dotfile, "{");

    fprintf(dotfile, "  general[color = \"#800000\", style = filled, fillcolor = \"#FFD5D5\", label = \"General information |  \
    Capasity: %lld | Head: %d | Tail: %d | Free: %d\"];\n", ListLen - 1, head, tail, list->free);

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
    //fprintf(LOG_FILE, "<img src=\"/Pictures/ListPictureimg/название файла с расширением\Ф>\n");

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

    strcat(command1, PictureNum);
    strcat(command1, command2);

    strcat(command0, command1);
    system(command0);

    fprintf(LOG_FILE, "<img src = \"%s\">\n", command1);

    snprintf(PictureNum, 2, "%d", (1 + atoi(PictureNum)));

}











