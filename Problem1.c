#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t lock;
pthread_cond_t cond;
int threads_ready = 0;

typedef struct node
{
    int val;
    struct node* next;
    void (*callback_print)(struct node* my_node);
} node;

node* create_node(int val, void (*callback_print)(struct node*))
{
    node* new_node = (node*)malloc(sizeof(node));
    new_node->val = val;
    new_node->next = NULL;
    new_node->callback_print = callback_print;
    return new_node;
}

void add_node(node** head, int val, void (*callback_print)(struct node*))
{
    pthread_mutex_lock(&lock);
    node* new_node = create_node(val, callback_print);
    if(*head == NULL)
    {
        *head = new_node;
    }
    else
    {
        node* temp = *head;
        while(temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = new_node;
    }
    pthread_mutex_unlock(&lock);
}

void print_list(node** head)
{
    pthread_mutex_lock(&lock);
    if(*head == NULL)
    {
        printf("head is NULL\n");
    }
    else
    {
        node* temp = *head;
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
    pthread_mutex_unlock(&lock);
}

void delete_node(node** head, int val)
{
    pthread_mutex_lock(&lock);
    if(*head == NULL)
    {
        printf("head is NULL\n");
        pthread_mutex_unlock(&lock);
        return;
    }

    node* temp = *head;

    if(temp != NULL && temp->val == val)
    {
        *head = temp->next;
        free(temp);
        pthread_mutex_unlock(&lock);
        return;
    }

    while(temp->next != NULL && temp->next->val != val)
    {
        temp = temp->next;
    }
    
    if(temp->next == NULL)
    {
        pthread_mutex_unlock(&lock);
        return;
    }
    else
    {
        node* next_temp = temp->next->next;
        free(temp->next);
        temp->next = next_temp;
    }
    pthread_mutex_unlock(&lock);
}

void flush_list(node** head)
{
    pthread_mutex_lock(&lock);
    node* temp = *head;
    node* next_node;
    while(temp != NULL)
    {
        next_node = temp->next;
        free(temp);
        temp = next_node;
    }
    *head = NULL;
    pthread_mutex_unlock(&lock);
}

void swap(node* node1, node* node2)
{
    int temp = node1->val;
    node1->val = node2->val;
    node2->val = temp;
}

void sort_list(node** head)
{
    pthread_mutex_lock(&lock);
    if(*head == NULL || (*head)->next == NULL)
    {
        printf("0 or 1 element\n");
        pthread_mutex_unlock(&lock);
        return;
    }

    node* temp = *head;
    node* temp2;

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
    pthread_mutex_unlock(&lock);
}

void print_node(node* my_node)
{
    printf("%d ", my_node->val);
}

void wait_for_all_threads()
{
    pthread_mutex_lock(&lock);
    threads_ready++;
    if (threads_ready < 3)
    {
        pthread_cond_wait(&cond, &lock);
    }
    else
    {
        pthread_cond_broadcast(&cond);
    }
    pthread_mutex_unlock(&lock);
}

void* thread_func1(void* arg)
{
    node** head = (node**)arg;
    wait_for_all_threads();
    printf("Thread %lu: add(2)\n", pthread_self());
    add_node(head, 2, print_node);
    printf("Thread %lu: add(4)\n", pthread_self());
    add_node(head, 4, print_node);
    printf("Thread %lu: add(10)\n", pthread_self());
    add_node(head, 10, print_node);
    printf("Thread %lu: delete(2)\n", pthread_self());
    delete_node(head, 2);
    printf("Thread %lu: sort_list()\n", pthread_self());
    sort_list(head);
    printf("Thread %lu: delete(10)\n", pthread_self());
    delete_node(head, 10);
    printf("Thread %lu: delete(5)\n", pthread_self());
    delete_node(head, 5);
    return NULL;
}

void* thread_func2(void* arg)
{
    node** head = (node**)arg;
    wait_for_all_threads();
    printf("Thread %lu: add(11)\n", pthread_self());
    add_node(head, 11, print_node);
    printf("Thread %lu: add(1)\n", pthread_self());
    add_node(head, 1, print_node);
    printf("Thread %lu: delete(11)\n", pthread_self());
    delete_node(head, 11);
    printf("Thread %lu: add(8)\n", pthread_self());
    add_node(head, 8, print_node);
    printf("Thread %lu: print_list()\n", pthread_self());
    print_list(head);
    return NULL;
}

void* thread_func3(void* arg)
{
    node** head = (node**)arg;
    wait_for_all_threads();
    printf("Thread %lu: add(30)\n", pthread_self());
    add_node(head, 30, print_node);
    printf("Thread %lu: add(25)\n", pthread_self());
    add_node(head, 25, print_node);
    printf("Thread %lu: add(100)\n", pthread_self());
    add_node(head, 100, print_node);
    printf("Thread %lu: sort_list()\n", pthread_self());
    sort_list(head);
    printf("Thread %lu: print_list()\n", pthread_self());
    print_list(head);
    printf("Thread %lu: delete(100)\n", pthread_self());
    delete_node(head, 100);
    return NULL;
}

int main()
{
    node* head = NULL;

    pthread_t thread1, thread2, thread3;
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&cond, NULL);

    pthread_create(&thread1, NULL, thread_func1, &head);
    pthread_create(&thread2, NULL, thread_func2, &head);
    pthread_create(&thread3, NULL, thread_func3, &head);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);

    printf("Final list:\n");
    print_list(&head);

    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&cond);
    flush_list(&head);

    return 0;
}
