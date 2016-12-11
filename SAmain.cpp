/*
Andrew Nguyen
Joshua Womack 
CPSC323
Assignment 3 - Intermediate Code Generator

*/

#include "LA.h"
//using namespace std;


void Rat16F();
void OptFuncDef();
void FuncDef();
void Func();
void OptParamList();
void ParamList();
void Parameter();
void Qualifier();
void Body();
void OptDecList();
void DecList();
void Declaration();
void IDs();
void StatementList();
void Statement();
void Compound();
void Assign();
void If();
void Return();
void Write();
void Read();
void While();
void Condition();
void Relop();
void Expression();
void ExpressionPrime();
void Term();
void TermPrime();
void Factor();
void Primary();
void Empty();
void lexAdv();
void generateInstruction(string opCode, int oprnd);
void backPatch(int jumpAddress);
void addDataTypeToSymbolTable(string dataType);
void printSymbolTable(vector<symbolData> &table, string filePath);
void printAssemblyCode(vector<instructionData> &instructions, string filePath);
int getAddress(string token);
bool alreadyInSymbolTable(string symbolIdentifier);


// TODO: Remove all 0 from Assembly Code output and replace with blank space


bool                        printSwitch = false;
int                         lineNumber = 0;
int                         tokenIndex = 0;                 //Index used to step through token vector
tokenData                   currentToken;
vector<tokenData>           tokens;                         //vector to hold tokens as they are being inputted
vector<tokenData>           tokenList;                      //vector that holds all tokens once they have been read in initially
ifstream			        ifget;
ofstream				    oftrace;

// ICG Additions
int                         memoryAddress = 6000;           // Physical memory address starts at 6000 and will get incremented when identifier gets placed in symbol table
int                         tempAddress = 0;
int                         tempInstructionNumber = 0;      // Temp place to hold the instruction number of an operation
int                         currentInstructionNumber = 1;   // Instructions start from 1 and increment after instruction gets placed in instruction table
stack<int>                  jumpStack;
string                      tempSaveToken;
vector<symbolData>          symbolTable;
vector<instructionData>     instructionTable;
bool						SymbolInsertDone = false;



int main() 
{
	LA                      lex;
	string				    current = "";
	string				    infilepath = "";
	string				    outfilepath = "";
	
/*
	//Input file to read from
	cout << "Before you begin, make sure the input test file is in\nthe same folder as the .exe of this program.\n";
	cout << "----------------------------------------------------------------------\n";
	cout << "Please enter the file name and extension of the input file (input2.txt).\n";
	cout << "Input: ";
	getline(cin, infilepath);
	cout << "You entered: " << infilepath << endl << endl;

	//Output file to write to
	cout << "Please enter the file and extension of the output file (output.txt).\n";
	cout << "Input: ";
	getline(cin, outfilepath);
	cout << "You entered: " << outfilepath << endl << endl;
*/


	ifget.open(infilepath);
	oftrace.open(outfilepath);

	infilepath = "input.txt";
	outfilepath = "output.txt";
	//infilepath = "/home/joshua/Git/323-intermediateCodeGenerator/cmake-build-debug/input.txt";
	//outfilepath = "/home/joshua/Git/323-intermediateCodeGenerator/cmake-build-debug/output.txt";
	ifget.open(infilepath);


	//Catch issue with opening file
	if (!ifget)
	{
		cout << "Error. Unable to read file." << endl;
		//system("pause");
		return -1;
	}

	//While not end of file, read every line.
	while (getline(ifget, current))
	{
		lineNumber++;
		tokens = lex.lexer(current, lineNumber);
		tokenList.insert(tokenList.end(), tokens.begin(), tokens.end());
		tokens.clear();
	}

	ifget.close();
	


	Rat16F();
	oftrace.close();
	printSymbolTable(symbolTable, outfilepath);
	printAssemblyCode(instructionTable, outfilepath);
	cout << "Results printed in output file.\n";
	system("pause");
	return 0;
}


void lexAdv() {
	if (tokenIndex < tokenList.size())
	{
		currentToken = tokenList[tokenIndex];
		if (printSwitch)
		{
			oftrace << "\nToken: " << left << setw(20) << currentToken.token
				 << left << setw(8) << "Lexeme: " << left << setw(20) << currentToken.lexeme << endl;
		}
		tokenIndex++;
	}
}


