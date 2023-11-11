#ifndef FILEMANAGER
#define FILEMANAGER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "GameElements.h"

void WriteOutBin(unsigned long long int data);

void GetSave(char path[], unsigned long long int *save);

void SetSave(unsigned short int id, unsigned char type, unsigned long long misc);

unsigned long long int GetBinary(unsigned short int id, unsigned char type, unsigned long long misc);

void SaveStats(unsigned int level,unsigned int money, unsigned long time);

void SaveInventory(Item *inventory, int n);
void GetInventory(Item *inventory, int n);
#endif
