/*
Andrew Nguyen
Joshua Womack 
CPSC323
Assignment 2 - Syntax Analyzer

*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <iomanip>
using namespace std;


//These will be placed inside the vector
struct tokenData
{
	string  token;  //generic
	string  lexeme; //instance
	int     lineNumber;
};


//Transition table for the ID FSM (Zero-Based)
//Acceptance states: 1, 2, 4
const int FSM_ID_Table[6][3] = {
	1, 5, 5,
	2, 4, 3,
	2, 4, 3,
	2, 4, 3,
	2, 4, 3,
	5, 5, 5
};


//Transition table for the Digit FSM (Zero-Based)
//Acceptance states: 1, 3
const int FSM_Digit_Table[5][2] = {
	1, 4,
	1, 2,
	3, 4,
	3, 4,
	4, 4
};


//Reserved keyword list
string keywords[13] = { "function", "integer", "boolean", "real", "if", "endif",
"else", "return", "print", "read", "while", "true", "false" };

//Reserved operator list
string operators[11] = { ":=", "=", "/=", ">", "<", "=>", "<=", "+", "-", "*", "/" };

//Reserved separator list
string separators[11] = { "$$", ",", ":", ";", "|", "(", ")", "[", "]", "{", "}" };


class LA
{
public:
	LA();

	//Function prototypes
	bool				checkIfKeyword(string currentToken);
	bool				checkIfReal(string currentToken);
	bool				doesTokenStartWithAlpha(string inputString);
	int					get_ID_column(char input);
	int					get_intReal_column(char input);
	void				printHeader(string outfilepath);
	void				printTokens(vector<tokenData> & tokens, string outfilepath);
	vector<tokenData>	lexer(string inputString, int currentLineNumber);

	//Enumeration for ID_FSM
	enum id_columns
	{
		LETTER, ID_DIGIT, UNDERSCORE
	};

	//Enumeration for intReal_FSM
	enum intReal_columns
	{
		INT_REAL_DIGIT, PERIOD
	};
};



//Lexer function to break apart source code into different tokens
vector<tokenData> LA::lexer(string inputString, int currentLineNumber)
{
	bool				found = false;
	char				currentChar;
	int					currentState = 0;		//Initial state in all FSMs is A.
	int					column = 0;
	string				currentToken = "";
	tokenData			temp;
	vector<tokenData>	tokens;

    //inputString currently has a entire line of the file
	for (int i = 0; i < inputString.length(); i++)
	{
		currentChar = inputString[i];
		found = false;

		//If character is letter or underscore, use ID FSM
		if ((isalpha(inputString[i])) 														//if currentChar is alpha
			|| (inputString[i] == '_') 														//if currentChar is '_'
			|| (isdigit(inputString[i]) && doesTokenStartWithAlpha(currentToken)))			//if currentChar is digit AND currentToken starts with an alpha
		{
			currentToken += currentChar;
			column = get_ID_column(currentChar);
			currentState = FSM_ID_Table[currentState][column];

			//If end of string is reached, or if a space is reached, or if a non-letter/non-digit/non-underscore is reached. End of token.
			if (i == inputString.length() - 1
				|| isspace(inputString[i + 1])
				|| (!isalpha(inputString[i + 1]) && !isdigit(inputString[i + 1]) && inputString[i + 1] != '_'))
			{
				if (currentState == 1 || currentState == 2 || currentState == 4)
				{
					if (checkIfKeyword(currentToken))
						temp.token = "KEYWORD";
					else
						temp.token = "IDENTIFIER";
				}
				else
					temp.token = "UNKNOWN";

				temp.lexeme = currentToken;
				temp.lineNumber = currentLineNumber;
				tokens.insert(tokens.end(), temp);
				currentToken.clear();
				column = 0;
				currentState = 0;
			}
		}

		//If first character in token is digit, use Digit FSM
		else if (((currentToken.empty() && isdigit(inputString[i]))
			|| (!currentToken.empty() && isdigit(currentToken[0]) && isdigit(inputString[i]))
			|| (!currentToken.empty() && isdigit(currentToken[0]) && inputString[i] == '.')))
		{
			currentToken += currentChar;
			column = get_intReal_column(currentChar);
			currentState = FSM_Digit_Table[currentState][column];

			//If end of string is reached, or if a space is reached, or if a non-digit/non-period is reached: then end of token
			if (i == inputString.length() - 1 										//end of string is reached
				|| isspace(inputString[i + 1]) 										//next char is a space
				|| (!isdigit(inputString[i + 1]) && inputString[i + 1] != '.'))		//next char is not a digit or a period
			{
				//Check if digit token terminated due to non-digit character.
				//If so, then continue reading in the token until end of string or space is reached.
				if ((i != inputString.length() - 1) && !isspace(inputString[i + 1]) && isalpha(inputString[i+1]))
				{
					while ((inputString[i] != '\0' && !isspace(inputString[i]) && (isdigit(inputString[i+1]) || isalpha(inputString[i+1]) || inputString[i+1] == '_')))
					{
						i++;
						currentChar = inputString[i];
						currentToken += currentChar;
					}
					currentState = 2;
				}

				if (currentState == 1 || currentState == 3)
				{
					if (checkIfReal(currentToken))
						temp.token = "REAL";
					else
						temp.token = "INTEGER";
				}
				else
				{
					temp.token = "UNKNOWN";
				}

				temp.lexeme = currentToken;
				temp.lineNumber = currentLineNumber;
				tokens.insert(tokens.end(), temp);
				currentToken.clear();
				column = 0;
				currentState = 0;
			}
		}

		else if (isspace(inputString[i]) || inputString[i] == '\t' || inputString[i] == '\n')
		{
			//Do nothing
		}

		//Token must be special character if it is not letter, digit, space, or underscore.
		//Check to see if it is a separator or operator.
		else
		{
			currentToken += currentChar;

			//Special character token termination checker
			if (i == inputString.length() - 1 || (i < inputString.length() - 1 && (!isspace(inputString[i + 1]) || !isalnum(inputString[i + 1]))))
			{
				//For finding operator
				for (int j = 0; j < 11; j++)
				{
					if (currentToken == operators[j])
					{
						//This for loop checks if operator is actually a 2-char operator.
						//Does not catch the := operator. A special case if statement at line 243 does.
						for (int k = 0; k < 11; k++)
						{
							if ((currentToken + inputString[i + 1]) == operators[k])
							{
								currentToken += inputString[i + 1];
								i++;
								break;
							}
						}
						temp.token = "OPERATOR";
						temp.lexeme = currentToken;
						temp.lineNumber = currentLineNumber;
						tokens.insert(tokens.end(), temp);
						currentToken.clear();
						found = true;
						break;
					}
				}

				//Special case conditional for checking for the := operator
				if (!found && i < inputString.length())
				{
					if (currentToken + inputString[i + 1] == ":=")
					{
						temp.token = "OPERATOR";
						temp.lexeme = currentToken + inputString[i + 1];
						temp.lineNumber = currentLineNumber;
						tokens.insert(tokens.end(), temp);
						currentToken.clear();
						i++;
						found = true;
					}
				}

				//For finding separator
				for (int j = 0; j < 11; j++)
				{
					if (currentToken == separators[j])
					{
						temp.token = "SEPARATOR";
						temp.lexeme = currentToken;
						temp.lineNumber = currentLineNumber;
						tokens.insert(tokens.end(), temp);
						currentToken.clear();
						found = true;
						break;
					}
				}

				//Special case conditional for checking for the $$ operator
				if (!found && i < inputString.length())
				{
					if (currentToken + inputString[i + 1] == "$$")
					{
						temp.token = "SEPARATOR";
						temp.lexeme = currentToken + inputString[i + 1];
						temp.lineNumber = currentLineNumber;
						tokens.insert(tokens.end(), temp);
						currentToken.clear();
						i++;
						found = true;
					}
				}

				if (!found)
				{
					temp.token = "UNKNOWN";
					temp.lexeme = currentToken;
					temp.lineNumber = currentLineNumber;
					tokens.insert(tokens.end(), temp);
					currentToken.clear();
				}
			}
		}
	}
	return tokens;
}


LA::LA()                //What is this here for?
{

};


//Checks if first character of string is a letter
bool LA::doesTokenStartWithAlpha(string currentToken)
{
    if (isalpha(currentToken[0]))
        return true;
    else
        return false;
}


//Sets up the column for ID_FSM
int LA::get_ID_column(char input)
{
	int column = -1;

	if (isalpha(input))
		column = LETTER;
	else if (isdigit(input))
		column = ID_DIGIT;
	else if (input == '_')
		column = UNDERSCORE;

	return column;
}


//Sets up the column for intReal_FSM
int LA::get_intReal_column(char input)
{
	int column = -1;

	if (isdigit(input))
		column = INT_REAL_DIGIT;
	else if (input == '.')
		column = PERIOD;

	return column;;
}


//Linear search through the keywords array to check for value
bool LA::checkIfKeyword(string currentToken)
{
	for (int i = 0; i < 13; i++)
	{
		if (currentToken == keywords[i])
			return true;
	}
	return false;
}


//Checks whether integer or real value
bool LA::checkIfReal(string currentToken)
{
	for (int i = 0; i < currentToken.length(); i++)
	{
		if (currentToken[i] == '.')
			return true;									//returning true means that currentToken is a REAL
	}
	return false;											//returning false means that currentToken is a INTEGER
}


//Prints tokens and lexemes to screen and file
void LA::printTokens(vector<tokenData> & tokens, string outfilepath)
{
	ofstream ofput;
	ofput.open(outfilepath, ios_base::app);

	for (int i = 0; i < tokens.size(); i++)
	{
		cout << left << setw(15) << tokens[i].token << left << setw(7) << tokens[i].lexeme << endl;
		ofput << left << setw(15) << tokens[i].token << left << setw(7) << tokens[i].lexeme << endl;
	}

	ofput.close();
}


//Prints the header to output file
void LA::printHeader(string outfilepath)
{
	ofstream ofput;
	ofput.open(outfilepath);

	cout << left << setw(15) << "token" << left << setw(5) << "lexeme" << endl << endl;
	ofput << left << setw(15) << "token" << left << setw(5) << "lexeme" << endl << endl;

	ofput.close();
}
