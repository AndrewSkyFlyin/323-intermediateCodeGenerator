/*
Andrew Nguyen
Joshua Womack 
CPSC323
Assignment 2 - Syntax Analyzer

*/

#include "LA.h"
using namespace std;


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


int                     lineNumber = 0;
int                     tokenIndex = 0;             //Index used to step through token vector
bool                    printSwitch = true;
vector<tokenData>       tokens;                     //vector to hold tokens as they are being inputted
vector<tokenData>       tokenList;                  //vector that holds all tokens once they have been read in initially
tokenData               currentToken;
ifstream			    ifget;
ofstream				oftrace;


int main() 
{

	LA                      lex;
	string				    current = "";
	string				    infilepath = "";
	string				    outfilepath = "";
	
	
	//Input file to read from
	cout << "Before you begin, make sure the input test file is in\nthe same folder as the .exe of this program.\n";
	cout << "----------------------------------------------------------------------\n";
	cout << "Please enter the file name and extension of the input file (input.txt).\n";
	cout << "Input: ";
	getline(cin, infilepath);
	cout << "You entered: " << infilepath << endl << endl;

	//Output file to write to
	cout << "Please enter the file and extension of the output file (output.txt).\n";
	cout << "Input: ";
	getline(cin, outfilepath);
	cout << "You entered: " << outfilepath << endl << endl;
	


	ifget.open(infilepath);
	oftrace.open(outfilepath);

	//infilepath = "/home/joshua/Git/323-syntaxAnalyzer/input.txt";
	//outfilepath = "/home/joshua/Git/323-syntaxAnalyzer/output.txt";
	//ifget.open(infilepath);


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
			lexAdv();
			OptDecList();
			StatementList();
			if (currentToken.lexeme == "$$")
				oftrace << "The End.\n";
			else
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
	else if (currentToken.lexeme == "integer" || currentToken.lexeme == "boolean" || currentToken.lexeme == "real")
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
			if (currentToken.lexeme == "integer" || currentToken.lexeme == "boolean" || currentToken.lexeme == "real")
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
		|| currentToken.lexeme == "false" || currentToken.lexeme == "real" || currentToken.lexeme == "boolean")
		lexAdv();
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


void Assign()
{
	if (printSwitch)
		oftrace << "\t<Assign> ::= <Identifier> := <Expression>;\n";

	if (currentToken.token == "IDENTIFIER")
	{
		lexAdv();
		if (currentToken.lexeme == ":=")
		{
			lexAdv();
			Expression();
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


void If()
{
	if (printSwitch)
		oftrace << "\t<If> ::= if (<Condition>) <Statement> endif | if (<Condition>) <Statement> else <Statement> endif\n";
	if (currentToken.lexeme == "if")
	{
		lexAdv();
		if (currentToken.lexeme == "(")
		{
			lexAdv();
			Condition();
			if (currentToken.lexeme == ")")
			{
				lexAdv();
				Statement();
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

	lexAdv();
	if (currentToken.lexeme == "(")
	{
		lexAdv();
		Expression();
		if (currentToken.lexeme == ")")
		{
			lexAdv();
			if (currentToken.lexeme == ";")
				lexAdv();
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


void While()
{
	if (printSwitch)
		oftrace << "\t<While> ::= while (<Condition>) <Statement>\n";

	lexAdv();
	if (currentToken.lexeme == "(")
	{
		lexAdv();
		Condition();
		if (currentToken.lexeme == ")")
		{
			lexAdv();
			Statement();
		}
	}
}


void Condition()
{
	if (printSwitch)
		oftrace << "\t<Condition> ::= <Expression> <Relop> <Expression>\n";

	Expression();
	Relop();
	Expression();
}


void Relop()
{
	if (printSwitch)
		oftrace << "\t<Relop> ::= = | /= | > | < | => | <=\n";

	if (currentToken.lexeme == "=" || currentToken.lexeme == "/=" || currentToken.lexeme == ">"
		|| currentToken.lexeme == "<" || currentToken.lexeme == "=>" || currentToken.lexeme == "<=")
	{
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


void ExpressionPrime()
{
	if (printSwitch)
		oftrace << "\t<Expression Prime> ::= + <Term> <Expression Prime> | - <Term> <Expression Prime> | <Empty>\n";

	if (currentToken.lexeme == "+" || currentToken.lexeme == "-")
	{
		lexAdv();
		Term();
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


void TermPrime()
{
	if (printSwitch)
		oftrace <<  "\t<Term Prime> ::= * <Factor> <Term Prime> | / <Factor> <Term Prime> | <Empty>\n";

	if (currentToken.lexeme == "*" || currentToken.lexeme == "/")
	{
		lexAdv();
		Factor();
		TermPrime();
	}
	else if (currentToken.token == "UNKNOWN")
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


void Primary()
{
	if (printSwitch)
		oftrace << "\t<Primary> ::= <Identifier> | <Integer> | <Identifier> [<IDs>] | (<Expression>) | <Real> | true | false\n";

	if (currentToken.token == "IDENTIFIER")
	{
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

