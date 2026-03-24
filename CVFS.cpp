#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAXINODE 5
#define READ 1
#define WRITE 2
#define READWRITE 3
#define MAXFILESIZE 1024
#define REGULAR 1
#define SPECIAL 0
#define START 0
#define CURRENT 1
#define END 2

typedef struct superblock
{
    int totalInodes;
    int freeInodes;
}SUPERBLOCK, *PSUPERBLOCK;

typedef struct inode
{
    char FileName[50];
    int InodeNumber;
    int FileSize;
    int FileActualSize;
    int FileType;
    char *Buffer;
    int LinkCount;
    int ReferenceCount;
    int Permission;
    struct inode *next;
}INODE, *PINODE, **PPINODE;

typedef struct filetable
{
    int readoffset;
    int writeoffset;
    int count;
    int mode;
    PINODE ptrinode;
}FILETABLE, *PFILETABLE;

typedef struct ufdt
{
    PFILETABLE ptrfiletable;
}UFDT;

UFDT UFDTArr[50];
SUPERBLOCK SUPERBLOCKobj;
PINODE head = NULL;

void InitialiseSuperBlock()
{
    int i = 0;
    
    while(i < 50)
    {
        UFDTArr[i].ptrfiletable = NULL;
        i++;
    }
    SUPERBLOCKobj.totalInodes = MAXINODE;
    SUPERBLOCKobj.freeInodes = MAXINODE;
}

void CreateDILB()
{
    int i = 0;
    PINODE newn = NULL;
    PINODE temp = head;
    
    while(i < MAXINODE)
    {
        newn = (PINODE) malloc (sizeof(INODE));
        
        newn -> LinkCount = 0;
        newn -> ReferenceCount = 0;
        newn -> FileType = 0;
        newn -> FileSize = MAXFILESIZE;
        newn -> FileActualSize = 0;
        newn -> Permission = 0;
        newn -> Buffer = NULL;
        newn -> next = NULL;
        newn -> InodeNumber = i;
        
        if(head == NULL)
        {
            head = newn;
            temp = head;
        }
        else
        {
            temp -> next = newn;
            temp = temp -> next;
        }
        i++;
    }
}

int GetFDFromName(char *name)
{
    int i = 0;
    
    while(i < 50)
    {
        if(UFDTArr[i].ptrfiletable != NULL)
        {
            if(strcmp(UFDTArr[i].ptrfiletable -> ptrinode -> FileName, name) == 0)
            return i;
        }
        i++;
    }
    return -1;
}

PINODE Get_Inode(char *name)
{
    PINODE temp = NULL;
    temp = head;
    
    if(name == NULL)
    {
        return NULL;
    }
    
    while(temp != NULL)
    {
        if(strcmp(temp -> FileName,name) == 0)
        break;
        temp = temp -> next;
    }
    return temp;
}

int CreateFile(char *name, int permission)
{
    int i = 0;
    PINODE temp = NULL;
    temp = head;
    
    if((name == NULL) || (permission <= 0) || (permission > 3))
    {
        return -1;
    }
    
    if(SUPERBLOCKobj.freeInodes == 0)
    {
        return -2;
    }
    
    if(Get_Inode(name) != NULL)
    {
        return -3;
    }
    
    while(temp != NULL)
    {
        if(temp -> FileType == 0)
        break;
        temp = temp -> next;
    }
    
    if(temp == NULL)
    {
        return -4;
    }
    
    while(i < 50)
    {
        if(UFDTArr[i].ptrfiletable == NULL)
        break;
        i++;
    }
    
    UFDTArr[i].ptrfiletable = (PFILETABLE) malloc (sizeof(FILETABLE));
    
    UFDTArr[i].ptrfiletable -> count = 1;
    UFDTArr[i].ptrfiletable -> mode = permission;
    UFDTArr[i].ptrfiletable -> readoffset = 0;
    UFDTArr[i].ptrfiletable -> writeoffset = 0;
    UFDTArr[i].ptrfiletable -> ptrinode = temp;
    
    strcpy(temp -> FileName, name);
    temp -> FileType = REGULAR;
    temp -> ReferenceCount = 1;
    temp -> LinkCount = 1;
    temp -> FileActualSize = 0;
    temp -> Permission = permission;
    temp -> Buffer = (char *) malloc (MAXFILESIZE);
    
    SUPERBLOCKobj.freeInodes--;
    return i;
}

