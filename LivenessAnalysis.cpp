#include "IR.h"

Instruction* nadjiInstrukciju(std::string id, Instructions& instructions)			//iz liste instrukcija trazi onu koja ima adekvatan id
{
	for (auto it = instructions.begin(); it != instructions.end(); ++it)
	{
		if ((*it)->getId() == id)
		{
			return (*it);
		}
	}

	return NULL;
}

void prethodnici(Instructions& instructions)
{
	for (auto it = instructions.rbegin(); it != instructions.rend(); ++it)
	{
		auto it_pred = it;
		++it_pred;

		if (it_pred == instructions.rend())
		{
			return;
		}

		(*it)->m_pred.push_back(*(it_pred));
	}
}

void sledbenici(Instructions& instructions)							//ovde popunjavam m_succ listu
{

	for (auto it = instructions.begin(); it != instructions.end(); ++it)
	{
		auto it_succ = it;
		++it_succ;

		//koristim komandu push_back((*it_succ)), sto ima smisla uvek, kada it_succ ne pokazuje na end(), sto je slucaj kod poslednje iteracije, stoga ovo
		//ovo se moglo implementirati i sa it != instructions.end() - 1, ali nisam suguran sta treba includovati da bi to proslo kompajler
		if (it_succ == instructions.end())
		{
			return;
		}

		//B ima jednog naslednika nepoznatog
		//BLTZ dva, jednog trivijalnog i jednog nepoznatog
		//svi ostali jednog trivijalnog

		if ((*it)->getType() == I_B)															//ako je instrukcija B
		{
			std::string id = (*it)->getId();													//id je ime labele gde treba da se skoci
			Instruction* i = nadjiInstrukciju(id, ref(instructions));							//vracam instrukciju sa datim id-jem
			(*it)->m_succ.push_back(i);															//ta instrukcija je naslednica 
			i->m_pred.push_back(*it);															//a njena prethodnica je instrukcija B
		}
		else if ((*it)->getType() == I_BLTZ)													//ako je instrukcija BLTZ
		{
			(*it)->m_succ.push_back((*it_succ));												//klasicna naslednica
			std::string id = (*it)->getId();													//isto kao za B
			Instruction* i = nadjiInstrukciju(id, ref(instructions));
			(*it)->m_succ.push_back(i);
			i->m_pred.push_back(*it);
		}
		else																					//ako su sve ostale
		{
			(*it)->m_succ.push_back((*it_succ));
		}
	}
}

void ubaciVarijable(Instructions& instructions) {
	//treba da prodjem kroz sve instrukcije i da u zavisnosti od tipa, inicijalizujem use i def
	for (auto it = instructions.begin(); it != instructions.end(); ++it)
	{
		switch ((*it)->getType())
		{
		case I_AND:
			(*it)->m_def.insert((*it)->m_def.end(), (*it)->m_dst.begin(), (*it)->m_dst.end());//ubacujem u m_def sve elemente iz m_dst
			(*it)->m_use.insert((*it)->m_use.end(), (*it)->m_src.begin(), (*it)->m_src.end());//
			break;
		case I_B:
			//nisam siguran da li ovde uopste imam use i def
			break;
		case I_ADD:
			(*it)->m_def.insert((*it)->m_def.end(), (*it)->m_dst.begin(), (*it)->m_dst.end());
			(*it)->m_use.insert((*it)->m_use.end(), (*it)->m_src.begin(), (*it)->m_src.end());
			break;
		case I_LW:
			(*it)->m_def.insert((*it)->m_def.end(), (*it)->m_dst.begin(), (*it)->m_dst.end());
			(*it)->m_use.insert((*it)->m_use.end(), (*it)->m_src.begin(), (*it)->m_src.end());
			break;
		case I_ABS:
			(*it)->m_def.insert((*it)->m_def.end(), (*it)->m_dst.begin(), (*it)->m_dst.end());
			(*it)->m_use.insert((*it)->m_use.end(), (*it)->m_src.begin(), (*it)->m_src.end());
			break;
		case I_SW:
			(*it)->m_def.insert((*it)->m_def.end(), (*it)->m_dst.begin(), (*it)->m_dst.end());
			(*it)->m_use.insert((*it)->m_use.end(), (*it)->m_src.begin(), (*it)->m_src.end());
			break;
		case I_XOR:
			(*it)->m_def.insert((*it)->m_def.end(), (*it)->m_dst.begin(), (*it)->m_dst.end());
			(*it)->m_use.insert((*it)->m_use.end(), (*it)->m_src.begin(), (*it)->m_src.end());
			break;
		case I_BLTZ:
			(*it)->m_use.insert((*it)->m_use.end(), (*it)->m_src.begin(), (*it)->m_src.end());
			break;
		case I_OR:
			(*it)->m_def.insert((*it)->m_def.end(), (*it)->m_dst.begin(), (*it)->m_dst.end());
			(*it)->m_use.insert((*it)->m_use.end(), (*it)->m_src.begin(), (*it)->m_src.end());
			break;
		case I_LA:
			(*it)->m_def.insert((*it)->m_def.end(), (*it)->m_dst.begin(), (*it)->m_dst.end());
			(*it)->m_use.insert((*it)->m_use.end(), (*it)->m_src.begin(), (*it)->m_src.end());
			break;
		default:
			std::cout << "Doslo je do greske prilikom popunjavanja polja use i def!!!" << std::endl;
			break;
		}
	}
}

