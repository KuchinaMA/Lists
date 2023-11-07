#ifndef LIST_H_INCLUDED
#define LIST_H_INCLUDED

const size_t INITIALLISTLEN = 5;
const int REALLOCCOEFF = 2;
const size_t MAXLISTLEN = 1000;
const int LISTPOISON = -555;

enum Errors {
    NoErrors         = 0,
    ListNull         = 1,
    DataNull         = 1 << 1,
    NextNull         = 1 << 2,
    PrevNull         = 1 << 3,
    NegativeFree     = 1 << 4,
    NotMatchNextPrev = 1 << 5,
    IncorrectInput   = 1 << 6,
    ReallocError     = 1 << 7,
};

struct List {
    int* data;
    int* next;
    int* prev;
    int free;
    size_t capacity;
    int size;
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
int list_realloc(List* list, size_t new_capacity);

int get_head(const List* list);
int get_tail(const List* list);

void push_dump(const List* list, int value, int previous);
void pop_dump(const List* list, int previous);
void ctor_dump(const List* list);
void sort_dump(const List* list);

void create_picture();

int phys_to_log(List* list, int phys_index);
int log_to_phys(List* list, int log_index);
int list_find(List* list, int value);
int list_sort (List* list);


#endif // LIST_H_INCLUDED
