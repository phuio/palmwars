#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//typedef unsigned char UInt8;
//typedef unsigned short UInt16;

#include "palmwarsmaps.h"

#define MAPDATAFILE "C:\\leggetr\\Mab\\mapdata.txt"
#define FILENAME "C:\\leggetr\\Mab\\palmwarsmaps.bin"

PWSYSTEMDATA *systems[50];

int main(void)
{
    PWMAPHEADER header;
    PWSYSTEMDATA *newSystem;
    FILE *fp;
    char  line[256];
    char *p;
    int   numSystems;
    int   numStars;
    int   i;
    int   j;

    fp = fopen(MAPDATAFILE, "r");
    if (!fp)
    {
        printf("Can't open mapdata.\n");
        exit(1);
    }

    fgets(line, 255, fp);
    numSystems = atoi(line);
    header.numSystems = numSystems;

    for (i=0; i<numSystems; i++)
    {

        fgets(line, 255, fp);
        numStars = atoi(line);

        newSystem = malloc(sizeof(PWSYSTEMDATA) + ((numStars-1) * sizeof(PWSTARPOS)));
        if (!newSystem)
        {
            printf("Can't allocate memory...\n");
            exit(2);
        }

        systems[i] = newSystem;

        newSystem->numStars = numStars;

        for (j=0; j<numStars; j++)
        {
            fgets(line, 255, fp);
            p = strtok(line, ",");
            newSystem->s[j].x = atoi(p);
            p = strtok(0, ",");
            newSystem->s[j].y = atoi(p);
            p = strtok(0, ",");
            p[strlen(p)-1] = 0;
            strcpy(newSystem->s[j].linkedStars, p);
        }
    }

    fclose(fp);

    fp = fopen(FILENAME, "wb");
    if (!fp)
    {
        printf("Can't create file\n");
        exit(3);
    }

    fwrite(&header, sizeof(PWMAPHEADER), 1, fp);

    for (i = 0; i<header.numSystems; i++)
    {
        fwrite(systems[i], sizeof(PWSYSTEMDATA) + (sizeof(PWSTARPOS) * (systems[i]->numStars-1)), 1, fp);
    }

    fclose(fp);

    printf("Done!\n");

    return 0;
}