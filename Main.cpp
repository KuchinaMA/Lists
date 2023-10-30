#include <stdio.h>

#include "Logfile.h"
#include "List.h"

int main() {

    open_log("ListDump.log", __TIME__);

    List list = {};

    list_constructor(&list);

    for (int i = 1; i < 9; i++) {
        list.data[i] = 10*i;
        list.next[i] = i + 1;
        list.prev[i] = i - 1;
    }

    list.next[8] = 0;

    list.head = 1;
    list.tail = 8;
    list.free = 9;

    //list_dump(&list);

    list_push(&list, 25, 2);
    list_dump(&list, __FILE__, __LINE__, __func__);

    list_push(&list, 90, 8);
    list_dump(&list, __FILE__, __LINE__, __func__);

    /*list_pop(&list, 8);
    list_dump(&list);*/

    list_pop(&list, 0);
    list_dump(&list, __FILE__, __LINE__, __func__);

    list_pop(&list, 0);
    list_dump(&list, __FILE__, __LINE__, __func__);

    list_push(&list, 7, 0);
    list_dump(&list, __FILE__, __LINE__, __func__);

    list_push(&list, 5, 0);
    list_dump(&list, __FILE__, __LINE__, __func__);

    list_push(&list, 3, 0);
    list_dump(&list, __FILE__, __LINE__, __func__);

    /*list_pop(&list, 5);
    list_dump(&list);*/

    list_destructor(&list);

    return 0;
}
