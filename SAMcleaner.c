#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//#include "ALE.h" // included by geneTree.h
#include "geneTree.h"
#include "ALEv2.h"
#include "ALElike.h"

// cc -g -O2 SAMcleaner.c -o SAMcleaner -lz -lm

int main(int argc, char **argv){
    if (argc < 2) {
        printf("Usage: %s [options] SAMfile\nOutputs a cleaned SAM file with no empty lines\n", argv[0]);
        return 0;
    }
    printf("Input file to clean: %s\n", argv[argc - 1]);
    
    
    // attempt to open the input file
    FILE *ins = fopen(argv[argc - 1], "r");
    if(ins == NULL){
        printf("Error! Could not open input file: %s\n", argv[argc - 1]);
        printf("Usage: %s [options] SAMfile\nOutputs a cleaned SAM file with no empty lines\n", argv[0]);
        return 0;
    }
    
    // open up output file
    FILE *fo;
    char fileName[100] = "clean_";
    strcat(fileName, argv[argc - 1]);
    fo = fopen(fileName, "w");
    if(fo == NULL){
        printf("Error! Could not open output files: %s\n", fileName);
        printf("Usage: %s [options] SAMfile\nOutputs a cleaned SAM file with no empty lines\n", argv[0]);
        return 0;
    }
    
    SAM_t read, readMate;
    
    int hasAlignment = 0;
    int keepGoing = 1;
    while(keepGoing > 0){
        keepGoing = fscanf( ins, "%255s%i%255s%i%i%255s%10s%i%i%255s%255s%255s", read.readName, &read.outInfo, read.refName, &read.mapStart, &read.mapPair, read.cigar, read.flag2, &read.mapEnd, &read.mapLen, read.readSeq, read.readQual, read.XA);
        
        if(keepGoing < 1){break;}
        
        if(read.cigar[0] != '*'){ // see if there is an actual alignment there
            keepGoing = fscanf( ins, "%255s%255s", read.MD, read.NM);
            hasAlignment = 1;
        }else{
            strcpy(read.MD, "MD:Z:0");
            strcpy(read.NM, "NM:i:0");
            hasAlignment = 0;
        }
        
        if (read.flag2[0] == '=' || read.flag2[0] == '*'){ // read in the mate, if it maps
            keepGoing = fscanf( ins, "%255s%i%255s%i%i%255s%10s%i%i%255s%255s%255s", readMate.readName, &readMate.outInfo, readMate.refName, &readMate.mapStart, &readMate.mapPair, readMate.cigar, readMate.flag2, &readMate.mapEnd, &readMate.mapLen, readMate.readSeq, readMate.readQual, readMate.XA);

            if(readMate.cigar[0] != '*'){ // see if there is an actual alignment there
                keepGoing = fscanf( ins, "%255s%255s", readMate.MD, readMate.NM);
            }else{
                strcpy(readMate.MD, "MD:Z:0");
                strcpy(readMate.NM, "NM:i:0");
            }
            
            if(hasAlignment == 1){
                fprintf( fo, "%s %i %s %i %i %s %s %i %i %s %s %s %s %s\n", read.readName, read.outInfo, read.refName, read.mapStart, read.mapPair, read.cigar, read.flag2, read.mapEnd, read.mapLen, read.readSeq, read.readQual, read.XA, read.MD, read.NM);
                fprintf( fo, "%s %i %s %i %i %s %s %i %i %s %s %s %s %s\n", readMate.readName, readMate.outInfo, readMate.refName, readMate.mapStart, readMate.mapPair, readMate.cigar, readMate.flag2, readMate.mapEnd, readMate.mapLen, readMate.readSeq, readMate.readQual, readMate.XA, readMate.MD, readMate.NM);
            }
        }
    }
    
    fclose(ins);
    fclose(fo);
}