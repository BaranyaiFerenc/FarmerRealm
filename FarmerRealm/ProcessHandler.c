#include "ProcessHandler.h"

void AddProcess(ProcessList *p_list, Process process)
{
    if(p_list == NULL)
        printf("Empty!\n");


    Process *new_list = (Process*)malloc(sizeof(Process)*(p_list ->n+1));

    for(int i = 0; i<p_list ->n;i++)
    {
        new_list[i] = p_list ->l[i];
    }

    new_list[p_list ->n] = process;

    if(p_list ->n > 0)
        free(p_list ->l);

    p_list->n++;

    p_list ->l = new_list;

    printf("Memory allocated with size of %I64u bite\n",(sizeof(Process)*(p_list ->n)));
}

void RemoveProcess(ProcessList *p_list, int id)
{
    Process *new_list = (Process*)malloc(sizeof(Process)*(p_list ->n-1));
    int index = 0;

    for(int i = 0; i<p_list ->n; i++)
    {
        if(p_list ->l[i].ProcessID != id)
        {
            new_list[index] = p_list ->l[i];
            index++;
        }
    }

    free(p_list->l);

    p_list->n--;
    p_list->l = new_list;

    printf("Memory allocated with size of %I64u bite\n",(sizeof(Process)*(p_list ->n)));
}

Process* GetProcess(ProcessList *p_list, int id)
{
    for(int i = 0; i<p_list->n;i++)
    {
        if(p_list->l[i].ProcessID == id)
            return &p_list->l[i];
    }

    return NULL;
}
