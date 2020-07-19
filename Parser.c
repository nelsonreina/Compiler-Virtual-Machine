// Nelson Reina
// COP 3402 Summer 2020 Parser-code-generator source code
#include <stdio.h>
#include <string.h>
#include "LexAnalyzer.c"
#include "VirtualMachine.c"

#define MAX_SYMBOL_TABLE_SIZE 200
#define MAX_ASSEMBLYCODE_TABLE_SIZE 1000

typedef struct{ 
    int kind; // const = 1, var = 2, proc = 3
    char name[10];// name up to 11 chars
    int val; // number (ASCII value) 
    int level; // L level
    int addr; // M address
    int mark;// to indicate unavailable or delete d  

    }symbol; 

// Symbol Table
symbol symbol_table[MAX_SYMBOL_TABLE_SIZE];

// Assembly Code Array
struct instruction assemblyCode[MAX_ASSEMBLYCODE_TABLE_SIZE];

//Global Variables
int assemblyIndex,symbolIndex,tokenIndex,level;


// Function Declarations
void addAssemblyCode(int op, int l, int m);
void addSymbol(int type, char* name, int address, int val);
void block();
void printError(int num);
void statement();
void condition();
void expression();
void term();
void factor();
void outputAssemblyCode();
void outputAll();

int main(int numInputs, char *args[])
{

    // Initial Declarations
    FILE *inputFile;
    FILE *outputFile;

    // Initial Values
    assemblyIndex = 0;
    tokenIndex = 0;
    int token = 0, assembly = 0, execution = 0;

    char** input = args;
    
    // Reads in compiler directives
    for(int i = 0; i < numInputs; i++)
    {
        if(strcmp(input[i], "-l") == 0)
        {
            token = 1;
        }
        if(strcmp(input[i], "-a") == 0)
        {
            assembly = 1;
        }
        if(strcmp(input[i], "-v") == 0)
        {
            execution = 1;
        }
    }

    // Generates Tokens
    LexMain();

    // Begins Parser Code
    block();


    // Error to seee if program doesnt end with a .
    if(tokens[tokenIndex].type != periodsym)
    {
        printError(9);
    }
    
    // Adds terminal code
    addAssemblyCode(11,0,3);


    // Generates Assemvly Code
    outputAssemblyCode();


    // Runs Virtual Machine
    VirtualMachineMain();


    // Outputs everything to a single file
    outputAll();



    // Prints output based off directives
    char c;

    if(token == 1)
    {
        FILE* input = fopen("lex_output.txt", "r");
        while((c = fgetc(input)) != EOF)
        {
            printf("%c", c);
        }
            
        printf("\n\n");

        fclose(input);
       
    } 
    
    if(assembly == 1)
    {
        
        FILE* file1 = fopen("assemblyLanguage.txt", "r");
        
        while((c = fgetc(file1)) != EOF)
        {
            printf("%c", c);

        }
        printf("\n\n");

        
        fclose(file1);
    }

    if(execution == 1)
    {
        FILE* file2 = fopen("VM_output.txt", "r");
        while((c = fgetc(file2)) != EOF)
        {
            printf("%c", c);

        }
            
        printf("\n\n");

        fclose(file2);
      
    }

    if( token == 0 && assembly == 0 && execution == 0)
    {
        FILE* file3 = fopen("input.txt", "r");
        while((c = fgetc(file3)) != EOF)
        {
            printf("%c", c);

        }
            
        printf("\n\n");

        fclose(file3);
    }

}

