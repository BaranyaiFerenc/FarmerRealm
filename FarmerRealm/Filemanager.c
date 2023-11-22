#include "Filemanager.h"

void WriteOutBin(unsigned long long int data)
{
    char bit[64];

    for(int i = 0; i< 64; i++)
    {
        bit[63-i] = (data>>i & 1) != 0 ? '1':'0';
    }

    printf("%s\n", bit);
}

void GetSave(char path[], unsigned long long int *save)
{
    FILE* file = fopen(path,"rb");

    unsigned long long int log;
    int index = 0;

    while (fread(&log,sizeof(log),1,file))
    {
        save[index] = log;
        index++;
    }

    fclose(file);
}

unsigned long long int GetBinary(unsigned short int id, unsigned char type, unsigned long long misc)
{
    unsigned long long int log = 0;
    log += id;
    log <<= 8;
    log += type;
    log <<= 44;
    log +=misc;

    return log;
}

void SetSave(unsigned short int id, unsigned char type, unsigned long long misc)
{
    //id -> 12 bit
    //type -> 8 bit
    //misc -> 44 bit

    unsigned long long int full[saveSize];
    GetSave("save.bin",full);

    //printf("%I64u\n",misc);

    unsigned long long int log = GetBinary(id, type, misc);
    full[id+1] = log;
    printf("Save set to %d\n",id);

    FILE *file;
    file = fopen("save.bin","wb");

    fwrite(&full,sizeof(full), 1, file);

    fclose(file);
}

void SaveStats(unsigned int level,unsigned int money, unsigned long time)
{
    unsigned long long int log = 0;
    log += level;
    log <<= 16;

    log += money;
    log <<= 32;

    log += time;

    //WriteOutBin(log);

    unsigned long long int full[saveSize];
    GetSave("save.bin",full);
    full[0] = log;


    FILE *file;
    file = fopen("save.bin","wb");

    fwrite(&full,sizeof(full), 1, file);

    fclose(file);

}

void SaveInventory(Item *inventory, int n)
{
    int saveLogs[n];

    for(int i = 0; i<n;i++)
    {
        saveLogs[i] = i;
        saveLogs[i] <<= 16;
        saveLogs[i] += inventory[i].Amount;
    }

    FILE *file;
    file = fopen("inventory.bin","wb");

    fwrite(&saveLogs,sizeof(saveLogs), 1, file);

    fclose(file);
}

void GetInventory(Item *inventory, int n)
{
    FILE* file = fopen("inventory.bin","rb");

    if(file == NULL) {
      printf("No inventory save\n");
      return;
    }

    int log;
    int inventoryLogs[n];
    int index = 0;

    while (fread(&log,sizeof(log),1,file))
    {
        inventoryLogs[index] = log;
        index++;
    }

    fclose(file);

    for(int i = 0; i<n;i++)
    {
        inventory[i].Amount = inventoryLogs[i] << 16 >>16;
    }
}
