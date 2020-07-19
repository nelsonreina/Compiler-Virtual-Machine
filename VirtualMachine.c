// Nelson Reina
// COP 3402 Summer 2020 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Constants
int MAX_DATA_STACK_HEIGHT = 1000;
int MAX_CODE_LENGTH = 500;

// Registers
int SP;
int BP;
int PC;
int IR;

int* stack;

// Flags
int halt;

// Instruction Structure
struct instruction
{
	int op;
	int l;
	int m;
};

// Find base L levels down
int base(int l, int base)
{
   int b1; 
   b1 = base;
   while (l > 0)
   {
   		b1 = stack[b1 - 1];
   		l--;
   }
   return b1;
}

// Fetches next instruction using PC
// Also increments PC after execution
struct instruction fetch(struct instruction *instructions)
{
	return instructions[PC++];
}

// Executes the instruction passed
void execute(struct instruction IR)
{
	int c = PC-1;

	// Each if statement represents a procedure to be executed based on the instruction

	// LIT
	if(IR.op == 1)
	{
		SP = SP - 1;
		stack[SP] = IR.m;
	}
	//OPR
	if(IR.op == 2)
	{
		// RET
		if(IR.m == 0 )
		{
			SP = BP +1;
			PC = stack[SP-4];
			BP = stack[SP-3];
		}	
		// NEG
		if(IR.m == 1)
		{
			stack[SP] = -1*stack[SP];
		}
		// ADD
		if(IR.m == 2)
		{
			SP = SP +1;
			stack[SP] = stack[SP]+stack[SP-1];
		}
		// SUB
		if(IR.m == 3)
		{
			SP = SP +1;
			stack[SP] = stack[SP]-stack[SP-1];
		}
		// MUL
		if(IR.m == 4)
		{
			SP = SP +1;
			stack[SP] = stack[SP]*stack[SP-1];
		}
		// DIV
		if(IR.m == 5)
		{
			SP = SP +1;
			stack[SP] = stack[SP]/stack[SP-1];
		}
		// ODD
		if(IR.m == 6)
		{
			stack[PC] = stack[PC] % 2;
		}
		// MOD
		if(IR.m == 7)
		{
			SP = SP +1;
			stack[SP] = stack[SP]%stack[SP-1];
		}
		// EQL
		if(IR.m == 8)
		{
			SP = SP +1;
			stack[SP] = stack[SP]==stack[SP-1];
		}
		// NEQ
		if(IR.m == 9)
		{
			SP = SP +1;
			stack[SP] = stack[SP]!=stack[SP-1];
		}
		// LSS
		if(IR.m == 10)
		{
			SP = SP +1;
			stack[SP] = stack[SP]<stack[SP-1];
		}
		// LEQ
		if(IR.m == 11)
		{
			SP = SP +1;
			stack[SP] = stack[SP]<=stack[SP-1];
		}
		// GTR
		if(IR.m == 12)
		{
			SP = SP +1;
			stack[SP] = stack[SP]>stack[SP-1];
		}
		// GEQ
		if(IR.m == 13)
		{
			SP = SP +1;
			stack[SP] = stack[SP]>=stack[SP-1];
		}
	}
	// LOD
	if(IR.op == 3)
	{
		SP = SP-1;
		stack[SP] = stack[base(IR.l,BP)-IR.m];
	}
	// STO
	if(IR.op == 4)
	{
		stack[base(IR.l,BP)-IR.m] = stack[SP];
		SP = SP+1;
	}
	// CAL
	if(IR.op == 5)
	{
		stack[SP-1] = 0; // Return value
		stack[SP-2] = base(IR.l,BP); // static link
		stack[SP-3] = BP; // dynamic address
		stack[SP-4] = PC; // return address
		BP = SP - 1;
		PC = IR.m;
	}
	// INC
	if(IR.op == 6)
	{
		SP = SP - IR.m;	
	}
	// JMP
	if(IR.op == 7)
	{
		PC = IR.m;
	}
	// JPC
	if(IR.op == 8)
	{
		if(stack[SP]==0)
		{
			PC = IR.m;
		}
		SP = SP + 1;
	}
	// SIO
	if(IR.op == 9)
	{
		printf("%d\n",stack[SP]);
		SP = SP + 1;
	}
	// SIO
	if(IR.op == 10)
	{
		SP = SP - 1;
		printf("Please input a value.");
           scanf("%d", &stack[SP]);
	}
	// SIO
	if(IR.op == 11)
	{
		halt = 1;
	}

	FILE* output = fopen("VM_output.txt", "a");

	// Gets the instruction name off the OP code
	char instructions[11][3] = {"LIT","OPR","LOD","STO","CAL","INC","JMP","JPC","SIO","SIO","SIO"};
	char instruct[3];
	memcpy(instruct, &instructions[IR.op-1],3);

	// Prints current registers
	fprintf(output,"%d %s %d %d 		%d 		%d 		%d 		",c,instruct, IR.l, IR.m,PC,BP,SP);


	// Doesnt print registers on termination
	if(IR.op==11)
	{
		fprintf(output,"\n");
		return;
	}

	// Prints stack
	if(BP==999 || BP == 0)
	{
		for(int x = BP ; x >= SP; x--)
		{
			fprintf(output,"%d ",stack[x]);
		}
	}
	else
	{
		for(int x = 999 ; x > BP ; x--)
		{
			fprintf(output,"%d ",stack[x]);
		}
		fprintf(output," | ");
		for(int x = BP ; x >= SP; x--)
		{
			fprintf(output,"%d ",stack[x]);
		}
	}

	fprintf(output,"\n");
	fclose(output);


	
}

// Prints assemly language representation
void printAssembly(struct instruction a[])
{
	FILE* output2 = fopen("VM_output.txt", "w");
	// Gets the instruction name off the OP code
	int i = 0 ;
	char instructions[11][3] = {"LIT","OPR","LOD","STO","CAL","INC","JMP","JPC","SIO","SIO","SIO"};
	fprintf(output2,"Line   OP  	 L 	 M\n");

	
	while(a[i].op!=0)
	{
		char instruct[3];
		memcpy(instruct, &instructions[a[i].op-1],3);
		
		fprintf(output2,"%d     %s  	 %d  	 %d\n",i,instruct,a[i].l,a[i].m);
		i++;
	}
	fprintf(output2,"\n");

	fprintf(output2,"Initial Values 		PC		BP 		SP 		Stack\n");
	fclose(output2);
}


int VirtualMachineMain()
{
	
	
	//Initialize default values
	SP = MAX_DATA_STACK_HEIGHT;
	BP = SP-1;
	PC = 0;
	IR = 0;
	stack = (int*) malloc(16 * sizeof(int));
	halt = 0;
	
	// File Input
	FILE *stream = fopen("assemblyLanguage.txt","r");
	char buffer[20];

	// Creates an array of all the instructions
	struct instruction *instructions = (struct instruction *) malloc(MAX_CODE_LENGTH * sizeof(struct instruction));
	int count = 0;
	
	// Adds instructions into the instruction array
	while (fscanf(stream, "%d %d %d", &instructions[count].op, &instructions[count].l, &instructions[count].m) != EOF)
        count = count + 1;

 	if (feof(stream))
 	{
   		//printf("EOF\n");
 	}
 	else
 	{
   		printf("Error reading file");
 	}

 	printAssembly(instructions);

 	

 	// Main Loop running until halt signal is sent
 	while(halt==0)
 	{
 		execute(fetch(instructions));	
 	}
 	free(instructions);
 	free(stack);
 	fclose(stream);


  	
  	return 0;
}