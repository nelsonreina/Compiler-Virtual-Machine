// Nelson Reina
// Lex Analyzer
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAXWORDS 15 // number of reserved words
#define MAXDIGITS 5  // maximum number of digits in the integer value 
#define MAXCHAR 11 // maximum number of chars for each indents
#define NUMSYMBOLS 15 // number of symbols

// Token Type Declaration
typedef enum{ nulsym = 1, identsym, numbersym, plussym, minussym, multsym, slashsym, 
            oddsym, eqsym, neqsym, lessym, leqsym, gtrsym, geqsym, lparentsym, rparentsym, 
            commasym, semicolonsym, periodsym,becomessym, beginsym, endsym, ifsym, thensym, 
            whilesym, dosym, callsym, constsym, varsym, procsym, writesym, readsym , elsesym } token_type;

// Token Structure
typedef struct
{ 
    token_type type; 
    char name[MAXCHAR];
    int n;
}token; 


// Global variables
token tokens[1000];
char *word[] = {"null", "begin", "call", "const", "do", "else", "end", "if", "odd", "procedure", "read", "then", "var", "while", "write"}; 
char symbols[] = {'+', '-', '*', '/', '(', ')','{','}', '=', ',' , '.', '<', '>',  ';' , ':'};
char charArray[MAXCHAR];
int numTokens = 0;

// Function Declaration
void getInput();
void output();
int isSymbol(char c);
int isReservedWord(char *s, int length);
token_type getTokenType(char *s, int length);


int LexMain()
{
    // Process input by turning each character into a token
    getInput();

    // Outputs the array of tokens
    output();
}


void getInput()
{
    
    // Opens file to be read from
    FILE* file = fopen("input.txt", "r");

    if (file == NULL)
    {
        //printf("File failed to open.");
        fclose(file);
    }
    
    // Current character being tokenized
    char currentToken;
    currentToken = fgetc(file);

    // Go through the file character by character
    while(currentToken != EOF)
    {
        // Empties the string
        memset(charArray,' ', MAXCHAR);

        // ignore because it's an invisible character
        if(currentToken == ' ' || currentToken == '\t' || currentToken == '\n')
        {
            currentToken = fgetc(file);
            continue;
        }

        // Process alphabet characters
        else if(isalpha(currentToken))
        {
            // adds alphacbet character to array    
            int wordIndex = 0;
            charArray[wordIndex++] = currentToken;
            
            // continue reading until there's a symbol or it's too long or there's a space
            currentToken = fgetc(file);
            while(currentToken != EOF  && (isalpha(currentToken) || isdigit(currentToken)) && wordIndex <= MAXCHAR)
            {
                charArray[wordIndex++] = currentToken;
                currentToken = fgetc(file);
            }
            // Print error if the word is too big
            if(wordIndex>=MAXCHAR)
            {
                //printf("Name is too big %s",charArray);
                fclose(file);
                exit(0);
            }

            // Turns the word into a token and add its to the token array
            if(isReservedWord(charArray, wordIndex))
            {
                tokens[numTokens].type = getTokenType(charArray,wordIndex);
                strncpy(tokens[numTokens].name, charArray, wordIndex);
                tokens[numTokens].n = (int)(getTokenType(charArray, wordIndex));
            }
            else
            {

              tokens[numTokens].type = identsym;
              strncpy(tokens[numTokens].name, charArray, wordIndex);
              tokens[numTokens].n = (int)(identsym);
            }

            numTokens++;
        }
        // Process a number
        else if(isdigit(currentToken))
        {
            // Adds number to array
            int numLength = 0;
            charArray[numLength++] = currentToken;
            currentToken = fgetc(file);

            // Continues adding onto the number until theres no longer a number or the length is too big
            while(currentToken != EOF)
            {

                if(isdigit(currentToken) && numLength <= MAXDIGITS && !isalpha(currentToken) )
                {

                    charArray[numLength++] = currentToken;
                    
                }
                // Prints error when the number is too big
                else if(numLength>=MAXDIGITS)
                {
                        //printf("Number is too big %s",charArray);
                        fclose(file);
                        exit(0);
                }
                // Prints error when the variable starts with a nunber 
                else if(isalpha(currentToken))
                {
                        //printf("Variable does not start with letter %s",charArray);
                        fclose(file);
                        exit(0);
                }
                else
                {
                    break;
                }
                currentToken = fgetc(file);  
            }

            // Creates a token and adds it to token array
            tokens[numTokens].type = numbersym;
            strncpy(tokens[numTokens].name, charArray, numLength);
            sscanf(charArray, "%d", &tokens[numTokens].n);
            //tokens[numTokens].n = (int)(numbersym);
            
   

            numTokens++;
        }
        // Process Symbol
        else
        {
            // If not a valid symbol then print error
            if(isSymbol(currentToken)==0)
            {
                    //printf("Invalid Symbol %c \n",currentToken);
                    fclose(file);
                    exit(0);

            }
            
            // Adds symbol to array
            int symbolLength = 0;
            charArray[symbolLength++] = currentToken;
          
            int isComment = 0;

            currentToken = fgetc(file);

            // Goes through until the end of file
            while(currentToken != EOF)
            {
                // Skips through a comment once it is found
                if(isSymbol(currentToken) && symbolLength == 1 && charArray[0] == '/' && currentToken == '*')
                {
                    isComment = 1;
                    charArray[1] = currentToken;
                    currentToken = fgetc(file);
                    while(currentToken != EOF)
                    {
                        if(currentToken == '*')
                        {

                            currentToken = fgetc(file);
                            if(currentToken == '/')
                            {                                
                                currentToken = fgetc(file);
                                //printf("Found Comments: %s \n",charArray);
                                break;
                            }
                        }
                        currentToken = fgetc(file);
                    }
                } 
                // Checks for the symbol ":="
                else if(currentToken=='=' && charArray[0]==':')
                {
                    charArray[symbolLength++] = currentToken;
                }
                 else if(currentToken=='=' && charArray[0]=='>' || currentToken=='=' && charArray[0]=='<' || currentToken=='*' && charArray[0]=='\\')
                {
                    charArray[symbolLength++] = currentToken;
                }
                // if it's not a symbol we're done with this symbol
                else
                {
                    break;
                }
                if(isComment==1)
                    break;

                if(currentToken != EOF)
                    currentToken = fgetc(file);  

                
            }

            // Creates a token and adds it token array
            tokens[numTokens].type = getTokenType(charArray, symbolLength);
            strncpy(tokens[numTokens].name, charArray, symbolLength);
            tokens[numTokens].n = (int)(getTokenType(charArray, symbolLength));
            numTokens++;
            
        }

    }

    fclose(file);
}
void output()
{

    FILE* output = fopen("lex_output.txt", "w");
 
  
    // Prints source code
    //printf("Source Program:\n");
    fprintf(output, "Source Program:\n");
    FILE* file = fopen("input.txt", "r");
    char t;
    t = fgetc(file);

    while(t != EOF)
    {
        //printf("%c",t);
        fprintf(output,"%c",t);

        t = fgetc(file);
    }



    // Prints lexeme table along with the token type
    //printf("\n\nLexeme Table:\nlexeme     token type\n");
    fprintf(output, "\nLexeme Table:\nlexeme     token type\n");
    for(int x = 0; x < numTokens ; x++)
    {
        //printf("%s      %d\n",tokens[x].name,tokens[x].n);
        fprintf(output,"%s      %d\n",tokens[x].name,tokens[x].n);
    }

    // Prints lexeme list
    //printf("\nLexeme List:\n");
    fprintf(output,"\nLexeme List:\n");
    for (int x = 0; x < numTokens; x++)
    {
        //printf("%d ", tokens[x].type);
        fprintf(output,"%d ", tokens[x].type);
        
        if (tokens[x].type == 2)
        {
            //printf("%s ", tokens[x].name);
            fprintf(output,"%s ", tokens[x].name);
            
            
        }
        if (tokens[x].type == 3)
        {
            //printf("%s ", tokens[x].name);
            fprintf(output,"%s ", tokens[x].name);
            
            
        }

    }
    //printf("\n");
    fclose(file);
    fclose(output);
    
}
// Checks to see if word is a valid symbol
int isSymbol(char c)
{
    int i;
    for(int i = 0; i < NUMSYMBOLS; i++)
    {
        if(c == symbols[i])
            return 1;
    }
    return 0;
}
// Checks to see if a word is a reserced word
int isReservedWord(char *s, int length)
{
    int i;
    for (i = 0; i < MAXWORDS; i++)
        if (strncmp(s, word[i], strlen(word[i])) == 0)
            return 1;
    return 0;
}