void block()
{

    // Initial Values
    char name[MAXCHAR];
    int stp = 5, memAddress = 4, level = 0, px = 0, jmp = assemblyIndex;

    //addAssemblyCode(7,0,0);

    // Generates Assembly Language for constants
    if(tokens[tokenIndex].type == constsym)
    {
        do
        {
            tokenIndex++; 

            // Invalid variable name error
            if(tokens[tokenIndex].type != identsym)
                printError(4);

            strcpy(name, tokens[tokenIndex].name);

            tokenIndex++;

            // Missing equal sign error
            if(tokens[tokenIndex].type != eqsym)
            {
                printError(3);
            }
            tokenIndex++;

            // Error for setting const equal to not a number
            if(tokens[tokenIndex].type != numbersym)
            {
                printError(2);
            }

            // Adds symbol to symbol table
            addSymbol(1, name, 0, tokens[tokenIndex].n);

            tokenIndex++;
            
        }while(tokens[tokenIndex].type == commasym);

        // Missing Semicolon error
        if(tokens[tokenIndex].type != semicolonsym)   
        {
            printError(5);
        }
        
        tokenIndex++;
    }




    // Generates Assembly Language for variables
    if(tokens[tokenIndex].type == varsym)
    {
        do
        {
            tokenIndex++;
            
            // Invalid variable name error
            if(tokens[tokenIndex].type != identsym)
                printError(4);

            // Adds variable to symbol table
            strcpy(name, tokens[tokenIndex].name);
            addSymbol(2, name, stp, 0);
            symbol_table[symbolIndex - 1].level = level;
            memAddress++;
            stp++;

            tokenIndex++;   
        } while(tokens[tokenIndex].type == commasym);

        // Missing semicolon error
        if(tokens[tokenIndex].type != semicolonsym)   
        {
            printError(5);
        }
        tokenIndex++;
    }


    assemblyCode[jmp].m = assemblyIndex;
    addAssemblyCode(6, 0, memAddress);
    statement();

    if (jmp != 0)
    {
        addAssemblyCode(2, 0, 0);

    }




    
}

