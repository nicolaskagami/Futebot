//Author: Nicolas Silveira Kagami 194636
//Converter from .log to .lrn
#include<stdio.h>
#include<string.h>


#define MAX_LINE_CHARS 512
#define MAX_FILENAME 512

int main(int argc, char ** argv)
{
    char lineBuffer[MAX_LINE_CHARS];
    if(argc == 2)
    {
        char * aux;
        int len = strlen(argv[1]);
        char outFileName[MAX_FILENAME];
        FILE * file = fopen(argv[1],"r");
        strncpy(outFileName,argv[1],MAX_FILENAME);
        outFileName[len-4]= '\0';
        strcat(outFileName,".lrn");
        FILE * outfile = fopen(outFileName,"w");
        if(file)
        {
            fgets(lineBuffer,MAX_LINE_CHARS,file);
            char valuesBuffer[8*10];
            char oldForce0[10] = " 0.0";
            char oldForce1[10] = " 0.0";
            int i;
            while(fgets(lineBuffer,MAX_LINE_CHARS,file)!=NULL)
            {
                aux = strtok(lineBuffer,",");
                for(i=0;i<8&&aux;i++)
                {
                    strncpy(&valuesBuffer[i*10],aux,10);
                    aux = strtok(NULL,",\n");
                }
                lineBuffer[0] ='\0';
                for(i=0;i<6;i++)
                {
                    strcat(lineBuffer,&valuesBuffer[10*i]);
                }
                strcat(lineBuffer,oldForce0);
                strcat(lineBuffer,oldForce1);
                strcat(lineBuffer,&valuesBuffer[10*6]);
                strcat(lineBuffer,&valuesBuffer[10*7]);
                strcat(lineBuffer,"\n");
                fputs(lineBuffer,outfile);
                strncpy(oldForce0,&valuesBuffer[60],10);
                strncpy(oldForce1,&valuesBuffer[70],10);
            }

            fclose(file);
        }
    }
}