void Rat16F()
{
	lexAdv();
	if (printSwitch)
		oftrace << "\t<Rat16F> ::= $$ <Opt Function Definitions>\n\t\t\t\t $$ <Opt Declaration List> <Statement List> $$\n";

	if (currentToken.lexeme == "$$")
	{
		lexAdv();
		OptFuncDef();
		if (currentToken.lexeme == "$$")
		{
			//SymbolInsertDone = true;
			lexAdv();
			OptDecList();
			SymbolInsertDone = true;
			StatementList();
			if (currentToken.lexeme != "$$")
				//oftrace << "The End.\n";
			//else
			{
				oftrace << "\n<><><> Syntax Error, expecting last '$$' before '" << currentToken.lexeme << "' on line " << currentToken.lineNumber;
				exit(1);
			}
		}
		else
		{
			oftrace << "\n<><><> Syntax Error, expecting second '$$' before '" << currentToken.lexeme << "' on line " << currentToken.lineNumber;
			exit(1);
		}
	}
	else
	{
		oftrace << "\n<><><> Syntax Error, expecting first '$$' before '" << currentToken.lexeme << "' on line " << currentToken.lineNumber;
		exit(1);
	}
}


void OptFuncDef()
{
	if (printSwitch)
		oftrace << "\t<Opt Function Definition> ::= <Function Definitions> | <Empty>\n";

	if (currentToken.lexeme == "function")
		FuncDef();
	else if (currentToken.lexeme == "$$")
		Empty();
	else
	{
		oftrace << "\n<><><> Syntax Error, expecting 'function' or '$$' before '" << currentToken.lexeme << "' on line " << currentToken.lineNumber;
		exit(1);
	}

}


void FuncDef()
{
	if (printSwitch)
		oftrace << "\t<Function Definitions> ::= <Function> | <Function> <Function Definitions>\n";

	while (currentToken.lexeme == "function")
	{
		Func();
	}
}


void Func()
{
	if (printSwitch)
		oftrace << "\t<Function> ::= function <Identifier> [ <Opt Paramenter List> ] <Opt Declaration List> <Body>\n";

	lexAdv();

	if (currentToken.token == "IDENTIFIER")
	{
		lexAdv();
		if (currentToken.lexeme == "[")
		{
			lexAdv();
			OptParamList();
			if (currentToken.lexeme == "]")
			{
				lexAdv();
				OptDecList();
				Body();
			}
			else
			{
				oftrace << "\n<><><> Syntax Error, expecting ']' before '" << currentToken.lexeme << "' on line " << currentToken.lineNumber;
				exit(1);
			}
		}
		else
		{
			oftrace << "\n<><><> Syntax Error, expecting '[' before '" << currentToken.lexeme << "' on line " << currentToken.lineNumber;
			exit(1);
		}
	}
	else
	{
		oftrace << "\n<><><> Syntax Error, expecting <Identifier> before '" << currentToken.lexeme << "' on line " << currentToken.lineNumber;
		exit(1);
	}
}


void OptParamList()
{
	if (printSwitch)
		oftrace << "\t<Opt Parameter List> ::= <Parameter List> | <Empty>\n";

	if (currentToken.token == "IDENTIFIER")
	{
		ParamList();
	}
	else if (currentToken.lexeme == "]")
	{
		Empty();
	}
	else
	{
		oftrace << "\n<><><> Syntax Error, expecting <Identifier> before '" << currentToken.lexeme << "' on line " << currentToken.lineNumber;
		exit(1);
	}
}


void ParamList()
{
	if (printSwitch)
		oftrace << "\t<Parameter List> ::= <Parameter> | <Parameter>, <Parameter List>\n";


	if (currentToken.token == "IDENTIFIER")
	{
		Parameter();
		if (currentToken.lexeme == ",")
		{
			lexAdv();
			ParamList();
		}
	}
}


void Parameter()
{
	if (printSwitch)
		oftrace << "\t<Parameter> ::= <IDs> : <Qualifier>\n";

	IDs();
	if (currentToken.lexeme == ":")
	{
		lexAdv();
		Qualifier();
	}
	else
	{
		oftrace << "\n<><><> Syntax Error, expecting ':' before '" << currentToken.lexeme << "' on line " << currentToken.lineNumber;
		exit(1);
	}
}


