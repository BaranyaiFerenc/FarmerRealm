#ifndef FILEMANAGER
#define FILEMANAGER

#define saveSize 2509

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "GameElements.h"

void WriteOutBin(unsigned long long int data);

bool GetSave(char path[], unsigned long long int *save);

void SetSave(unsigned short int id, unsigned char type, unsigned long long misc);

void SaveTime();

unsigned long long int GetBinary(unsigned short int id, unsigned char type, unsigned long long misc);

void SaveStats(unsigned int level,unsigned int money, unsigned long time);

void SaveInventory(Item *inventory, int n);
bool GetInventory(Item *inventory, int n);
#endif
