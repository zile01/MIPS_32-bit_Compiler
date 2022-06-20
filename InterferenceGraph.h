#pragma once

#ifndef __INTERFERENCE_GRAPH__
#define __INTERFERENCE_GRAPH__

#include "IR.h"
#include "SyntaxAnalysis.h"
#include "Constants.h"

void upisiRegistarskeVarijable(Variables& variables, Variables& reg_variables)
{
	for (auto it = variables.begin(); it != variables.end(); ++it)
	{
		if ((*it)->getType() == 1)				//ako je REG_VAR
		{
			reg_variables.push_back((*it));
		}
	}

	int i = 0;
	for (auto it = reg_variables.begin(); it != reg_variables.end(); ++it, ++i)
	{
		(*it)->m_position = i;					
	}
	//zato sto bi se desilo recimo da pozicije u listi idu 0,1,4,5,9...zbog toga sto se elementi samo prebace iz liste variables, gde imam i memorijske prom
}

class InterferenceGraph
{
private:
	Instructions& instrukcije;
	int dimenzija;							//dimenzija kvadratne matrice smetnji
public:
	vector<vector<int>> matrica_smetnji;	//matrica smetnji
	Variables& varijable;					//lista promenljivih koje se koriste

	InterferenceGraph(Instructions& instructions, Variables& registarske_promenljive) : instrukcije(instructions), varijable(registarske_promenljive)
	{
		dimenzija = varijable.size();
	}

	void inicijalizujMatricuSmetnji()
	{
		matrica_smetnji.resize(dimenzija);
		for (int i = 0; i < dimenzija; ++i)
		{
			matrica_smetnji[i].resize(dimenzija);
		}
	}

	void popuniMatricuSmetnji()
	{
		int a;
		int b;

		for (auto it_i = instrukcije.begin(); it_i != instrukcije.end(); ++it_i)							//iteriram kroz instrukcije
		{
			for (auto it_def = (*it_i)->m_def.begin(); it_def != (*it_i)->m_def.end(); ++it_def)			//iteriram kroz m_def
			{
				for (auto it_out = (*it_i)->m_out.begin(); it_out != (*it_i)->m_out.end(); ++it_out)		//iteriram kroz m_out
				{
					//cilj je da za sve instrukcije koje nisu MOVE, a nijedna nije MOVE, dodam smetnju izmedju prom iz m_def i m_out
					//te promenjive se moraju razlikovati, sto proveravam preko pozicija
					//a = (*it_def)->m_position;			//vraca mi poziciju iz varijabli
					//b = (*it_out)->m_position;			//a meni treba pozicija iz reg varijabli
					Variable* x = (*it_def);
					Variable* y = (*it_out);
					a = vratiPozRegProm(x);					//treba da prodjem kroz varijable i da vratim poziciju na osnovu poklapanja imena
					b = vratiPozRegProm(y);

					if (a == b)
					{
						matrica_smetnji[a][b] = __EMPTY__;
						matrica_smetnji[b][a] = __EMPTY__;
					}
					else
					{
						matrica_smetnji[a][b] = __INTERFERENCE__;
						matrica_smetnji[b][a] = __INTERFERENCE__;
					}
				}
			}
		}
	}

	int vratiPozRegProm(Variable* prom)			
	{
		for (auto it = varijable.begin(); it != varijable.end(); ++it)
		{
			if ((*it)->getName() == prom->getName())
			{
				return (*it)->m_position;
			}
		}
	}
};

class Stack
{
public:
	stack<Variable*> stek;

	Stack(){}

