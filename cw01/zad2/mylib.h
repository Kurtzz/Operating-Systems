#ifndef MYLIB
#define MYLIB

typedef struct DateOfBirth{
    int day, month, year;
} DateOfBirth;

typedef struct Contact{
    char fname[10], lname[10], email[20], phone[10], address[30];
    struct DateOfBirth date;
} Contact;

typedef struct Node{
    struct Node *next, *prev;
    struct Contact data;
} Node;

typedef struct DCList{
    struct Node *head, *tail;
    int count;
} DCList;

DCList* createList(void);

void removeList(DCList*);

Node* createElement(char*, char*, int, int, int, char*, char*, char*);

void addContact(DCList*, Node*);

void removeContact(DCList*, Node*);

void find(DCList*, char*, char*);

void merge(int, int, int, Node**, int (*comparator)(const void*, const void*));

void mergesort(int, int, Node**,int (*comparator)(const void *, const void*));

int compareByDate(const void *, const void *);

int compareByName(const void *, const void *);

int compareBySurname(const void *, const void *);

void sortlist(DCList *);

void printContact(Contact);

void printList(DCList*);

#endif
