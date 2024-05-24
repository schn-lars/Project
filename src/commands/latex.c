#include "latex.h"

#define BUFFER_SIZE 1024

char* variables;
char* exNumb;
char* pathToCheck;

int latex(char **args) {
    if (args[1] == NULL || args[2] == NULL) { // args[1] path/filename // args[2] = tempalte type
        warn("Missing data. Usage latex <Path/filename> <template type>");
        return FAILURE;
    }
    variables = calloc(sizeof(char), 1024);
    exNumb = calloc(sizeof(char), 128);
    int ind = 3; // from here there could be arguments
    pathToCheck = calloc( 1024, sizeof(char));
    char *suffix = ".tex";
    strcpy(pathToCheck, args[1]);
    if (strstr(pathToCheck, "'") != NULL || strchr(pathToCheck, '"') != NULL) { // check if file has "" or '' and remove them
        char *newToCheck = removeQuotes(pathToCheck);
        strcpy(pathToCheck, newToCheck);
    }

    if (checkFile(pathToCheck)) { // if there is already a file with this name, add (i)
        int i= 0;
        char* numb = calloc(100, sizeof(char));
        while(checkFile(pathToCheck) != 0) {
            char *token = strtok(pathToCheck, "(");
            if (token != NULL) {
                strcpy(pathToCheck, token);
            }
            sprintf(numb,"(%d)", ++i);
            strcat(pathToCheck, numb);
        }
        free(numb);
    }
    // now pathToCheck contains the next free filename that can be given
    printf("I created a directory for you: %s\n", pathToCheck);
    if (mkdir(pathToCheck, 0755) != 0) {
        printf("I couldn't create this directory for you.\n");
        free(variables);
        free(exNumb);
        return FAILURE;
    }
    setupLatexArg(args, ind);
    char* filename = calloc(sizeof(char), 128);
    if (strstr(args[2], "exercise") != NULL) {
        strcat(filename, "exercise");
        if (exNumb != NULL) { // if there was given an exercise number X, call the file exerciseX
            strcat(filename, exNumb);
        }
    } else if (strstr(args[2], "project") != NULL || strstr(args[2], "report") != NULL){
        strcat(filename, "project_report");
    }
    strcat(filename, suffix);
    char* filePath = calloc(sizeof(char), 1024);
    strcat(filePath, pathToCheck); // create path to actual latex file
    strcat(filePath, "/");
    strcat(filePath, filename);
    FILE *file = fopen(filePath, "w+"); // creates new file
    if (file == NULL) {
        warn("ERROR: Could not create file.\n");
        free(pathToCheck);
        free(variables);
        free(filename);
        free(filePath);
        free(exNumb);
        return FAILURE;
    }
    //checking variables for missing inputs and filling in with default values
    if (strstr(variables, "\\author") == NULL) {
        strcat(variables, "\\newcommand{\\authorname}{<author>}\n");
    }
    if (strstr(variables, "\\course") == NULL) {
        strcat(variables, "\\newcommand{\\course}{<course>}\n");
    }
    if (strstr(variables, "\\homeworkNumber") == NULL && strstr(args[2], "exercise") != NULL) {
        strcat(variables, "\\newcommand{\\homeworkNumber}{<Nr. X>}\n");
    }
    if (strstr(variables, "\\subtitle") == NULL && (strstr(args[2], "report") != NULL || strstr(args[2], "project") != NULL)) {
        strcat(variables, "\\newcommand{\\subtitlename}{<subtitle>}\n");
    }
    if (strstr(variables, "\\title") == NULL && (strstr(args[2], "report") != NULL || strstr(args[2], "project") != NULL)) {
        strcat(variables, "\\newcommand{\\titlename}{<Title>}\n");
    }
    if (strstr(variables, "\\uniname") == NULL && (strstr(args[2], "report") != NULL || strstr(args[2], "project") != NULL)) {
        strcat(variables, "\\newcommand{\\uniname}{<university name>}\n");
    }
    if (strstr(variables, "\\picturename") == NULL) {
        strcat(variables, "\\newcommand{\\picturename}{example.png}\n");
    }
    // check which template is chosen
    if (strstr(args[2], "exercise") != NULL) {
        if (copyFileContents("../resources/templates/exercise.tex", file) == FAILURE) {
            fclose(file);
            free(pathToCheck);
            free(variables);
            free(filename);
            free(filePath);
            free(exNumb);
            return FAILURE;
        }
    } else if (strstr(args[2], "report") != NULL || strstr(args[2], "project") != NULL) {
        if (copyFileContents("../resources/templates/project-report.tex", file) == FAILURE) {
            fclose(file);
            free(pathToCheck);
            free(variables);
            free(filename);
            free(filePath);
            free(exNumb);
            return FAILURE;
        }
    }
    free(pathToCheck);
    free(variables);
    free(filename);
    free(filePath);
    free(exNumb);
    return SUCCESS;
}