void statement()
{
    
    // Generates Assembly Language for identities
    if (tokens[tokenIndex].type == identsym)
    {
        int j, eq = 0;

        // Finds the symbol in the table
        for (j = 0; j <= symbolIndex; j++)
        {
            
            if(strcmp(tokens[tokenIndex].name, symbol_table[j].name) == 0)
            {
                eq = 1;
                break;
            }
        }

        // Couldnt find symbol error
        if (!eq)
        {
            printError(11);
        }
        tokenIndex++;

        // Assigment operation error
        if (tokens[tokenIndex].type != becomessym)
            printError(13);


        tokenIndex++;
        expression();
        addAssemblyCode(4, symbol_table[j].level, symbol_table[j].addr - 1);
        
    }

    // Generates Assembly language for if statements
    else if(tokens[tokenIndex].type == ifsym)
    {
        tokenIndex++;
        condition();

        // Mussing then statement error
        if (tokens[tokenIndex].type != thensym)
        {
            printError(16);
        }
        tokenIndex++;
        int newIndex = assemblyIndex;
        addAssemblyCode(8, 0, 0);
        
        // Statement inside of if
        statement();


        // Semicolon missing error
        while(tokens[tokenIndex].type == semicolonsym)
        {
            tokenIndex++;
            statement();
        }


        // Where else statement would be implemented
        if(tokens[tokenIndex].type == elsesym)
        {
           

        }
        else
        {
            assemblyCode[newIndex].m = assemblyIndex;
 
        }
    }
    // Generate Assembly Language for while statements
    else if(tokens[tokenIndex].type == whilesym)
    {
        int tempIndex1 = assemblyIndex;
        int tempIndex2;
        tokenIndex++;

        // Condition for while loop
        condition();

        // Stores spot to jump back to 
        tempIndex2 = assemblyIndex;
        addAssemblyCode(8, 0, 0);
        
        // Missing do error
        if(tokens[tokenIndex].type != dosym)
        {
            printError(18);
        }
        tokenIndex++;

        statement();

        // Jumps back to address
        addAssemblyCode(7,  0, tempIndex1);
        
        assemblyCode[tempIndex2].m = assemblyIndex;

    }

    // Generates assembly code for begin statements which replaces {}
    else if(tokens[tokenIndex].type == beginsym)
    {
        tokenIndex++;

        // Statement inside of {}
        statement();

        // Missing semicolon error
        if(tokens[tokenIndex].type != semicolonsym) 
        {
            printError(10);
        }

        // While theres code inside continue to create statements
        while(tokens[tokenIndex].type == semicolonsym)
        {
            tokenIndex++;
            statement();
        }


        // Missing end symbol error
        if(tokens[tokenIndex].type != endsym)
        {
            printError(8);
        }
        tokenIndex++;
    }   


    // Generates assembly code for read statements
    else if(tokens[tokenIndex].type == readsym)
    {
        tokenIndex++;

        // No symbol to write to error
        if(tokens[tokenIndex].type != identsym)
            printError(29);

        int j, eq = 0;

        // Looks to find to symbol in table
        for (j = 0; j <= symbolIndex; j++)
        {
            
            if(strcmp(tokens[tokenIndex].name, symbol_table[j].name) == 0)
            {
                eq = 1;
                break;
            }
        }
        // Cant find symbol in table error
        if (!eq)
        {
            printError(11);
        }

        // Read command assembly language
        addAssemblyCode(10, 0, 2);

        // Stores the input
        if(symbol_table[symbolIndex-1].kind == 2)
            addAssemblyCode(4, level - symbol_table[symbolIndex-1].level, symbol_table[symbolIndex-1].addr - 1);
        else if(symbol_table[symbolIndex-1].kind == 1)
            printError(12); 
        else
        {

        }

        tokenIndex++; 
    }
    // Generates assembly code for write statements
    else if(tokens[tokenIndex].type == writesym){
        tokenIndex++;

        // No symbol to write to error
        if(tokens[tokenIndex].type != identsym) 
            printError(29);
        int j, eq = 0;

        // Looks to find to symbol in table
        for (j = 0; j <= symbolIndex; j++)
        {
            
            if(strcmp(tokens[tokenIndex].name, symbol_table[j].name) == 0)
            {
                eq = 1;
                break;
            }
        }
        // Cant find symbol in table error
        if (!eq)
        {
            printError(11);
        }

        // Write commands
        if(symbol_table[symbolIndex-1].kind == 2)
        {
            addAssemblyCode(3, level - symbol_table[symbolIndex-1].level, symbol_table[symbolIndex-1].addr - 2);
            addAssemblyCode(9, 0, 1); 
        }

        else if(symbol_table[symbolIndex-1].kind == 1){
            addAssemblyCode(1, 0, symbol_table[symbolIndex-1].val );
            addAssemblyCode(9, 0, 1);
        }

        tokenIndex++;
    }
}

void condition()
{
    // Odd operation
    if (tokens[tokenIndex].type == oddsym)
    {
        tokenIndex++;
        expression();
        addAssemblyCode(2, 0, 6);
    }

    else
    {
        expression();
        
        if(tokens[tokenIndex].type != gtrsym && tokens[tokenIndex].type != lessym && tokens[tokenIndex].type != neqsym && tokens[tokenIndex].type != eqsym && tokens[tokenIndex].type != geqsym && tokens[tokenIndex].type != leqsym)
        {
            printError(20);
        }
        
        int newIndex = tokenIndex;
        tokenIndex++;
        expression();
        if (tokens[newIndex].type == eqsym)
        {
            addAssemblyCode(2, 0, 8);
            

        }   
        else if (tokens[newIndex].type == neqsym)
        {
            addAssemblyCode(2, 0,9);
         
        }
        else if (tokens[newIndex].type == lessym)
        {
            addAssemblyCode(2, 0,10);
        }
        else if(tokens[newIndex].type == leqsym)
        {
            addAssemblyCode(2, 0,11);
        }
        else if(tokens[newIndex].type == gtrsym)
        {
            addAssemblyCode(2, 0,12);
        }
        else if(tokens[newIndex].type == geqsym)
        {
            addAssemblyCode(2, 0,13);
        }
    }
}