void Body()
{
	if (printSwitch)
		oftrace << "\t<Body> ::= { <Statement List> }\n";

	if (currentToken.lexeme == "{")
	{
		lexAdv();
		StatementList();
		if (currentToken.lexeme == "}")
			lexAdv();
		else
		{
			oftrace << "\n<><><> Syntax Error, expecting '}' before '" << currentToken.lexeme << "' on line " << currentToken.lineNumber;
			exit(1);
		}
	}
	else
	{
		oftrace << "\n<><><> Syntax Error, expecting '{' before '" << currentToken.lexeme << "' on line " << currentToken.lineNumber;
		exit(1);
	}
}


void OptDecList()
{
	if (printSwitch)
		oftrace << "\t<Opt Declaration List> ::= <Declaration List> | <Empty>\n";


	if (currentToken.lexeme == "{")
		Empty();
	else if (currentToken.lexeme == "integer" || currentToken.lexeme == "boolean")  // || currentToken.lexeme == "real")
		DecList();
	else
	{
		oftrace << "\n<><><> Syntax Error, expecting 'integer', 'boolean', or 'real' before '" << currentToken.lexeme << "' on line: " << currentToken.lineNumber;
		exit(1);
	}
}


void DecList()
{
	if (printSwitch)
		oftrace << "\t<Declaration List> ::= <Declaration>; | <Declaration> ; <Declaration List>\n";

		Declaration();
		if (currentToken.lexeme == ";")
		{
			lexAdv();
			if (currentToken.lexeme == "integer" || currentToken.lexeme == "boolean")   // || currentToken.lexeme == "real")
				DecList();
		}
		else
		{
			oftrace << "\n<><><> Syntax Error, expecting ';' on line " << currentToken.lineNumber;
			exit(1);
		}
}


void Declaration()
{
	if (printSwitch)
		oftrace << "\t<Declaration> ::= <Qualifier> <IDs>\n";

	Qualifier();
	IDs();
}


void Qualifier()
{
	if (printSwitch)
		oftrace << "\t<Qualifier> ::= integer | boolean | real\n";

	if (currentToken.lexeme == "integer" || currentToken.lexeme == "true" 
		|| currentToken.lexeme == "false" || currentToken.lexeme == "boolean")  // || currentToken.lexeme == "real"
	{
		tempSaveToken = currentToken.lexeme;
		//addDataTypeToSymbolTable(tempSaveToken);
		lexAdv();
	}
	else
	{
		oftrace << "\n<><><> Syntax Error, expecting 'integer', 'boolean', or 'real' before '" << currentToken.lexeme << "' on line " << currentToken.lineNumber;
		exit(1);
	}
}


void IDs()
{
	if (printSwitch)
		oftrace << "\t<IDs> ::= <Identifier> | <Identifier>, <IDs>\n";

	if (currentToken.token == "IDENTIFIER")
	{
		if (!SymbolInsertDone)
		{
			if (!alreadyInSymbolTable(currentToken.lexeme))
			{
				addDataTypeToSymbolTable(tempSaveToken);
				symbolTable.back().identifier = currentToken.lexeme;                                                        //Changes the first item in symbol table rather than adding a new one
				symbolTable.back().memoryLocation = memoryAddress;
				memoryAddress++;
			}
			else if (alreadyInSymbolTable(currentToken.lexeme))
			{
				cout << "\n<><><> Redeclaration error: " << currentToken.lexeme << endl;
				system("pause");
				exit(1);
			}
		}
		
		if (SymbolInsertDone)
		{
			generateInstruction("POPM", getAddress(currentToken.lexeme));
		}

		lexAdv();
		if (currentToken.lexeme == ",")
		{
			lexAdv();
			IDs();
		}
		else if (currentToken.token == "IDENTIFIER")
		{
			oftrace << "\n<><><> Syntax Error, expecting ',' between multiple identifiers on line " << currentToken.lineNumber;
			exit(1);
		}
	}
	else
	{
		oftrace << "\n<><><> Syntax Error, expecting <Identifier> before '" << currentToken.lexeme << "' on line " << currentToken.lineNumber;
		exit(1);
	}
}