int OpenFile(char *name, int mode)
{
    PINODE temp = NULL;
    temp = head;
    int i = 0;
    
    if((name == NULL) || (mode <= 0) || (mode > 3))
    {
        return -1;
    }
    
    temp = Get_Inode(name);
    if(temp == NULL)
    {
        return -2;
    }
    
    if(temp -> Permission < mode)
    {
        return -3;
    }
    
    while(i < 50)
    {
        if(UFDTArr[i].ptrfiletable == NULL)
        break;
        
        i++;
    }
    
    UFDTArr[i].ptrfiletable = (PFILETABLE) malloc (sizeof(FILETABLE));
    
    UFDTArr[i].ptrfiletable -> count = 1;
    UFDTArr[i].ptrfiletable -> mode = mode;
    
    if(mode == READ)
    {
        UFDTArr[i].ptrfiletable -> readoffset = 0;
    }
    
    else if(mode == WRITE)
    {
        UFDTArr[i].ptrfiletable -> writeoffset = 0;
    }
    
    else if(mode == READWRITE)
    {
        UFDTArr[i].ptrfiletable -> readoffset = 0;
        UFDTArr[i].ptrfiletable -> writeoffset = 0;
    }
    
    UFDTArr[i].ptrfiletable -> ptrinode = temp;
    temp -> ReferenceCount++;
    return i;
}

int CloseFileByName(char *name)
{
    int fd = GetFDFromName(name);
    
    if(fd == -1)
    {
        return -1;
    }
    
    UFDTArr[fd].ptrfiletable -> readoffset = 0;
    UFDTArr[fd].ptrfiletable -> writeoffset = 0;
    UFDTArr[fd].ptrfiletable -> ptrinode -> ReferenceCount--;
    
    free(UFDTArr[fd].ptrfiletable);
    UFDTArr[fd].ptrfiletable = NULL;
    
    return 0;
}

int CloseAllFile()
{
    int i = 0;
    
    while(i < 50)
    {
        if(UFDTArr[i].ptrfiletable != NULL)
        {
            UFDTArr[i].ptrfiletable -> readoffset = 0;
            UFDTArr[i].ptrfiletable -> writeoffset = 0;
            UFDTArr[i].ptrfiletable -> ptrinode -> ReferenceCount--;
            
            free(UFDTArr[i].ptrfiletable);
            UFDTArr[i].ptrfiletable = NULL;
        }
        i++;
    }
    return 0;
}

int rm_file(char *name)
{
    int fd = GetFDFromName(name);
    
    if(fd == -1)
    {
        return -1;
    }
    
    UFDTArr[fd].ptrfiletable->ptrinode->ReferenceCount--;
    UFDTArr[fd].ptrfiletable -> ptrinode -> LinkCount--;
    
    if(UFDTArr[fd].ptrfiletable -> ptrinode -> LinkCount == 0)
    {
        UFDTArr[fd].ptrfiletable -> ptrinode -> FileType = 0;
        
        free(UFDTArr[fd].ptrfiletable -> ptrinode -> Buffer);
        
        strcpy(UFDTArr[fd].ptrfiletable -> ptrinode -> FileName, "");
        
        SUPERBLOCKobj.freeInodes++;
    }
    free(UFDTArr[fd].ptrfiletable);
    UFDTArr[fd].ptrfiletable = NULL;
    return 0;
}

void ls_file()
{
    PINODE temp = NULL;
    temp = head;
    
    if(SUPERBLOCKobj.freeInodes == MAXINODE)
    {
        printf("Error : no Files Present \n");
        return;
    }
    
    printf("\n File Name \t Inode Number \t File Size \n");
    
    printf("------------------------------------------------------\n");
    
    while(temp != NULL)
    {
        if(temp -> FileType != 0)
        {
            printf("%s\t\t%d\t\t%d\n", temp->FileName, temp->InodeNumber, temp->FileActualSize);
        }
        
        temp = temp -> next;
    }
    printf("-------------------------------------------------------\n");
}

int WriteFile(int fd, char *arr, int size)
{
    if(fd < 0 || fd >= 50)
    {
        return -1;
    }
    
    if(UFDTArr[fd].ptrfiletable == NULL)
    {
        return -1;
    }
    
    if((UFDTArr[fd].ptrfiletable -> mode != WRITE) && (UFDTArr[fd].ptrfiletable -> mode != READWRITE))
    {
        return -2;
    }
    
    if(UFDTArr[fd].ptrfiletable -> writeoffset + size > MAXFILESIZE)
    {
        return -3;
    }
    
    strncpy(UFDTArr[fd].ptrfiletable -> ptrinode -> Buffer + UFDTArr[fd].ptrfiletable -> writeoffset, arr, size);
    
    UFDTArr[fd].ptrfiletable -> writeoffset += size;
    
    UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize += size;
    
    return size;
}

int ReadFile(int fd , char *arr, int size)
{
    if(fd < 0 || fd >= 50)
    {
        return -1;
    }
    
    int read_size = 0;
    
    if(UFDTArr[fd].ptrfiletable == NULL)
    {
        return -1;
    }
    
    if(UFDTArr[fd].ptrfiletable -> readoffset == UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize)
    {
        return -2;
    }
    
    read_size = UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize - UFDTArr[fd].ptrfiletable -> readoffset;
    
    if(read_size < size)
    {
        strncpy(arr, UFDTArr[fd].ptrfiletable -> ptrinode -> Buffer + UFDTArr[fd].ptrfiletable -> readoffset, read_size);
    }
    
    else
    {
        strncpy(arr, UFDTArr[fd].ptrfiletable -> ptrinode -> Buffer + UFDTArr[fd].ptrfiletable -> readoffset, size);
    }
    
    int actual_read = (read_size < size) ? read_size : size;
    UFDTArr[fd].ptrfiletable -> readoffset += actual_read;
    return actual_read;
}