void expression()
{
    int temp = 0;

    // Relation operator expectation error
    if(tokens[tokenIndex].type == lessym || tokens[tokenIndex].type == multsym || tokens[tokenIndex].type == eqsym || tokens[tokenIndex].type == semicolonsym || tokens[tokenIndex].type == commasym || tokens[tokenIndex].type == periodsym || tokens[tokenIndex].type == gtrsym || tokens[tokenIndex].type == slashsym)
        printError(24); 

    // Follows Grammar
    if(tokens[tokenIndex].type == plussym || tokens[tokenIndex].type == minussym)
    {
        tokenIndex++;
        temp = tokens[tokenIndex].type;
        term();

        // Handles negative operations
        if (temp == minussym)
        {
            addAssemblyCode(2, 0, 3);
       
        }
    }
    else
        term();

    while(tokens[tokenIndex].type == plussym || tokens[tokenIndex].type == minussym)
    {
        temp = tokens[tokenIndex].type;
        tokenIndex++;
        term();

        // Add operation
        if (temp == plussym)
        {   
            addAssemblyCode(2,0,2);
        }

        // Subtract operation
        else if (temp == minussym)
        {   
            addAssemblyCode(2, 0, 3);
        }
        else // unexpected operation
        {
           printError(24);
        }
    }
}

void term()
{
    int temp = 0;
    factor();
    // Multiply and Divide Operations
    while (tokens[tokenIndex].type == multsym || tokens[tokenIndex].type == slashsym)
    {
        temp = tokens[tokenIndex].type;
        tokenIndex++;
        factor();    
        
        // Multiplication operation
        if(temp == multsym)
        {
            addAssemblyCode(2, 0, 4);
        }
        // Division operation
        else if(temp == slashsym)
        {
            addAssemblyCode(2, 0, 5);
        }
    }
}

void factor()
{

    // Ident Check
    if(tokens[tokenIndex].type == identsym)
    {
        int j, eq = 0;

        // Looks for symbol in table
        for (j = 0; j <=symbolIndex ; j= j + 1)
        {

            if(strcmp(tokens[tokenIndex].name, symbol_table[j].name)==0)
            {
                eq = 1;
                break;
            }
        }
        // Cant find symbol error
        if (!eq)
        {
            printError(11);
        }
        //Place identifier on stack for operations.
        else if(symbol_table[j].kind == 1)
        {   
            addAssemblyCode(1, 0, symbol_table[j].val);
        }
        else if(symbol_table[j].kind == 2)
        {
            addAssemblyCode(3,  symbol_table[j].level, symbol_table[j].addr - 1);
        }
        
        tokenIndex++;
    }
    // If constant then just load from table
    else if(tokens[tokenIndex].type == numbersym)
    {       
        addAssemblyCode(1, 0, atoi(tokens[tokenIndex].name));
        tokenIndex++;
    }
    // Left and right parentheses operation
    else if(tokens[tokenIndex].type == lparentsym)
    {
        tokenIndex++;

        expression();
        
        // Missing right parenteses error
        if(tokens[tokenIndex].type != rparentsym)
        {
            printError(22);
        }
        tokenIndex++;
    }
    // No statement error
    else
    {
        printError(7);
    }
}

// Adds symbol to symbol table
void addSymbol(int type, char* name, int address, int val)
{
    symbol_table[symbolIndex].kind = type;
    strcpy(symbol_table[symbolIndex].name, name);
    symbol_table[symbolIndex].addr = address;
    symbol_table[symbolIndex].val = val;
    symbolIndex++;
}

// Adds assembly code to array
void addAssemblyCode(int op, int l, int m)
{

    if (symbolIndex > MAX_CODE_LENGTH)
        printError(26);

    assemblyCode[assemblyIndex].l = l;
    assemblyCode[assemblyIndex].m = m;
    assemblyCode[assemblyIndex].op = op; 

    assemblyIndex++;

}

