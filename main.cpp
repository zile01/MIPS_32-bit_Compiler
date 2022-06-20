#include <iostream>
#include <exception>

#include "LexicalAnalysis.h"
#include "SyntaxAnalysis.h"
#include "IR.h"
#include "InterferenceGraph.h"

using namespace std;

int main()
{
	try																			//kriticni deo koda
	{
		std::string fileName = ".\\..\\examples\\simple.mavn";
		//std::string fileName = ".\\..\\examples\\sintaksnagreska.mavn";
		//std::string fileName = ".\\..\\examples\\prelivanje.mavn";
		//std::string fileName = ".\\..\\examples\\primer.mavn";
		bool retValL = false;
		bool retValS = false;

		//LEKSICKA ANALIZA

		LexicalAnalysis lex;													

		if (!lex.readInputFile(fileName))										//ako fajl nije uspesno ucitan
			throw runtime_error("\nException! Failed to open input file!\n");

		lex.initialize();														//metoda za inicijalizaciju leksicke analize i FSM

		retValL = lex.Do();														//leksicka analiza

		if (retValL)																
		{
			cout << "Lexical analysis finished successfully!" << endl;
			lex.printTokens();													//ispis leksicke analize
		}
		else
		{
			lex.printLexError();												//ispis tokena kod kog je doslo do greske
			throw runtime_error("\nException! Lexical analysis failed!\n");
		}

		//SINTAKSNA ANALIZA

		Instructions instructions;												//lista instrukcija, koja se popunjava u sintaksnoj analizi
		SyntaxAnalysis syntax(lex, instructions);								

		retValS = syntax.Do();													//sintaksna analiza

		if (retValS)															
		{
			cout << "Syntax analysis finished successfully!" << endl << endl;
		}
		else
		{
			cout << "Syntax analysis has finished unsuccessfully!" << endl << endl;
		}

		cout << "ISPIS INSTRUKCIJA NAKON UCITAVANJA..." << endl;
		ispisiInstrukcije(instructions);

		//ANALIZA ZIVOTNOG VEKA

		prethodnici(instructions);												//podesavam pred
		sledbenici(instructions);												//podesavam succ
		ubaciVarijable(instructions);											//podesavam use i def

		livenessAnalysis(instructions);											//analiza zivotnog veka

		cout << endl << "ISPIS INSTRUKCIJA NAKON ANALIZE ZIVOTNOG VEKA..." << endl << endl;
		ispisiInstrukcije(instructions);

		//DODELA RESURSA

		upisiRegistarskeVarijable(syntax.varijable, syntax.registarske_promenljive);					//iz varijabli izdvajam registarske, jer se za njih primenjuje dodela resursa
		InterferenceGraph* IG = doInterferenceGraph(instructions, syntax.registarske_promenljive);		//graf smetnji

		Stack stack;

		bool pomocni_flag;
		pomocni_flag = stack.doSimplification(IG, __REG_NUMBER__);										//faza uproscavanja

		if (pomocni_flag == false)																		//prelivanje, samo ispis
		{
			cout << "Doslo je do prelivanja!!!" << endl;
			return 0;
		}

		bool flag = doResourceAllocation(&(stack.stek), IG);											//dodela resursa
		if (flag == true) {
			cout << "Resource Allocation finished successfully!" << endl << endl;
		}
		else
		{
			cout << "Resource Allocation has finished unsuccessfully!" << endl << endl;
		}

		//
		cout << "SADA BI TREBALO DA SE ISPISE PREVEDENI ASEMBLERSKI KOD, ALI TO NISAM STIGAO URADITI :( ..." << endl;


		freeInterferenceGraph(IG);																		//brisanje grafa smetnji
	}
	catch (runtime_error e)																				//uhvati exception i ispisi detalje istog
	{
		cout << e.what() << endl;
		return 1;
	}

	return 0;
}		