char* removeSuffix(char* filename, char* suffix) {
    if (strstr(filename, suffix) != NULL) { // check if file has .tex suffix and remove it
        size_t lenPath = strlen(filename);
        size_t lenSuffix = strlen(suffix);
        if (lenPath >= lenSuffix && strcmp(filename + lenPath - lenSuffix, suffix) == 0) { // check if pathToCheck end in .tex
            filename[lenPath - lenSuffix] = '\0'; // set \0 instead of .tex
        }
        return filename;
    } else {
        return 0;
    }
}

int copyFileContents(const char *sourcePath, FILE *destFile) {
    FILE *sourceFile = fopen(sourcePath, "r");
    int c;
    if (sourceFile == NULL) {
        perror("Could not find the template in DAVIS\n");
        return FAILURE;
    }
    fprintf(destFile, "%s", variables);
    while ((c = fgetc(sourceFile)) != EOF)
    {
        fputc(c, destFile);
    }
    fclose(sourceFile);
    fclose(destFile);
    return SUCCESS;
}

int copyFile(const char *sourcePath, const char *destPath) {
    if (isImageFile(sourcePath) == 0) { // Check if not a picture
        printf("The given File is not a picture.\n");
        return 0;
    }
    FILE *sourceFile = fopen(sourcePath, "rb");
    if (sourceFile == NULL) {
        perror("ERROR - could not open sourcefile\n");
        return 1;
    }

    FILE *destFile = fopen(destPath, "wb");
    if (destFile == NULL) {
        perror("ERROR - could not open destfile\n");
        fclose(sourceFile);
        return 1;
    }

    char buffer[BUFFER_SIZE];
    size_t bytesRead;
    printf("Before while.\n");
    while ((bytesRead = fread(buffer, 1, BUFFER_SIZE, sourceFile)) > 0) { // writes Bytes for Bytes to buffer
        if (fwrite(buffer, 1, bytesRead, destFile) != bytesRead) {
            perror("ERROR - could not write to destfile\n");
            fclose(sourceFile);
            fclose(destFile);
            return 1;
        }
    }
    printf("After while.\n");

    if (ferror(sourceFile)) {
        perror("ERROR- could not read from sourcefile\n");
    }

    fclose(sourceFile);
    fclose(destFile);

    printf("Datei erfolgreich kopiert von %s nach %s\n", sourcePath, destPath);
    return 0;
}

int isImageFile(const char *filename) {
    unsigned char pngSignature[8] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A}; // identidier of PNG
    unsigned char jpgSignature[3] = {0xFF, 0xD8, 0xFF}; // most commmon identifier of JPG

    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("ERROR - could not open file");
        return 0;
    }

    unsigned char buffer[8];
    size_t bytesRead = fread(buffer, 1, 8, file);
    fclose(file);

    if (bytesRead < 8) {
        return 0; // file can't be a picture because it is too small
    }

    // Check if PNG-signature
    if (memcmp(buffer, pngSignature, 8) == 0) {
        return 1; // PNG-file
    }

    // Check if JPG-signature
    if (memcmp(buffer, jpgSignature, 3) == 0) {
        return 1; // JPG-file
    }

    return 0; // else not a picture
}


