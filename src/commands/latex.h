/*
 * latex <path/filename> <template type> {optional arguments}
 * This command creates a LaTeX file and puts it into a folder (at the given PATH) which you can use for example with Overleaf.com to make a PDF.
 * There are three different templates: exercise, project-report and CV.
 * Each template has different contents that have to be replaced.
 * You can change some of them directly with the command arguments and some data has to be replaced in the LaTeX file manually.
 *
 * Here are which arguments can be used for which template:
 * !Write "_" instead of a space. This will be automatically changed back for you in the template!
 * EXERCISE:
 *      - author:<Name_Surname>
 *      - course:<coursename>
 *      - semester:<semester>
 *      - number:<exercise_number>
 *      - picture:<./path/to/picture> or pic:<./path/to/picture>
 * PROJECT-REPORT:
 *      - author:<Name_Surname>
 *      - title:<Title_of_project>
 *      - subtitle:<Subtitle>
 *      - course:<coursename>
 *      - semester:<semester>
 *      - university:<Uni_Name> or uni:<Uni_Name>
 *      - picture:<./path/to/picture> or pic:<./path/to/picture>
 * CV:
 *      - author:<Name_Surname>
 *      - color:<Template_color> or colour:<Template_color>
 *      - picture:<./path/to/picture> or pic:<./path/to/picture>
 *
 * Examples:
 *  latex ./LatexExerciseSheet exercise author:Max_Mustermann course:Operating_Systems semester:Spring_Semester_2024 number:3 pic:./Desktop/testPic.png
 *  latex ./LatexProject project-report author:Max_Mustermann title:DAVIS_Shell course:Operating_Systems uni:University_of_Basel pic:./Desktop/testPic.png
 *      (Note that you don't have to give all arguments. The rest will be filled out with default values)
 *  latex ./LatexCV CV author:Max_Mustermann color:red picture:./photos/profilpic.png
 */
#ifndef LATEX_H
#define LATEX_H

#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "../davis.h"
#include "./plot.h"

int latex(char **args);
int checkFile(const char *path);
char* removeQuotes(char* str);
char* removeSuffix(char* filename, char* suffix);
char* removeUnderlines(char* input);
int copyFileContents(const char *sourcePath, FILE *destFile);
int copyFile(const char *sourcePath, const char *destPath);
int isImageFile(const char *filename);
int checkLatexArgs(char* arg);
int setupLatexArg(char** args, int start);

#endif