void StatementList()
{
	if (printSwitch)
		oftrace << "\t<Statement List> ::= <Statement> | <Statement> <Statement List>\n";

	while (currentToken.lexeme == "if" || currentToken.lexeme == "return" || currentToken.lexeme == "print"
			|| currentToken.lexeme == "read" || currentToken.lexeme == "while" || currentToken.token == "IDENTIFIER")
	{
		Statement();
	}
}


void Statement()
{
	if (printSwitch)
		oftrace << "\t<Statement> ::= <Compound> | <Assign> | <If> | <Return> | <Write> | <Read> | <While>\n";

	if (currentToken.lexeme == "{")
		Compound();
	else if (currentToken.token == "IDENTIFIER")
		Assign();
	else if (currentToken.lexeme == "if")
		If();
	else if (currentToken.lexeme == "return")
		Return();
	else if (currentToken.lexeme == "print")
		Write();
	else if (currentToken.lexeme == "read")
		Read();
	else if (currentToken.lexeme == "while")
		While();
	else
	{
		cout << "??? Statement error\n";
		system("pause");
		oftrace << "\n<><><> Syntax Error, expecting proper '<Statement>' before '" << currentToken.lexeme << "' on line " << currentToken.lineNumber;
		exit(1);
	}
}


void Compound()
{
	if (printSwitch)
		oftrace << "\t<Compound> ::= {<Statement List>}\n";

	if (currentToken.lexeme == "{")
	{
		lexAdv();
		StatementList();
		if (currentToken.lexeme == "}")
		{
			lexAdv();
		}
	}
}


// *ADDED CODE FROM PROMPT
void Assign()
{
	if (printSwitch)
		oftrace << "\t<Assign> ::= <Identifier> := <Expression>;\n";

	if (currentToken.token == "IDENTIFIER")
	{
		tempSaveToken = currentToken.lexeme;                         //*
		lexAdv();
		if (currentToken.lexeme == ":=")
		{
			lexAdv();
			Expression();
			tempAddress = getAddress(tempSaveToken);                // Checks symbol table if token is in there
			generateInstruction("POPM", tempAddress);     //*
			if (currentToken.lexeme == ";")
				lexAdv();
			else
			{
				oftrace << "\n<><><> <Assign> Syntax Error";
				exit(1);
			}
		}
		else
		{
			oftrace << "\n<><><> Syntax Error, expecting ':=' before '" << currentToken.lexeme << "' on line " << currentToken.lineNumber;
			exit(1);
		}
	}
}


// *ADDED CODE FROM PROMPT
void If()
{
	if (printSwitch)
		oftrace << "\t<If> ::= if (<Condition>) <Statement> endif | if (<Condition>) <Statement> else <Statement> endif\n";
	if (currentToken.lexeme == "if")
	{
		tempInstructionNumber = currentInstructionNumber;
		lexAdv();
		if (currentToken.lexeme == "(")
		{
			lexAdv();
			Condition();
			if (currentToken.lexeme == ")")
			{
				lexAdv();
				Statement();
				backPatch(currentInstructionNumber);
				if (currentToken.lexeme == "endif")
				{
					lexAdv();
				}
				else if (currentToken.lexeme == "else")
				{
					lexAdv();
					Statement();
					if (currentToken.lexeme == "endif")
					{
						lexAdv();
					}
					else
					{
						oftrace << "\n<><><> Syntax Error, expecting 'endif' on line " << currentToken.lineNumber;
						exit(1);
					}
				}
				else
				{
					oftrace << "\n<><><> Syntax Error, expecting 'endif' or 'else' on line " << currentToken.lineNumber;
					exit(1);
				}
			}
			else
			{
				oftrace << "\n<><><> Syntax Error, expecting ) after <Condition> on line " << currentToken.lineNumber;
				exit(1);
			}
		}
		else
		{
			oftrace << "\n<><><> Syntax Error, expecting ( on line " << currentToken.lineNumber;
			exit(1);
		}
	}
}


void Return()
{
	if (printSwitch)
		oftrace << "\t<Return> ::= return; | return <Expression>;\n";

	lexAdv();
	if (currentToken.lexeme == ";")
	{
		lexAdv();
	}
	else
	{
		Expression();
		if (currentToken.lexeme == ";")
			lexAdv();
		else
		{
			oftrace << "\n<><><> Syntax Error, expecting ';' before '" << currentToken.lexeme << "' on line " << currentToken.lineNumber;
			exit(1);
		}
	}
}