	bool doSimplification(InterferenceGraph* IG, const int br_reg)
	{
		vector<vector<int>> matrix = IG->matrica_smetnji;
		Variables reg_variables = IG->varijable;
		int K = br_reg;
		int max_smetnji = 0;
		int broj_smetnji;
		int pozicija = -1;
		int max_pozicija = 0;
		bool flag = true;

		while (reg_variables.size() != 0) {													//radi nesto, sve dok ima cvorova u grafu

			//prolazim kroz matricu smetnji i trazim varijablu sa najvise smetnji, zatim je uklanjam i ubacujem na stek

			for (auto it_1 = matrix.begin(); it_1 != matrix.end(); ++it_1)					//prolazim kroz matricu smetnji
			{
				broj_smetnji = 0;															//broj smetnji se odnosi na pojedinacnu promenljivu

				for (auto it_2 = (*it_1).begin(); it_2 != (*it_1).end(); ++it_2)
				{
					if ((*it_2) == __INTERFERENCE__)
					{
						++broj_smetnji;
					}
				}
				++pozicija;																	//svaki put kada prodjem ovu for petlju, prosao sam jednu prom

				if (broj_smetnji >= K)														//prelivanje
				{
					return false;
				}
				else if (broj_smetnji > max_smetnji)		//ako prom iz koje sam izasao ima vise smetnji nego prethodna prom sa najvise smetnji
				{
					max_smetnji = broj_smetnji;				//menja se max smetnji
					max_pozicija = pozicija;				//oznacavam poziciju promenljive sa max smetnji
				}

				if (max_smetnji == 0 && broj_smetnji == 0)			//graf se ne moze vise uprostiti, sad svi idu na stek
				{
					max_pozicija = 0;								//moze se desiti da ostanu 2,3, vise cvorova sa 0 smetnji, svi treba da idu na stek po redu, stoga max_pozicija = 0
				}
			}

			//sada gledam da uklonim prom sa najvise smetnji
			//uklanjam je iz matrice smetnji i iz liste registarskih prom

			matrix.erase(matrix.begin() + max_pozicija);					//izbrisi prom iz kolone
			for (int i = 0; i < matrix.size(); ++i)						//izbrisi prom iz vrste
			{
				matrix[i].erase(matrix[i].begin() + max_pozicija);
			}

			max_smetnji = 0;

			for (auto it = reg_variables.begin(); it != reg_variables.end(); ++it)
			{
				if ((*it)->m_position == pozicija)
				{
					stek.push(*it);				//stavi na stek
					reg_variables.erase(it);	//brisi sa liste
					break;
				}
			}
			pozicija = -1;
		}

		return flag;
	}
};

InterferenceGraph* doInterferenceGraph(Instructions& instructions, Variables& registarske_promenljive)					//formiranje grafa smetnji
{
	InterferenceGraph* pom = new InterferenceGraph(instructions, registarske_promenljive);
	pom->inicijalizujMatricuSmetnji();
	pom->popuniMatricuSmetnji();

	return pom;
}

void freeInterferenceGraph(InterferenceGraph* ig)									//oslobadjanje grafa smetnji
{
	delete ig;
}

//void printInterferenceGraph(InterferenceGraph* ig);								//ispis grafa smetnji

bool doResourceAllocation(stack<Variable*>* simplificationStack, InterferenceGraph* ig)
{
	while (!simplificationStack->empty())							//prolazim kroz stek, dok on ne ostane prazan
	{							
		Variable* pom = simplificationStack->top();					//preuzeta je promenljiva sa vrha steka
		int index1 = pom->m_position;							//index promenljive u matrici smetnji
		int index2;

		Variables lista = ig->varijable;

		//sad trazim smetnje

		for (auto it = lista.begin(); it != lista.end(); it++)
		{		
			index2 = (*it)->m_position;										//index druge promenljive sa kojom poredim pocetnu
			if (ig->matrica_smetnji[index1][index2] == __INTERFERENCE__) {		//postoji smetnja izmedju prom sa steka i neke dr prom
				Regs boja = (*it)->m_assignment;								//boja druge prom
				if (boja == t3) {												//boje dodeljujem on najnizeg indexa ka visima, npr. prvo ide t0
					//dolazi do prelivanja
					return false;
				}
				else if (boja == t2) {
					pom->m_assignment = t3;
				}
				else if (boja == t1) {
					pom->m_assignment = t2;
				}
				else if (boja == t0) {
					pom->m_assignment = t1;
				}
				else pom->m_assignment = t0;									//nema boje druge pom
			}
		}

		simplificationStack->pop();
	}
	return true;
}

#endif