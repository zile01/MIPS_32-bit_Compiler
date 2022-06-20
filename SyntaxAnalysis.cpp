#include <iostream>
#include <iomanip>

#include "SyntaxAnalysis.h"

using namespace std;


SyntaxAnalysis::SyntaxAnalysis(LexicalAnalysis& lex, Instructions& instructions)
	:lexicalAnalysis(lex), errorFound(false), tokenIterator(lexicalAnalysis.getTokenList().begin()), instrukcije(instructions)
{
	pozicija_instrukcija = 0;
	pozicija_varijabli = 0;
}

bool SyntaxAnalysis::Do()
{
	currentToken = getNextToken();

	//Call function for the starting non-terminal symbol
	Q();

	return !errorFound;
}

void SyntaxAnalysis::printSyntaxError(Token token)
{
	cout << "Syntax error! Token: " << token.getValue() << " unexpected" << endl;
}

void SyntaxAnalysis::printTokenInfo(Token token)
{
	token.printTokenInfo();
}

void SyntaxAnalysis::eat(TokenType t)
{
	if (errorFound == false)
	{
		if (currentToken.getType() == t)
		{

			if (currentToken.getType() == T_END_OF_FILE)							//da ne bi doslo do exceptiona, eof se posebno proverava
			{
				cout << currentToken.getValue() << endl;
				return;
			}

			cout << currentToken.getValue() << endl;
			currentToken = getNextToken();
		}
		else
		{
			printSyntaxError(currentToken);
			errorFound = true;
		}
	}
}

Token SyntaxAnalysis::getNextToken()
{
	if (tokenIterator == lexicalAnalysis.getTokenList().end())
		throw runtime_error("End of input file reached");
	
	return *tokenIterator++;
}

void SyntaxAnalysis::ubaciRegistarskuVarijablu(Token& token)
{
	string ime = token.getValue();
	Variable* varijabla = new Variable(ime, pozicija_varijabli, Variable::REG_VAR);
	++pozicija_varijabli;

	varijable.push_back(varijabla);
}

void SyntaxAnalysis::ubaciMemorijskuVarijablu(Token& token)
{
	string ime = token.getValue();
	Variable* varijabla = new Variable(ime, pozicija_varijabli, Variable::MEM_VAR);
	++pozicija_varijabli;

	varijable.push_back(varijabla);
}

Variable* SyntaxAnalysis::vratiVarijablu(Token& token) {											//sluzi da kada pravim instrukciju, vratim varijablu na osnovu njegov naziva
	for (auto it = varijable.begin(); it != varijable.end(); ++it)
	{
		string ime = token.getValue();
		if ((*it)->getName() == ime)
		{
			return (*it);
		}
	}

	return NULL;																					//nisam znao sta drugo da vratim, pa NULL
}

