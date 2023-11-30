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
/*Folyamat hozzáadása dinamikus tömbhöz*/

void RemoveProcess(ProcessList *p_list, int id);
/*Folyamat eltávolítása*/

Process* GetProcess(ProcessList *p_list, int id);
/*Folyamat keresése azonosító alapján*/

void ListProcesses(ProcessList *p_list);
/*A dinamikus tömb kilistázása tesztelés végett*/

#endif // PROCESSHANDLER
