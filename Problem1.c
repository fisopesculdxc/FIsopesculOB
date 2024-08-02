#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct node
{
    int val;
    struct node* next;
    void (*callback_print)(struct node* my_node);
};

struct node* create_node(int val, void (*callback_print)(struct node*))
{
    struct node* new_node = (struct node*)malloc(sizeof(struct node));
    new_node->val = val;
    new_node->next = NULL;
    new_node->callback_print = callback_print;
    return new_node;
}

void add_node(struct node** head, int val, void (*callback_print)(struct node*))
{
    struct node* new_node = create_node(val, callback_print);
    if(*head == NULL)
    {
        *head = new_node;
    }
    else
    {
        struct node* temp = *head;
        while(temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = new_node;
    }
}

void print_list(struct node** head)
{
    if(*head == NULL)
    {
        printf("head is NULL\n");
        return;
    }
    else
    {
        struct node* temp = *head;
        while(temp!= NULL)
        {
            if(temp->callback_print != NULL)
            {
                temp->callback_print(temp);
            }
            else
            {
                printf("%d ", temp->val);
            }
            temp = temp->next;
        }
        printf("\n");
    }
}

void delete_node(struct node** head, int val)
{
    if(*head == NULL)
    {
        printf("head is NULL\n");
        return;
    }

    struct node* temp = *head;

    if(temp != NULL && temp->val == val)
    {
        *head = temp->next;
        free(temp);
        return;
    }

    while(temp->next != NULL && temp->next->val != val)
    {
        temp = temp->next;
    }
    
    if(temp->next == NULL)
    {
        return;
    }
    else
    {
        struct node* next_temp = temp->next->next;
        free(temp->next);
        temp->next = next_temp;
    }
}

void flush_list(struct node** head)
{
    struct node* temp = *head;
    struct node* next_node;
    while(temp != NULL)
    {
        next_node = temp->next;
        free(temp);
        temp = next_node;
    }
    *head = NULL;
}

void swap(struct node* node1, struct node* node2)
{
    int temp = node1->val;
    node1->val = node2->val;
    node2->val = temp;

}

void sort_list(struct node** head)
{
    if(*head == NULL || (*head)->next == NULL)
    {
        printf("0 or 1 element\n");
        return;
    }

    struct node* temp = *head;
    struct node* temp2;

    while(temp != NULL && temp->next != NULL)
    {
        temp2 = temp->next;
        while(temp2 != NULL)
        {
            if(temp->val > temp2->val)
            {
                swap(temp, temp2);
            }
            temp2 = temp2->next;
        }
        temp = temp->next;
    }
}

void print_node(struct node* my_node)
{
    printf("%d ", my_node->val);
}

int main()
{
    struct node* head = NULL;
    add_node(&head, 5, print_node);
    add_node(&head, 4, print_node);
    add_node(&head, 3, print_node);
    add_node(&head, 2, print_node);
    add_node(&head, 1, print_node);

    print_list(&head);

    //delete_node(&head, 4);
    //print_list(&head);

    //delete_node(&head, 3);
    //print_list(&head);

    //flush_list(&head);
    //print_list(&head);

    sort_list(&head);
    print_list(&head);

    return 0;
}