void SyntaxAnalysis::napraviInstrukciju(vector<Token>& destination, vector<Token>& source, InstructionType tip_instrukcije)
{
	Variables dst;
	Variables src;
	Variable* pom;
	Instruction* i;
	string id;
	int num;

	dst.clear();
	src.clear();
	//ove dve linije sluze da, svaki put kada pozovem napraviInstrukciju, obrisem sadrzaj listi dst i src, tako da se novi elementi ne nadovezuju na prethodne

	switch (tip_instrukcije)
	{
		case I_AND:
			pom = vratiVarijablu(destination[0]);
			dst.push_back(pom);														

			pom = vratiVarijablu(source[0]);
			src.push_back(pom);
			pom = vratiVarijablu(source[1]);
			src.push_back(pom);

			i = new Instruction(pozicija_instrukcija, tip_instrukcije, dst, src,"");
			++pozicija_instrukcija;
			break;
		case I_B:
			id = destination[0].getValue();

			i = new Instruction(pozicija_instrukcija, tip_instrukcije, dst, src, id);
			++pozicija_instrukcija;
			break;
		case I_ADD:
			pom = vratiVarijablu(destination[0]);
			dst.push_back(pom);

			pom = vratiVarijablu(source[0]);
			src.push_back(pom);
			pom = vratiVarijablu(source[1]);
			src.push_back(pom);

			i = new Instruction(pozicija_instrukcija, tip_instrukcije, dst, src, "");
			++pozicija_instrukcija;
			break;
		case I_LW:													//ovaj slucaj, kao i SW su drugaciji zbog koriscenja num u konstruktoru instrukcije
			pom = vratiVarijablu(destination[0]);
			dst.push_back(pom);

			num = stoi(source[0].getValue());						//parsiranje num iz string u int, jer je to ustv. broj koji prosledjujem konstruktoru, ako ga ne prosledim njegova je default vrednost 0

			pom = vratiVarijablu(source[1]);
			src.push_back(pom);

			i = new Instruction(pozicija_instrukcija, tip_instrukcije, dst, src, "", num);
			++pozicija_instrukcija;
			break;
		case I_ABS:
			pom = vratiVarijablu(destination[0]);
			dst.push_back(pom);

			pom = vratiVarijablu(source[0]);
			src.push_back(pom);
		
			i = new Instruction(pozicija_instrukcija, tip_instrukcije, dst, src, "");
			++pozicija_instrukcija;
			break;
		case I_SW:
			pom = vratiVarijablu(destination[0]);
			dst.push_back(pom);

			num = stoi(source[0].getValue());			

			pom = vratiVarijablu(source[1]);
			src.push_back(pom);

			i = new Instruction(pozicija_instrukcija, tip_instrukcije, dst, src, "", num);
			++pozicija_instrukcija;
			break;
		case I_XOR:
			pom = vratiVarijablu(destination[0]);
			dst.push_back(pom);

			pom = vratiVarijablu(source[0]);
			src.push_back(pom);
			pom = vratiVarijablu(source[1]);
			src.push_back(pom);

			i = new Instruction(pozicija_instrukcija, tip_instrukcije, dst, src, "");
			++pozicija_instrukcija;
			break;
		case I_BLTZ:
			pom = vratiVarijablu(source[0]);
			src.push_back(pom);

			id = destination[0].getValue();

			i = new Instruction(pozicija_instrukcija, tip_instrukcije, dst, src, id);
			++pozicija_instrukcija;
			break;
		case I_OR:
			pom = vratiVarijablu(destination[0]);
			dst.push_back(pom);

			pom = vratiVarijablu(source[0]);
			src.push_back(pom);
			pom = vratiVarijablu(source[1]);
			src.push_back(pom);

			i = new Instruction(pozicija_instrukcija, tip_instrukcije, dst, src, "");
			++pozicija_instrukcija;
			break;
		case I_LA:
			pom = vratiVarijablu(destination[0]);
			dst.push_back(pom);

			pom = vratiVarijablu(source[0]);
			src.push_back(pom);

			i = new Instruction(pozicija_instrukcija, tip_instrukcije, dst, src, "");
			++pozicija_instrukcija;
			break;
	}

	instrukcije.push_back(i);					//ubacujem novonapravljenu instrukciju na listu svih instrukcija
}

//TO DO
void SyntaxAnalysis::Q()
{
	if (errorFound)
	{
		return;
	}

	S();
	if (currentToken.getType() == T_SEMI_COL)					//ako je trenutni token ';'
	{
		eat(T_SEMI_COL);
	}
	else 
	{
		printSyntaxError(currentToken);							//ako nije, sintaksna greska
		errorFound = true;
	}
	L();
}

