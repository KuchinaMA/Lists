#include <stdio.h>

#include "Logfile.h"
#include "List.h"

int main() {

    //open_log("ListDump.log", __TIME__);
    open_log("ListDump.htm", __TIME__);

    List list = {};

    list_constructor(&list);

    for (int i = 0; i < 6; i++) {
        list_push(&list, 10*(i + 1), i);
    }

    list_push(&list, 25, 2);

    list_pop(&list, 4);

    //printf("%d\n", phys_to_log(&list, 7));

    //printf("%d\n", log_to_phys(&list, 134));

    //printf("%d\n", list_find(&list, 90));

    list_sort(&list);

    list_destructor(&list);

    return 0;
}
