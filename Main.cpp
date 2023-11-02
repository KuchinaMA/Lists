#include <stdio.h>

#include "Logfile.h"
#include "List.h"

int main() {

    //open_log("ListDump.log", __TIME__);
    open_log("ListDump.htm", __TIME__);

    List list = {};

    list_constructor(&list);

    //list_dump(&list, __FILE__, __LINE__, __func__);

    for (int i = 0; i < 6; i++) {
        list_push(&list, 10*(i + 1), i);
        //list_dump(&list, __FILE__, __LINE__, __func__);
    }

    list_push(&list, 25, 2);
    //list_dump(&list, __FILE__, __LINE__, __func__);

    list_pop(&list, 4);
    //list_dump(&list, __FILE__, __LINE__, __func__);

    //list_dump_picture(&list);

    /*list_push(&list, 90, 8);
    list_dump(&list, __FILE__, __LINE__, __func__);

    list_pop(&list, 8);
    list_dump(&list, __FILE__, __LINE__, __func__);

    list_pop(&list, 0);
    list_dump(&list, __FILE__, __LINE__, __func__);

    list_pop(&list, 0);
    list_dump(&list, __FILE__, __LINE__, __func__);

    list_pop(&list, 4);
    list_dump(&list, __FILE__, __LINE__, __func__);

    list_push(&list, 7, 0);
    list_dump(&list, __FILE__, __LINE__, __func__);

    list_push(&list, 5, 0);
    list_dump(&list, __FILE__, __LINE__, __func__);

    list_push(&list, 3, 0);
    list_dump(&list, __FILE__, __LINE__, __func__);*/

    /*list_pop(&list, 5);
    list_dump(&list, __FILE__, __LINE__, __func__);*/

    list_destructor(&list);

    return 0;
}
