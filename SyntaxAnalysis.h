#pragma once

#include "LexicalAnalysis.h"
#include "Token.h"
#include "IR.h"

using namespace std;

class SyntaxAnalysis
{
public:
	Variables varijable;						//sve varijable na koje nailazim prilikom sintaksne analize
	Variables registarske_promenljive;			//samo registarske promenljive
	/**
	* Constructor
	*/
	SyntaxAnalysis(LexicalAnalysis& lex, Instructions& instructions);

	/**
	* Method which performs lexical analysis
	*/
	bool Do();
	void ubaciRegistarskuVarijablu(Token& token);
	void ubaciMemorijskuVarijablu(Token& token);
	void napraviInstrukciju(vector<Token>& destination, vector<Token>& source, InstructionType tip_instrukcije);
	Variable* vratiVarijablu(Token& token);
private:
	/**
	* Prints the error message, and token that caused the syntax error
	*/
	void printSyntaxError(Token token);

	/**
	* Prints the token info
	*/
	void printTokenInfo(Token token);

	/**
	* Eats the current token if its type is "t"
	* otherwise reports syntax error
	*
	* param[in] - t - the expected token type
	*/
	void eat(TokenType t);

	/**
	* Returns the next token from the token list
	*/
	Token getNextToken();

	/**
	 * Nonterminal Q
	 */
	void Q();

	/**
	 * Nonterminal S
	 */
	void S();

	/**
	 * Nonterminal L
	 */
	void L();

	/**
	 * Nonterminal E
	 */
	void E();

	/**
	* Reference to lexical analysis module
	*/
	LexicalAnalysis& lexicalAnalysis;

	/**
	* Syntax error indicator
	*/
	bool errorFound;

	/**
	* Iterator to the token list which represents the output of the lexical analysis
	*/
	TokenList::iterator tokenIterator;

	/**
	* Current token that is being analyzed
	*/
	Token currentToken;

	Instructions& instrukcije;					//instrukcije koje treba popuniti paralelno sa izvrsavanjem sintaksne analize
	int pozicija_varijabli;						//zbog konstruktora varijabli i instrukcija koristim ova dva polja
	int pozicija_instrukcija;			
};