void Write()
{
	if (printSwitch)
		oftrace << "\t<Write> ::= print (<Expressions>);\n";

	//generateInstruction("STDOUT", 0);
	lexAdv();
	if (currentToken.lexeme == "(")
	{
		lexAdv();
		Expression();
		if (currentToken.lexeme == ")")
		{
			lexAdv();
			if (currentToken.lexeme == ";")
			{
				generateInstruction("STDOUT", 0);
				lexAdv();
			}
			else
			{
				oftrace << "\n<><><> Syntax Error, expecting ';' before '" << currentToken.lexeme << "' on line " << currentToken.lineNumber;
				exit(1);
			}
		}
		else
		{
			oftrace << "\n<><><> Syntax Error, expecting ')' before '" << currentToken.lexeme << "' on line " << currentToken.lineNumber;
			exit(1);
		}
	}
	else
	{
		oftrace << "\n<><><> Syntax Error, expecting '(' before '" << currentToken.lexeme << "' on line " << currentToken.lineNumber;
		exit(1);
	}
}


void Read()
{
	if (printSwitch)
		oftrace << "\t<Read> ::= read (<IDs>);\n";

	generateInstruction("STDIN", 0);
	lexAdv();
	if (currentToken.lexeme == "(")
	{
		lexAdv();
		IDs();
		if (currentToken.lexeme == ")")
		{
			lexAdv();
			if (currentToken.lexeme == ";")
				lexAdv();
			else
			{
				oftrace << "\n<><><> Syntax Error. Expecting ';' before '" << currentToken.lexeme << "' on line " << currentToken.lineNumber;
				exit(1);
			}
		}
		else
		{
			oftrace << "\n<><><> Syntax Error, expecting ')' before '" << currentToken.lexeme << "' on line " << currentToken.lineNumber;
			exit(1);
		}
	}
	else
	{
		oftrace << "\n<><><> Syntax Error, expecting '(' before '" << currentToken.lexeme << "' on line " << currentToken.lineNumber;
		exit(1);
	}
}


// *ADDED CODE FROM PROMPT
void While()
{
	if (printSwitch)
		oftrace << "\t<While> ::= while (<Condition>) <Statement>\n";

	int tempLocalInstructAddress = currentInstructionNumber;               //*
	generateInstruction("LABEL", NULL);                             //*
	lexAdv();
	if (currentToken.lexeme == "(")
	{
		lexAdv();
		Condition();
		if (currentToken.lexeme == ")")
		{
			lexAdv();                                                  //*
			Statement();
			generateInstruction("JUMP", tempLocalInstructAddress);     //*
			backPatch(currentInstructionNumber);                       //*
		}
	}
}


// *ADDED CODE FROM PROMPT
void Condition()
{
	if (printSwitch)
		oftrace << "\t<Condition> ::= <Expression> <Relop> <Expression>\n";

	Expression();
	Relop();
	Expression();

	if (tempSaveToken == "=")
	{
		generateInstruction("EQU", NULL);
		jumpStack.push(currentInstructionNumber);
		generateInstruction("JUMPZ", NULL);
	}
	else if (tempSaveToken == "/=" || tempSaveToken == "!=")
	{
		generateInstruction("EQU", 0);
		generateInstruction("PUSHI", 0);
		generateInstruction("EQU", 0);
		jumpStack.push(currentInstructionNumber);
		generateInstruction("JUMPZ", 0);
	}
	else if (tempSaveToken == ">")
	{
		generateInstruction("GTR", NULL);
		jumpStack.push(currentInstructionNumber);
		generateInstruction("JUMPZ", NULL);
	}
	else if (tempSaveToken == "<")
	{
		generateInstruction("LES", NULL);
		jumpStack.push(currentInstructionNumber);
		generateInstruction("JUMPZ", NULL);
	}
	else if (tempSaveToken == "=>")
	{
		generateInstruction("POPM", currentInstructionNumber);
		generateInstruction("POPM", currentInstructionNumber + 1);
		generateInstruction("PUSHM", currentInstructionNumber);
		generateInstruction("PUSHM", currentInstructionNumber + 1);
		generateInstruction("GRT", 0);
		generateInstruction("JUMPZ", currentInstructionNumber + 2);
		generateInstruction("JUMP", currentInstructionNumber + 5);
		generateInstruction("PUSHM", currentInstructionNumber);
		generateInstruction("PUSHM", currentInstructionNumber + 1);
		generateInstruction("EQU", 0);
		jumpStack.push(currentInstructionNumber);
		generateInstruction("JUMPZ", 0);
	}
	else if (tempSaveToken == "<=")
	{
		generateInstruction("POPM", currentInstructionNumber);
		generateInstruction("POPM", currentInstructionNumber + 1);
		generateInstruction("PUSHM", currentInstructionNumber);
		generateInstruction("PUSHM", currentInstructionNumber + 1);
		generateInstruction("LES", 0);
		generateInstruction("JUMPZ", currentInstructionNumber + 2);
		generateInstruction("JUMP", currentInstructionNumber + 5);
		generateInstruction("PUSHM", currentInstructionNumber);
		generateInstruction("PUSHM", currentInstructionNumber + 1);
		generateInstruction("EQU", 0);
		jumpStack.push(currentInstructionNumber);
		generateInstruction("JUMPZ", 0);
	}
}