int truncate_File(char *name)
{
    int fd = GetFDFromName(name);
    
    if(fd == -1)
    {
        return -1;
    }
    
   if(UFDTArr[fd].ptrfiletable->mode != WRITE &&
   UFDTArr[fd].ptrfiletable->mode != READWRITE)
   {
       return -2;
   }
    
    memset(UFDTArr[fd].ptrfiletable -> ptrinode -> Buffer ,0, MAXFILESIZE);
    
    UFDTArr[fd].ptrfiletable -> readoffset = 0;
    
    UFDTArr[fd].ptrfiletable -> writeoffset = 0;
    
    UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize = 0;
    
    return 0;
}

int stat_file(char *name)
{
    PINODE temp = NULL;
    temp = Get_Inode(name);
    
    if(temp == NULL)
    {
        return -1;
    }
    
    printf("\nFile Name : %s",temp -> FileName);
    printf("\nInode Number : %d",temp -> InodeNumber);
    printf("\nFile Size : %d",temp -> FileSize);
    printf("\nActual File Size : %d",temp -> FileActualSize);
    printf("\nLink Count : %d",temp -> LinkCount);
    printf("\nReference Count : %d\n",temp -> ReferenceCount);
    
    return 0;
}

void Display()
{
    printf("Create Open read write ls stat Close CloseAll rm truncate exit \n");
}

int main()
{
    char str[80], arr[1024];
    char command[4][80];
    int count = 0, iRet = 0, fd = 0;

    InitialiseSuperBlock();
    CreateDILB();

    printf("========================================\n");
    printf("     Custom Virtual File System (CVFS)\n");
    printf("========================================\n");
    printf("Type 'help' to see available commands\n");
    printf("========================================\n");

    while(1)
    {
        printf("\nCVFS: > ");
        fgets(str, 80, stdin);
        
        str[strcspn(str, "\n")] = 0;

        count = sscanf(str, "%s %s %s %s",
                       command[0], command[1],
                       command[2], command[3]);

        
        if(count == 1)
        {
            if(strcmp(command[0], "ls") == 0)
            {
                ls_file();
            }
            else if(strcmp(command[0], "closeall") == 0)
            {
                CloseAllFile();
                printf("All files closed successfully\n");
            }
            else if(strcmp(command[0], "help") == 0)
            {
                printf("\nCommands:\n");
                printf("create <name> <permission>\n");
                printf("open <name> <mode>\n");
                printf("read <name> <size>\n");
                printf("write <name> <data>\n");
                printf("close <name>\n");
                printf("closeall\n");
                printf("rm <name>\n");
                printf("truncate <name>\n");
                printf("stat <name>\n");
                printf("ls\n");
                printf("exit\n");
            }
            else if(strcmp(command[0], "exit") == 0)
            {
                printf("Terminating VFS...\n");
                break;
            }
            else
            {
                printf("Command not found\n");
            }
        }

        else if(count == 2)
        {
            if(strcmp(command[0], "stat") == 0)
            {
                stat_file(command[1]);
            }
            else if(strcmp(command[0], "rm") == 0)
            {
                rm_file(command[1]);
            }
            else if(strcmp(command[0], "close") == 0)
            {
                CloseFileByName(command[1]);
            }
            else if(strcmp(command[0], "truncate") == 0)
            {
                truncate_File(command[1]);
            }
            else
            {
                printf("Invalid command\n");
            }
        }

        else if(count == 3)
        {
            if(strcmp(command[0], "create") == 0)
            {
                fd = CreateFile(command[1], atoi(command[2]));

                if(fd >= 0)
                {
                    printf("File created successfully with FD : %d\n",fd);
                }
                else
                {
                    printf("Unable to create file\n");
                }    
            }
            else if(strcmp(command[0], "open") == 0)
            {
                fd = OpenFile(command[1], atoi(command[2]));

                if(fd >= 0)
                {
                    printf("File opened successfully with FD : %d\n",fd);
                }
                else
                {
                    printf("Unable to open file\n");
                }
            }
            else if(strcmp(command[0], "read") == 0)
            {
                fd = GetFDFromName(command[1]);

                iRet = ReadFile(fd, arr, atoi(command[2]));

                if(iRet < 0)
                {
                    printf("Read Error\n");
                }
                else
                {
                    arr[iRet] = '\0';
                    printf("Data : %s\n", arr);
                }
            }
            else if(strcmp(command[0], "write") == 0)
            {
                fd = GetFDFromName(command[1]);

                iRet = WriteFile(fd, command[2], strlen(command[2]));

                if(iRet < 0)
                {
                    printf("Write Error\n");
                }
                else
                {
                    printf("Bytes Written : %d\n",iRet);
                }
            }
            else
            {
                printf("Invalid command\n");
            }
        }

        else
        {
            printf("Invalid input\n");
        }
    }

    return 0;
}
    
