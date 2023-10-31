#ifndef LIST_H_INCLUDED
#define LIST_H_INCLUDED

const size_t ListLen = 10;
const int ListPoison = -555;

enum Errors {
    NoErrors         = 0,
    ListNull         = 1,
    DataNull         = 1 << 1,
    NextNull         = 1 << 2,
    PrevNull         = 1 << 3,
    NegativeHead     = 1 << 4,
    NegativeTail     = 1 << 5,
    NegativeFree     = 1 << 6,
    NotMatchNextPrev = 1 << 7,
    IncorrectInput   = 1 << 8,
};

struct List {
    int* data;
    int* next;
    int* prev;
    int head;
    int tail;
    int free;
};


int list_constructor(List* list);
int list_destructor(List* list);

void list_verify(const List* list);
int list_check(const List* list);
int next_prev_check(const List* list);

int list_dump(const List* list, const char *file, int line, const char *function);
int list_dump_picture(const List* list);

int list_push(List* list, int value, int previous);
int list_pop(List* list, int previous);


#endif // LIST_H_INCLUDED