// *ADDED CODE FROM PROMPT
void Relop()
{
	if (printSwitch)
		oftrace << "\t<Relop> ::= = | /= | > | < | => | <=\n";

	if (currentToken.lexeme == "=" || currentToken.lexeme == "/=" || currentToken.lexeme == ">"
		|| currentToken.lexeme == "<" || currentToken.lexeme == "=>" || currentToken.lexeme == "<=")
	{
		tempSaveToken = currentToken.lexeme;
		lexAdv();
	}
	else
	{
		oftrace << "\n<><><> Syntax error, expecting valid comparison operator before " << currentToken.lexeme << " on line " << currentToken.lineNumber;
		exit(1);
	}
}


void Expression()
{
	if (printSwitch)
		oftrace << "\t<Expression> ::= <Term> <Expression Prime>\n";

	Term();
	ExpressionPrime();
}


// *ADDED CODE FROM PROMPT
void ExpressionPrime()
{
	if (printSwitch)
		oftrace << "\t<Expression Prime> ::= + <Term> <Expression Prime> | - <Term> <Expression Prime> | <Empty>\n";

	if (currentToken.lexeme == "+") //|| currentToken.lexeme == "-")
	{
		lexAdv();
		Term();
		generateInstruction("ADD", NULL);                           //*
		ExpressionPrime();
	}
	else if (currentToken.lexeme == "-")
	{
		lexAdv();
		Term();
		generateInstruction("SUB", NULL);                           //*
		ExpressionPrime();
	}
	else if (currentToken.token == "UNKNOWN")
	{
		oftrace << "\n<><><> Syntax error, expecting '+', '-', or nothing before '" << currentToken.lexeme << "' on line " << currentToken.lineNumber;
		exit(1);
	}
	else
	{
		Empty();
	}
}


void Term()
{
	if (printSwitch)
		oftrace << "\t<Term> ::= <Factor> <Term Prime>\n";
	
	Factor();
	TermPrime();
}


// *ADDED CODE FROM PROMPT
void TermPrime()
{
	if (printSwitch)
		oftrace <<  "\t<Term Prime> ::= * <Factor> <Term Prime> | / <Factor> <Term Prime> | <Empty>\n";

	if (currentToken.lexeme == "*") //|| currentToken.lexeme == "/")
	{
		lexAdv();
		Factor();
		generateInstruction("MUL", NULL);                           //*
		TermPrime();
	}
	else if (currentToken.lexeme == "/")
	{
		lexAdv();
		Factor();
		generateInstruction("DIV", NULL);                           //*
		TermPrime();
	}else if (currentToken.token == "UNKNOWN")
	{
		oftrace << "\n<><><> Syntax Error, expecting '*', '/', or 'Empty' before '" << currentToken.lexeme << "' on line " << currentToken.lineNumber;
		exit(1);
	}
	else
	{
		Empty();
	}
}


void Factor()
{
	if (printSwitch)
		oftrace << "\t<Factor> ::= - <Primary> | <Primary>\n";

	if (currentToken.lexeme == "-")
	{
		lexAdv();
		Primary();
	}
	
	else if (currentToken.token != "UNKNOWN")
	{
		Primary();
	}
	
	else
	{
		oftrace << "\n<><><> Syntax Error, expecting something different before '" << currentToken.lexeme << "' on line " << currentToken.lineNumber;
		exit(1);
	}
}