//TO DO
void SyntaxAnalysis::S()
{
	if (errorFound)
	{
		return;
	}

	if (currentToken.getType() == T_MEM)
	{
		eat(T_MEM);
		if (currentToken.getType() == T_M_ID)
		{
			ubaciMemorijskuVarijablu(currentToken);					//ubaci memorijsku varijablu na listu memorijskih varijabli
			eat(T_M_ID);
			if (currentToken.getType() == T_NUM)
			{
				eat(T_NUM);
			}
			else
			{
				printSyntaxError(currentToken);
				errorFound = true;
			}
		}
		else
		{
			printSyntaxError(currentToken);
			errorFound = true;
		}
	}
	else if (currentToken.getType() == T_REG)
	{
		eat(T_REG);
		if (currentToken.getType() == T_R_ID)
		{
			ubaciRegistarskuVarijablu(currentToken);						//ubaci registarsku varijablu na listu registarskih varijabli
			eat(T_R_ID);
		}
		else
		{
			printSyntaxError(currentToken);
			errorFound = true;
		}
	}
	else if (currentToken.getType() == T_FUNC)
	{
		eat(T_FUNC);
		if (currentToken.getType() == T_ID)
		{
			eat(T_ID);
		}
		else
		{
			printSyntaxError(currentToken);
			errorFound = true;
		}
	}
	else if (currentToken.getType() == T_ID)
	{
		eat(T_ID);
		if (currentToken.getType() == T_COL)
		{
			eat(T_COL);
			E();
		}
		else
		{
			printSyntaxError(currentToken);
			errorFound = true;
		}
	}
	else
	E();
}

//TO DO
void SyntaxAnalysis::L()
{
	if (errorFound)
	{
		return;
	}

	if (currentToken.getType() == T_END_OF_FILE)
	{
		eat(T_END_OF_FILE);
	}
	else
	Q();
}

