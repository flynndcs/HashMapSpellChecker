#include "hashMap.h"
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


/**
 * Allocates a string for the next word in the file and returns it. This string
 * is null terminated. Returns NULL after reaching the end of the file.
 * @param file
 * @return Allocated string or NULL.
 */
char* nextWord(FILE* file)
{
    int maxLength = 16;
    int length = 0;
    char* word = malloc(sizeof(char) * maxLength);
    while (1)
    {
        char c = fgetc(file);
        if ((c >= '0' && c <= '9') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z') ||
            c == '\'')
        {
            if (length + 1 >= maxLength)
            {
                maxLength *= 2;
                word = realloc(word, maxLength);
            }
            word[length] = c;
            length++;
        }
        else if (length > 0 || c == EOF)
        {
            break;
        }
    }
    if (length == 0)
    {
        free(word);
        return NULL;
    }
    word[length] = '\0';
    return word;
}

/**
 * Loads the contents of the file into the hash map.
 * @param file
 * @param map
 */
void loadDictionary(FILE* file, HashMap* map)
{
    // FIXME: implement
    //while next word available
    //put word into map
    for (int i = 0; i < 109581; i++)
    {
        hashMapPut(map, nextWord(file), 0);
    }
}

/**
 * Checks the spelling of the word provded by the user. If the word is spelled incorrectly,
 * print the 5 closest words as determined by a metric like the Levenshtein distance.
 * Otherwise, indicate that the provded word is spelled correctly. Use dictionary.txt to
 * create the dictionary.
 * @param argc
 * @param argv
 * @return
 * adapted from:
 * https://rosettacode.org/wiki/Levenshtein_distance
 */
int levDistance(char* s, int sLen, char* t, int tLen)
{
    int a;
    int b;
    int c;

    if (sLen == 0)
    {
        return tLen;
    }
    if (tLen == 0)
    {
        return sLen;
    }

    if (s[sLen - 1] == t[tLen - 1])
    {
        return levDistance(s, sLen - 1, t, tLen - 1);
    }
    a = levDistance(s, sLen - 1, t, tLen - 1);
    b = levDistance(s, sLen, t, tLen - 1);
    c = levDistance(s, sLen - 1, t, tLen);

    if (a > b)
    {
        a = b;
    }
    if (a > c)
    {
        a = c;
    }

    return a + 1;
}

void addLevDistance(char* s, HashMap *map)
{
    int sLength = strlen(s);

    for (int i = 0; i < map->capacity; i++)
    {
        struct HashLink* temp = map->table[i];
        
        while (temp != NULL)
        {
            int tLength = strlen(temp->key);
            temp->value = levDistance(s, sLength, temp->key, tLength);
            //printf("%d\n", temp->value);
            temp = temp->next;
        }
    }
}



void spellcheck(char* word, HashMap *map)
{
    if (strcmp(word, "quit") == 0)
    {
        return;
    }

    addLevDistance(word, map);
    //hashMapPrint(map);

    struct HashLink* current;

    for (int i = 0; i < map->capacity; i++)
    {
        current = map->table[i];
        while (current != NULL)
        {
            if (strcmp(current->key, word) == 0)
            {
                printf("word is spelled correctly.\n");
                return;
            }
            current = current->next;
        }
    }

    int wordCount = 0;
    printf("Did you mean:\n");
    //if not found in linked list find 5 closest
    for (int i = 0; i < map->capacity; i++)
    {
        current = map->table[i];
        while (current != NULL)
        {
            if (current->value == 1)
            {
                printf(current->key);
                printf("\n");
                wordCount++;
                if (wordCount == 5)
                {
                    return;
                }
            }
            if (current->value == 2)
            {
                printf(current->key);
                printf("\n");
                wordCount++;
                if (wordCount == 5)
                {
                    return;
                }
            }
            current = current->next;
        }
    }
}

int main(int argc, const char** argv)
{
    //printf("started");
    HashMap* map = hashMapNew(1000);

    FILE* file = fopen("dictionary.txt", "r");
    clock_t timer = clock();
    //printf("loading");
    loadDictionary(file, map);
    //printf("loaded");
    //hashMapPrint(map);
    timer = clock() - timer;
    printf("\nDictionary loaded in %f seconds\n", (float)timer / (float)CLOCKS_PER_SEC);
    fclose(file);

    char inputBuffer[256];
    int quit = 0;
    while (!quit)
    {
        printf("Enter a word or \"quit\" to quit: ");

        scanf("%s", inputBuffer);

        for (int i = 0; i < strlen(inputBuffer); i++)
        {
            inputBuffer[i] = tolower(inputBuffer[i]);
        }


        spellcheck(inputBuffer, map);

        if (strcmp(inputBuffer, "quit") == 0)
        {
            quit = 1;
        }

        fflush(stdin);

    }

    hashMapDelete(map);
    return 0;
}
