#pragma once

void printargs(int ac, char** av, FILE* fp);
void fputformat(char* s, FILE* fp);
int checkheader(FILE* fin, char* fmt, FILE* fout);