//TO DO
void SyntaxAnalysis::E()
{
	if (errorFound) 
	{
		return;
	}

	vector<Token> source;										//koristim ove dve lokalne liste kao pomoc pri kreiranju instrukcija
	vector<Token> destination;


	//---------------------------------------------------------------------------------------------------------
	if (currentToken.getType() == T_AND)
	{
		eat(T_AND);
		if (currentToken.getType() == T_R_ID)
		{
			destination.push_back(currentToken);
			eat(T_R_ID);
			if (currentToken.getType() == T_COMMA)
			{
				eat(T_COMMA);
				if (currentToken.getType() == T_R_ID)
				{
					source.push_back(currentToken);
					eat(T_R_ID);
					if (currentToken.getType() == T_COMMA)
					{
						eat(T_COMMA);
						if (currentToken.getType() == T_R_ID)
						{
							source.push_back(currentToken);
							eat(T_R_ID);
							napraviInstrukciju(destination, source, I_AND);
						}
						else
						{
							printSyntaxError(currentToken);
							errorFound = true;
						}
					}
					else
					{
						printSyntaxError(currentToken);
						errorFound = true;
					}
				}
				else
				{
					printSyntaxError(currentToken);
					errorFound = true;
				}
			}
			else
			{
				printSyntaxError(currentToken);
				errorFound = true;
			}
		}
		else
		{
			printSyntaxError(currentToken);
			errorFound = true;
		}
	}
	//---------------------------------------------------------------------------------------------------------
	else if (currentToken.getType() == T_B)
	{
		eat(T_B);
		if (currentToken.getType() == T_ID)
		{
			destination.push_back(currentToken);
			eat(T_ID);
			napraviInstrukciju(destination, source,I_B);
		}
		else
		{
			printSyntaxError(currentToken);
			errorFound = true;
		}
	}
	//---------------------------------------------------------------------------------------------------------
	else if (currentToken.getType() == T_ADD)
	{
		eat(T_ADD);
		if (currentToken.getType() == T_R_ID)
		{
			destination.push_back(currentToken);
			eat(T_R_ID);
			if (currentToken.getType() == T_COMMA)
			{
				eat(T_COMMA);
				if (currentToken.getType() == T_R_ID)
				{
					source.push_back(currentToken);
					eat(T_R_ID);
					if (currentToken.getType() == T_COMMA)
					{
						eat(T_COMMA);
						if (currentToken.getType() == T_R_ID)
						{
							source.push_back(currentToken);
							eat(T_R_ID);
							napraviInstrukciju(destination, source,I_ADD);
						}
						else
						{
							printSyntaxError(currentToken);
							errorFound = true;
						}
					}
					else
					{
						printSyntaxError(currentToken);
						errorFound = true;
					}
				}
				else
				{
					printSyntaxError(currentToken);
					errorFound = true;
				}
			}
			else
			{
				printSyntaxError(currentToken);
				errorFound = true;
			}
		}
		else
		{
			printSyntaxError(currentToken);
			errorFound = true;
		}
	}
	//---------------------------------------------------------------------------------------------------------
	else if (currentToken.getType() == T_LW)
	{
		eat(T_LW);
		if (currentToken.getType() == T_R_ID)
		{
			destination.push_back(currentToken);
			eat(T_R_ID);
			if (currentToken.getType() == T_COMMA)
			{
				eat(T_COMMA);
				if (currentToken.getType() == T_NUM)
				{
					source.push_back(currentToken);
					eat(T_NUM);
					if (currentToken.getType() == T_L_PARENT)
					{
						eat(T_L_PARENT);
						if (currentToken.getType() == T_R_ID)
						{
							source.push_back(currentToken);
							eat(T_R_ID);
							if (currentToken.getType() == T_R_PARENT)
							{
								eat(T_R_PARENT);
								napraviInstrukciju(destination, source,I_LW);
							}
							else
							{
								printSyntaxError(currentToken);
								errorFound = true;
							}
						}
						else
						{
							printSyntaxError(currentToken);
							errorFound = true;
						}
					}
					else 
					{
						printSyntaxError(currentToken);
						errorFound = true;
					}
				}
				else
				{
					printSyntaxError(currentToken);
					errorFound = true;
				}
			}
			else
			{
				printSyntaxError(currentToken);
				errorFound = true;
			}
		}
		else
		{
			printSyntaxError(currentToken);
			errorFound = true;
		}
	}
	//---------------------------------------------------------------------------------------------------------
	else if (currentToken.getType() == T_ABS)
	{
		eat(T_ABS);
		if(currentToken.getType() == T_R_ID)
		{
			destination.push_back(currentToken);
			eat(T_R_ID);
			if (currentToken.getType() == T_COMMA)
			{
				eat(T_COMMA);
				if (currentToken.getType() == T_R_ID)
				{
					source.push_back(currentToken);
					eat(T_R_ID);
					napraviInstrukciju(destination, source,I_ABS);
				}
				else
				{
					printSyntaxError(currentToken);
					errorFound = true;
				}
			}
			else
			{
				printSyntaxError(currentToken);
				errorFound = true;
			}
		}
		else
		{
			printSyntaxError(currentToken);
			errorFound = true;
		}
	}
	//---------------------------------------------------------------------------------------------------------
	else if (currentToken.getType() == T_SW)
	{
		eat(T_SW);
		if (currentToken.getType() == T_R_ID)
		{
			destination.push_back(currentToken);
			eat(T_R_ID);
			if (currentToken.getType() == T_COMMA)
			{
				eat(T_COMMA);
				if (currentToken.getType() == T_NUM)
				{
					source.push_back(currentToken);
					eat(T_NUM);
					if (currentToken.getType() == T_L_PARENT)
					{
						eat(T_L_PARENT);
						if (currentToken.getType() == T_R_ID)
						{
							source.push_back(currentToken);
							eat(T_R_ID);
							if (currentToken.getType() == T_R_PARENT)
							{
								eat(T_R_PARENT);
								napraviInstrukciju(destination, source,I_SW);
							}
							else
							{
								printSyntaxError(currentToken);
								errorFound = true;
							}
						}
						else
						{
							printSyntaxError(currentToken);
							errorFound = true;
						}
					}
					else
					{
						printSyntaxError(currentToken);
						errorFound = true;
					}
				}
				else
				{
					printSyntaxError(currentToken);
					errorFound = true;
				}
			}
			else
			{
				printSyntaxError(currentToken);
				errorFound = true;
			}
		}
		else
		{
			printSyntaxError(currentToken);
			errorFound = true;
		}
	}
	//---------------------------------------------------------------------------------------------------------
	else if (currentToken.getType() == T_XOR)
	{
		eat(T_XOR);
		if (currentToken.getType() == T_R_ID)
		{
			destination.push_back(currentToken);
			eat(T_R_ID);
			if (currentToken.getType() == T_COMMA)
			{
				eat(T_COMMA);
				if (currentToken.getType() == T_R_ID)
				{
					source.push_back(currentToken);
					eat(T_R_ID);
					if (currentToken.getType() == T_COMMA)
					{
						eat(T_COMMA);
						if (currentToken.getType() == T_R_ID)
						{
							source.push_back(currentToken);
							eat(T_R_ID);
							napraviInstrukciju(destination, source,I_XOR);
						}
						else
						{
							printSyntaxError(currentToken);
							errorFound = true;
						}
					}
					else
					{
						printSyntaxError(currentToken);
						errorFound = true;
					}
				}
				else
				{
					printSyntaxError(currentToken);
					errorFound = true;
				}
			}
			else
			{
				printSyntaxError(currentToken);
				errorFound = true;
			}
		}
		else
		{
			printSyntaxError(currentToken);
			errorFound = true;
		}
	}
	//---------------------------------------------------------------------------------------------------------
	else if (currentToken.getType() == T_BLTZ)
	{
		eat(T_BLTZ);
		if (currentToken.getType() == T_R_ID)
		{
			source.push_back(currentToken);
			eat(T_R_ID);
			if (currentToken.getType() == T_COMMA)
			{
				eat(T_COMMA);
				if (currentToken.getType() == T_ID)
				{
					destination.push_back(currentToken);
					eat(T_ID);
					napraviInstrukciju(destination, source,I_BLTZ);
				}
				else
				{
					printSyntaxError(currentToken);
					errorFound = true;
				}
			}
			else
			{
				printSyntaxError(currentToken);
				errorFound = true;
			}
		}
		else
		{
			printSyntaxError(currentToken);
			errorFound = true;
		}
	}
	//---------------------------------------------------------------------------------------------------------
	else if (currentToken.getType() == T_OR)
	{
		eat(T_OR);
		if (currentToken.getType() == T_R_ID)
		{
			destination.push_back(currentToken);
			eat(T_R_ID);
			if (currentToken.getType() == T_COMMA)
			{
				eat(T_COMMA);
				if (currentToken.getType() == T_R_ID)
				{
					source.push_back(currentToken);
					eat(T_R_ID);
					if (currentToken.getType() == T_COMMA)
					{
						eat(T_COMMA);
						if (currentToken.getType() == T_R_ID)
						{
							source.push_back(currentToken);
							eat(T_R_ID);
							napraviInstrukciju(destination, source,I_OR);
						}
						else
						{
							printSyntaxError(currentToken);
							errorFound = true;
						}
					}
					else
					{
						printSyntaxError(currentToken);
						errorFound = true;
					}
				}
				else
				{
					printSyntaxError(currentToken);
					errorFound = true;
				}
			}
			else
			{
				printSyntaxError(currentToken);
				errorFound = true;
			}
		}
		else
		{
			printSyntaxError(currentToken);
			errorFound = true;
		}
	}
	//---------------------------------------------------------------------------------------------------------
	else
	{
		if (currentToken.getType() == T_LA)
		{
			eat(T_LA);
			if (currentToken.getType() == T_R_ID)
			{
				destination.push_back(currentToken);
				eat(T_R_ID);
				if (currentToken.getType() == T_COMMA)
				{
					eat(T_COMMA);
					if (currentToken.getType() == T_M_ID)
					{
						source.push_back(currentToken);
						eat(T_M_ID);
						napraviInstrukciju(destination, source,I_LA);
					}
					else
					{
						printSyntaxError(currentToken);
						errorFound = true;
					}
				}
				else
				{
					printSyntaxError(currentToken);
					errorFound = true;
				}
			}
			else
			{
				printSyntaxError(currentToken);
				errorFound = true;
			}
		}
		else
		{
			printSyntaxError(currentToken);
			errorFound = true;
		}
	}
	//---------------------------------------------------------------------------------------------------------
}