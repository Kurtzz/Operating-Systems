#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"mylib.h"

DCList* createList()
{
    DCList *newList = malloc(sizeof(DCList));
    newList->head = NULL;
    newList->tail = NULL;
    newList->count = 0;
    return newList;
}

void removeList(DCList *dclist)
{
    Node *ptr = dclist->head->next;
    while(ptr) {
        free(ptr->prev);
        ptr = ptr->next;
    }
    free(dclist->tail);
}

Node* createElement(char fname[10], char lname[10], int day, int month, int year, char address[30], char email[20], char phone[10])
{
    Node *element = malloc(sizeof(Node));

    element->next = NULL;
    element->prev = NULL;

    strcpy(element->data.fname,fname);
    strcpy(element->data.lname, lname);
    element->data.date.day = day;
    element->data.date.month = month;
    element->data.date.year = year;
    strcpy(element->data.email, email);
    strcpy(element->data.phone, phone);
    strcpy(element->data.address, address);

    return element;
}

void addContact(DCList *dclist, Node *newEl)
{
    newEl->next = dclist->head;
    dclist->head = newEl;
    dclist->count++;

    if(newEl->next) newEl->next->prev = newEl;
    else dclist->tail = newEl;
}

void removeContact(DCList *dclist, Node *element)
{
  if(element->prev) element->prev->next = element->next;
  else dclist->head = element->next;

  if(element->next) element->next->prev = element->prev;
  else dclist->tail = element->prev;

  dclist->count--;
  free(element);
}

void find(DCList *dclist, char *fname, char *lname)
{
    Node *p = dclist->head;
    int found = 0;
    while(p) {
        if (strcmp(p->data.fname,fname)==0 && strcmp(p->data.lname,lname)==0) {
            printContact(p->data);
            found = 1;
        }
        p = p->next;
    }
    if(!found) printf("Contact not found\n");
}

void merge(int first, int middle, int last, Node **tab, int (*comparator)(const void*, const void*))
{
    int i,j,q;
    Node *t[sizeof(*tab)];

    for (i=first; i<=last; i++) t[i]=tab[i];
    i=first; j=middle+1; q=first;
    while (i<=middle && j<=last) {
        if (comparator(t[i], t[j])<= 0) tab[q++]=t[i++];
        else tab[q++]=t[j++];
    }
    while (i<=middle) tab[q++]=t[i++];
}

void mergesort(int first, int last, Node **tab,int (*comparator)(const void *, const void*))
{
    int middle;
    if (first<last) {
        middle = (first + last) / 2;
        mergesort(first, middle, tab, comparator);
        mergesort(middle + 1, last, tab, comparator);
        merge(first, middle, last, tab, comparator);
    }
}
int compareByDate(const void *a, const void *b)
{
    Node *node1 = (Node*)a;
    Node *node2 = (Node*)b;
    if(node1->data.date.year < node2->data.date.year) return -1;
    else if(node1->data.date.year > node2->data.date.year) return 1;
    else {
        if(node1->data.date.month < node2->data.date.month) return -1;
        else if(node1->data.date.month > node2->data.date.month) return 1;
        else {
            if(node1->data.date.day < node2->data.date.day) return -1;
            else if(node1->data.date.day > node2->data.date.day) return 1;
            else return 0;
        }
    }
}
int compareByName(const void *a, const void *b)
{
    Node *node1 = (Node*)a;
    Node *node2 = (Node*)b;
    return strcmp((node1->data.fname), (node2->data.fname));
}
int compareBySurname(const void *a, const void *b)
{
    Node *node1 = (Node*)a;
    Node *node2 = (Node*)b;
    return strcmp((node1->data.lname), (node2->data.lname));
}

void sortlist(DCList *dclist)
{
    int size = dclist->count;
    Node **tab= calloc(size, sizeof(Node*));
    int i;
    Node *ptr = dclist->head;

    for(i=0; ptr; i++, ptr=ptr->next){
        tab[i] = ptr;
    }

    mergesort(0, dclist->count -1, tab, compareByDate);
    mergesort(0, dclist->count -1, tab, compareByName);
    mergesort(0, dclist->count -1, tab, compareBySurname);

    dclist->head = tab[0];
    dclist->head->next = tab[1];
    dclist->head->prev = NULL;

    for(i=1; i< dclist->count - 1; i++){
        tab[i]->next = tab[i+1];
        tab[i]->prev = tab[i-1];
    }

    dclist->tail = tab[dclist->count -1];
    dclist->tail->next = NULL;
    dclist->tail->prev = tab[dclist->count -2];
}

void printContact(Contact contact)
{
    printf("{%s %s, %d.%d.%d, %s, %s, %s}\n", contact.lname, contact.fname,
           contact.date.day, contact.date.month, contact.date.year, contact.address, contact.phone, contact.email);
}

void printList(DCList *dclist)
{
    Node *ptr = dclist->head;
    while(ptr) {
        printContact(ptr->data);
        ptr = ptr->next;
    }
}