// Creates an output file with all the ouput from each step of the compilation process
void outputAll()
{

    char c;

    FILE* outputAll = fopen("all_output.txt", "w");

   
    FILE* input = fopen("lex_output.txt", "r");
    while((c = fgetc(input)) != EOF)
    {
        fprintf(outputAll, "%c", c);
    }

    fprintf(outputAll, "\n\n");
    fclose(input);
       
    
    FILE* file1 = fopen("assemblyLanguage.txt", "r");
    while((c = fgetc(file1)) != EOF)
    {
        fprintf(outputAll, "%c", c);
    }

    fprintf(outputAll, "\n\n");
    fclose(file1);
    

    
    FILE* file2 = fopen("VM_output.txt", "r");
    while((c = fgetc(file2)) != EOF)
    {
        fprintf(outputAll, "%c", c);
    }
    fprintf(outputAll, "\n\n");
    fclose(file2);
      
    
    fclose(outputAll);

}

// Prints all possible error 
void printError(int num)
{

    printf("Error With this token:%s   Token#:%d\n",tokens[tokenIndex].name,tokenIndex);

    switch(num)
    {
        case 1:
            printf("Error #1: Use = instead of :=.\n");
            break;
        case 2:
            printf("Error #2: = must be followed by a number.\n");
            break;
        case 3:
            printf("Error #3: Identifier must be followed by =.\n");
            break;
        case 4:
            printf("Error #4: 'const', 'var', 'procedure' must be followed by identifier.\n");
            break;
        case 5:
            printf("Error #5: Semicolon or comma missing.\n");
            break;
        case 6: 
            printf("Error #6: Incorrect symbol after procedure declaration.\n");
            break;
        case 7: 
            printf("Error #7: Statement expected.\n");
            break;
        case 8:
            printf("Error #8: Incorrect symbol after statement part in block.\n");
            break;
        case 9:
            printf("Error #9: Period expected.\n");
            break;
        case 10:
            printf("Error #10: Semicolon between statements missing.\n");
            break;
        case 11:
            printf("Error #11: Undeclared identifier.\n");
            break;
        case 12:
            printf("Error #12: Assignment to constant or procedure is not allowed.\n");
            break;
        case 13:
            printf("Error #13: Assignment operator expected.\n");
            break;
        case 14:
            printf("Error #14: 'call' must be followed by an identifier.\n");
            break;
        case 15:
            printf("Error #15: Call of a constant or variable is meaningless.\n");
            break;
        case 16:
            printf("Error #16: 'then' expected.\n");
            break;
        case 17:
            printf("Error #17: Semicolon or } expected.\n");
            break;
        case 18:
            printf("Error #18: 'do' expected.\n");
            break;
        case 19:
            printf("Error #19: Incorrect symbol following statement.\n");
            break;
        case 20:
            printf("Error #20: Relational operator expected.\n");
            break;
        case 21:
            printf("Error #21: Expression must not contain a procedure identifier.\n");
            break;
        case 22:
            printf("Error #22: Right parenthesis missing.\n");
            break;
        case 23:
            printf("Error #23: The preceding factor cannot begin with this symbol.\n");
            break;
        case 24: 
            printf("Error #24: An expression cannot begin with this symbol.\n");
            break;
        case 25:
            printf("Error #25: This number is too large.\n");
            break;
        case 26: 
            printf("Error #26: Too many instructions.\n");
            break;
        default:
            break;
    }
    exit(0);
}



// Outputs assembly code to a text file
void outputAssemblyCode()
{
    FILE *output = fopen("assemblyLanguage.txt", "w");
    int i;
    for (i = 0; i < assemblyIndex; i++)
    {
        fprintf(output, "%d %d %d\n", assemblyCode[i].op, assemblyCode[i].l, assemblyCode[i].m);
    }

    fclose(output);
}