// *ADDED CODE FROM PROMPT
void Primary()
{
	if (printSwitch)
		oftrace << "\t<Primary> ::= <Identifier> | <Integer> | <Identifier> [<IDs>] | (<Expression>) | <Real> | true | false\n";

	if (currentToken.token == "IDENTIFIER")
	{
		tempInstructionNumber = getAddress(currentToken.lexeme);          //*
		generateInstruction("PUSHM", tempInstructionNumber);        //*
		lexAdv();
		if (currentToken.lexeme == "[")
		{
			lexAdv();
			IDs();
			if (currentToken.lexeme == "]")
			{
				lexAdv();
			}
			else
			{
				oftrace << "\n<><><> Syntax Error, expecting ']' before '" << currentToken.lexeme << "' on line " << currentToken.lineNumber;
				exit(1);
			}
		}
		else 
		{
			//If current token is not '['.  Do nothing here.
			//It means this function is running <Primary> ::= <Identifier> instead of <Primary> ::= <Identifier> [<IDs>]
		}

	}
	
	else if (currentToken.token == "INTEGER" || currentToken.token == "REAL")
	{
		generateInstruction("PUSHI", stoi(currentToken.lexeme));
		lexAdv();
	}
	
	else if (currentToken.lexeme == "(")
	{
		lexAdv();
		Expression();
		if (currentToken.lexeme == ")")
		{
			lexAdv();
		}
		else
		{
			oftrace << "\n<><><> Syntax Error, expecting ')' before '" << currentToken.lexeme << "' on line " << currentToken.lineNumber;
			exit(1);
		}
	}

	else if (currentToken.lexeme == "true" || currentToken.lexeme == "false")
	{
		lexAdv();
	}
	else
	{
		oftrace << "\n<><><> Syntax Error, expecting '<Identifer>', '<Qualifier>' or '<Expression>' before '" << currentToken.lexeme << "' on line " << currentToken.lineNumber;
		exit(1);
	}
}


void Empty()
{
	if (printSwitch)
		oftrace << "\t<Empty> ::= epsilon\n";

}

void generateInstruction(string opCode, int memoryLocation)
{
	instructionData tempInstructionData;
	tempInstructionData.instructionNumber = currentInstructionNumber;
	tempInstructionData.opCode = opCode;
	tempInstructionData.memoryLocation = memoryLocation;
	instructionTable.push_back(tempInstructionData);
	currentInstructionNumber++;
}


void backPatch(int jumpAddress)
{
	tempAddress = jumpStack.top();
	jumpStack.pop();
	instructionTable[tempAddress - 1].memoryLocation = jumpAddress; //not sure if -1 is needed
}


int getAddress(string token)
{
	for (int i = 0; i < symbolTable.size(); i++)
	{
		if (token == symbolTable.at(i).identifier)
			return symbolTable.at(i).memoryLocation;
	}
	oftrace << "\n<><><> Error, undeclared variable used.";
	exit(-1);
}


bool alreadyInSymbolTable(string symbolIdentifier)
{
	for (int i = 0; i < symbolTable.size(); i++)
	{
		if (symbolTable[i].identifier.compare(symbolIdentifier) == 0)
			return true;
	}
	return false;
}


void addDataTypeToSymbolTable(string dataType)
{
	symbolData tempSymbolData;
	tempSymbolData.dataType = dataType;
	symbolTable.push_back(tempSymbolData);
}


void printSymbolTable(vector<symbolData> &table, string filePath)
{
	ofstream    fout;
	fout.open(filePath);

	fout << "Identifiers in Symbol Table:\n\n";

	for (int i = 0; i < table.size(); i++)
	{
		fout << left << setw(4) << table[i].identifier <<
			left << setw(4) << table[i].memoryLocation << endl;
	}

	fout.close();
}


void printAssemblyCode(vector<instructionData> &instructions, string filePath)
{
	ofstream    fout;
	fout.open(filePath, ios_base::app);

	fout << "\n\n\nAssembly Code:\n\n";

	for (int i = 0; i < instructions.size(); i++)
	{
		fout << left << setw(6) << instructions[i].instructionNumber << left << setw(15) << 
			instructions[i].opCode << left << setw(7) << instructions[i].memoryLocation << endl;
	}

	fout.close();
}