int setupLatexArg(char** args, int start) {
    int i;
    for (i = start; args[i] != NULL; i++) { // go over rest of arguments if existing
        if (strstr(args[i], ":") == NULL) {
            printf("%s is not a valid argument and gets ignored.\n", args[i]);
        } else {
            char* arg = malloc(sizeof(char) * (MAX_ARG_LENGTH + 1));
            memcpy(arg, args[i], 128);
            if (checkLatexArgs(arg) == 0) {
                return FAILURE;
            }
            free(arg);
        }
    }
    return 1;
}

int checkLatexArgs(char* arg) {
    if (arg == NULL) {
        printf("no argument");
        return 0;
    }

    int i;
    char* extractedArg = malloc(sizeof(char) * (MAX_ARG_LENGTH + 1));
    for (i = 0; arg[i] != ':'; i++) { // goes over argument until input comes
        extractedArg[i] = arg[i];
    }
    i++;
    extractedArg[i] = '\0';
    if (&arg[i] == NULL) {
        free(extractedArg);
        return 0;
    }
    char* extractedInput = &arg[i];
    removeUnderlines(extractedInput);
    if (strstr(extractedArg, "subtitle") != NULL || strstr(extractedArg, "sub") != NULL) {
        strcat(variables, "\\newcommand{\\subtitlename}{");
        strcat(variables, extractedInput);
        strcat(variables, "}\n");
    } else if (strstr(extractedArg, "title") != NULL || strstr(extractedArg, "tit") != NULL) {
        strcat(variables, "\\newcommand{\\titlename}{");
        strcat(variables, extractedInput);
        strcat(variables, "}\n");
    } else if (strstr(extractedArg, "author") != NULL || strstr(extractedArg, "aut") != NULL) {
        strcat(variables, "\\newcommand{\\authorname}{");
        strcat(variables, extractedInput);
        strcat(variables, "}\n");
    } else if (strstr(extractedArg, "semester") != NULL || strstr(extractedArg, "sem") != NULL) {
        strcat(variables, "\\newcommand{\\semester}{");
        strcat(variables, extractedInput);
        strcat(variables, "}\n");
    } else if (strstr(extractedArg, "course") != NULL) {
        strcat(variables, "\\newcommand{\\course}{");
        strcat(variables, extractedInput);
        strcat(variables, "}\n");
    } else if (strstr(extractedArg, "number") != NULL || strstr(extractedArg, "numb") != NULL) {
        strcat(variables, "\\newcommand{\\homeworkNumber}{");
        strcpy(exNumb, extractedInput);
        strcat(variables, extractedInput);
        strcat(variables, "}\n");
    } else if (strstr(extractedArg, "university") != NULL || strstr(extractedArg, "uni") != NULL) {
        strcat(variables, "\\newcommand{\\uniname}{");
        strcat(variables, extractedInput);
        strcat(variables, "}\n");
    } else if (strstr(extractedArg, "picture") != NULL || strstr(extractedArg, "pic") != NULL) {
        char *filename;
        if (isImageFile(extractedInput) == 0) {
            printf("%s is not a valid picture and gets ignored.\n", extractedInput);
        } else {
            char* picPath = calloc(sizeof(char), 1024);
            const char *lastSlash = strrchr(extractedInput, '/');
            if (lastSlash == NULL) {
                // there was no '/' which means the path is the filename
                filename = strdup(extractedInput);
            } else {
                // the name starts after the '/'
                filename = strdup(lastSlash + 1);
            }
            strcpy(picPath, pathToCheck);
            strcat(picPath, "/");
            strcat(picPath, filename);
            if (strstr(filename, ".") == NULL) {
                strcat(picPath, ".png");
            }
            copyFile(extractedInput, picPath);
            strcat(variables, "\\newcommand{\\picturename}{");
            strcat(variables, filename);
            strcat(variables, "}\n");
            free(picPath);
        }
    } else {
        printf("%s is not a valid argument and gets ignored.\n", arg);
    }
    free(extractedArg);
    return 1;
}


char* removeUnderlines(char* input) {
    if (input == NULL) {
        return 0;
    }

    while (*input != '\0') {
        if (*input == '_') {
            *input = ' ';
        }
        input++;
    }
    return input;
}