// Converts name to token type
token_type getTokenType(char *s, int length)
{
    if (strncmp(s, "null", length) == 0)
        return nulsym;
    if (strncmp(s, "call", length) == 0)
        return callsym;
    if (strncmp(s, "const", length) == 0)
        return constsym;
    if (strncmp(s, "do", length) == 0)
        return dosym;
    if (strncmp(s, "else", length) == 0)
        return elsesym;
    if (strncmp(s, "if", length) == 0)
        return ifsym;
    if (strncmp(s, "odd", length) == 0) 
        return oddsym;
    if (strncmp(s, "procedure", length) == 0)
        return procsym;
    if (strncmp(s, "read", length) == 0)
        return readsym;
    if (strncmp(s, "then", length) == 0)
        return thensym;
    if (strncmp(s, "var", length) == 0)
        return varsym;
    if (strncmp(s, "while", length) == 0)
        return whilesym;
    if (strncmp(s, "write", length) == 0)
        return writesym;
     if (strncmp(s, "begin", length) == 0)
        return beginsym;
    if (strncmp(s, "end", length) == 0)
        return endsym;
    // Symbols
    if (strncmp(s, "+", length) == 0)
        return plussym;
    if (strncmp(s, "-", length) == 0)
        return minussym;
    if (strncmp(s, "*", length) == 0)
        return multsym;
    if (strncmp(s, "/", length) == 0)
        return slashsym;
    if (strncmp(s, "(", length) == 0)
        return lparentsym;
    if (strncmp(s, ")", length) == 0)
        return rparentsym;
    if (strncmp(s, "{", length) == 0)
        return beginsym;
    if (strncmp(s, "}", length) == 0)
        return endsym;
    if (strncmp(s, "=", length) == 0)
         return eqsym;
    if (strncmp(s, ",", length) == 0)
        return commasym;
    if (strncmp(s, ".", length) == 0)
        return periodsym;
    if (strncmp(s, "<", length) == 0)
        return lessym;
    if (strncmp(s, "<>", length) == 0)
         return neqsym;
    if (strncmp(s, "<=", length) == 0)
        return leqsym;
    if (strncmp(s, ">", length) == 0)
        return gtrsym;
    if (strncmp(s, ">=", length) == 0)
        return geqsym;
    if (strncmp(s, ";", length) == 0)
        return semicolonsym;    
    if (strncmp(s, ":=", length) == 0)
        return becomessym;

    return -1;
}