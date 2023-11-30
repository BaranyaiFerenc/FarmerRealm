#ifndef PROCESSHANDLER
#define PROCESSHANDLER

#include <stdio.h>
#include <stdlib.h>

#include "debugmalloc.h"

typedef enum ProcessType
{
    Building, Plant,Craft, Loop
} ProcessType;

typedef struct Process
{
    int ProcessID;
    ProcessType type;

    int TileID;
    unsigned long long t;
    int misc;

    bool done;
} Process;

typedef struct ProcessList
{
    Process *l;
    int n;
} ProcessList;

Process* AddProcess(ProcessList *p_list, Process process);
/*Folyamat hozz�ad�sa dinamikus t�mbh�z*/

void RemoveProcess(ProcessList *p_list, int id);
/*Folyamat elt�vol�t�sa*/

Process* GetProcess(ProcessList *p_list, int id);
/*Folyamat keres�se azonos�t� alapj�n*/

void ListProcesses(ProcessList *p_list);
/*A dinamikus t�mb kilist�z�sa tesztel�s v�gett*/

#endif // PROCESSHANDLER
