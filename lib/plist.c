/*
 *
 *  TOTP: Time-Based One-Time Password Algorithm
 *  Copyright (c) 2017, fmount <fmount9@autistici.org>
 *
 *  This software is distributed under MIT License
 *
 *  Compute the hmac using openssl library.
 *  SHA-1 engine is used by default, but you can pass another one,
 *
 *  e.g EVP_md5(), EVP_sha224, EVP_sha512, etc
 *
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "plist.h"


void
print(NODE *head, int mode)
{
    switch(mode) {
    case 0:
        print_status(head);
        break;
    case 1:
        print_json(head);
        break;
    default:
        print_status(head);
    }
}

void
print_json(NODE *head)
{

    NODE *cur = NULL;
    cur = head;
    size_t len = get_len(head);
    printf("{\n");
    printf("\t\"providers\": {\n");

    while (len > 0) {
        if (len == 1)
            printf("\t\t\"%s\": \"%06u\"\n", (cur->p)->pname, (cur->p)->otpvalue);
        else
            printf("\t\t\"%s\": \"%06u\",\n", (cur->p)->pname, (cur->p)->otpvalue);
        cur = cur->next;
        len--;
    }

    printf("\t}\n");
    printf("}\n");
}

void
print_status(NODE *head)
{

    NODE *cur = NULL;
    cur = head;

    printf("[");
    while (cur != NULL && (cur->p)->otpvalue != 0) {
        if ((cur->p)->otpvalue == 0xFFFFFFFF) {
            printf("(%s: %s)", (cur->p)->pname, "Invalid base32");
        } else {
            printf("(%s: %06u)", (cur->p)->pname, (cur->p)->otpvalue);
        }
        cur = cur->next;
    }
    printf("]\n");
}

size_t
get_len(NODE *head)
{
    NODE *cur = NULL;
    cur = head;
    size_t length = 0;
    while (cur != NULL) {
        cur = cur->next;
        length++;
    }
    return length;
}

bool
exists(NODE *head, NODE *target)
{
    NODE *cur = NULL;
    cur = head;
    while (cur != NULL) {
        if ((cur->p)->pname == (target->p)->pname)
            return 1;
        cur = cur->next;
    }
    return 0;
}

NODE
*get_node(NODE *head, char *pname)
{
    NODE *cur = NULL;
    cur = head;
    while (cur != NULL) {
        if ((cur->p)->pname == pname) {
            return cur;
        }
        cur = cur->next;
    }
    return NULL;
}

void
push(NODE **head, char *pname, char *psecret, uint32_t otpvalue)
{
    NODE *cur = (NODE *) malloc(sizeof(NODE));
    PROVIDER *p = (PROVIDER *) malloc(sizeof(PROVIDER));

    p->pname = pname;
    p->psecret = psecret;
    p->otpvalue = otpvalue;
    cur->p = p;
    cur->next = *head;
    *head = cur;
}

NODE
*pop(NODE **head)
{
    NODE *tmp = *head;
    *head = (*head)->next;
    return tmp;
}

void
del(char *del, NODE *head)
{
    if(head == NULL)
        fprintf(stderr, "No valid list, no head found\n");

    NODE *cur = NULL;
    NODE *prev = NULL;
    cur = prev = head;

    while(cur != NULL && (strcmp((cur->p)->pname, del) != 0)) {
        prev = cur;
        cur = cur->next;
    }
    // Reached the end, should return ..
    if(cur == NULL)
        return;
    /* Found the pname in the list, free the node and
     * modify the pointer to next
     */
    prev->next = cur->next;
    free(cur);
}

void
freeProvider(PROVIDER *p)
{
    free(p->pname);
    free(p->psecret);
    free((PROVIDER *)p);
}

void
freeList(NODE *head)
{
   NODE *tmp;

   while (head != NULL) {
       tmp = head;
       #ifdef DEBUG
       printf("Deleting Provider %s\n", (tmp->p)->pname);
       #endif
       freeProvider(tmp->p);
       head = head->next;
       free(tmp);
   }
}

/*
int
main()
{

    //NODE *head = (NODE*) malloc(sizeof(NODE));
    NODE *head = NULL;
    NODE *tmp = NULL

    //Insert providers
    pushHead(&head, "AMAZON", "jsdios90");
    pushHead(&head, "GOOGLE", "jsdios90");
    pushHead(&head, "PROTONMAIL", "jsdios90");


    * Testing POP
    * tmp = pop(&head);
    * printf("[POP] => Got Node: (%s - %s)\n", (tmp->p)->pname, (tmp->p)->psecret);
    * tmp = pop(&head);
    * printf("[POP] => Got Node: (%s - %s)\n", (tmp->p)->pname, (tmp->p)->psecret);
    *

    *  Testing DELETE of a given NODE ..
    *
    *  deleteNode("GOOGLE", head);
    *  deleteNode("AMAZON", head);
    *  deleteNode("PROTONMAIL", head);
    *  deleteNode("TESTINGDELETE", head);
    *
    **/
/*
    printlist(head);
    return 0;
}
*/