bool variableExists(Variable* varijabla, Variables promenljive)									//proverava postojanje promenljive varijabla u listi promenljive
{
	bool pom = false;

	for (auto it = promenljive.begin(); it != promenljive.end(); ++it)
	{
		if ((*it)->getName() == varijabla->getName())
		{
			pom  = true;
			break;
		}
	}

	return pom;
}

void livenessAnalysis(Instructions& instructions) {
	bool goAgain = true;
	while (goAgain) {

		Instructions::reverse_iterator rit;
		for (rit = instructions.rbegin(); rit != instructions.rend(); rit++) {					//prolazim svaku instrukciju od kraja do pocetka
			Variables Out = (*rit)->m_out;														//uzima vrednost koju je out imao u prethodnoj iteraciji	
			Variables In = (*rit)->m_in;														//uzima vrednost koju je in imao u prethodnoj iteraciji
			(*rit)->m_out.clear();																//trenutne vrednosti out se brisu, da bi se ponovo podesile
			//(*rit)->m_in.clear();																//trenutne vrednosti in se brisu, da bi se ponovo podesile

			//podesavam novu vrednost za out
			Instructions::iterator it_out;
			Variables::iterator it_succ;
			for (it_out = (*rit)->m_succ.begin(); it_out != (*rit)->m_succ.end(); it_out++) {			//iteriram kroz instrukcije narednih cvorova
				for (it_succ = (*it_out)->m_in.begin(); it_succ != (*it_out)->m_in.end(); it_succ++) {	//iteriram kroz in-ove
					(*rit)->m_out.push_back((*it_succ));												//ubacujem nove vrednosti u out
				}
			}

			(*rit)->m_out.sort();
			(*rit)->m_out.unique();

			//podesavam novu vrednost za in
			(*rit)->m_in = (*rit)->m_use;															//in je sada isti kao i use, treba dodati jos elemenata

			Variables::iterator it_out2;
			for (it_out2 = (*rit)->m_out.begin(); it_out2 != (*rit)->m_out.end(); it_out2++) {
				if ( !(variableExists( (*it_out2), (*rit)->m_def) ) ) {										//ako se promenljiva iz out ne nalazi u def, treba je ubaciti u in
					(*rit)->m_in.push_back((*it_out2));
				}
			}

			(*rit)->m_in.sort();								//sortiranje liste
			(*rit)->m_in.unique();								//brisanje ne jedinstvenih elemenata iz liste, prvo mora sort obavezno

			if ((*rit)->m_out == Out && (*rit)->m_in == In) {
				goAgain = false;																	//ako ne vazi uslov onda izvrsiti jos jednu iteraciju
			}
		}
	}
}

std::string vratiListuPromenljivih(list<Variable*>& varijable)
{
	std::string ret = "";

	for (auto it = varijable.begin(); it != varijable.end(); ++it)
	{
		ret += (*it)->getName() + " ";
	}

	return ret;
}

void ispisiInstrukcije(Instructions& instructions)
{
	for (auto it = instructions.begin(); it != instructions.end(); ++it)
	{
		(*it)->ispisiInstrukciju();
	}
}