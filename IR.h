#ifndef __IR__
#define __IR__

#include "Types.h"

using namespace std;

/**
 * This class represents one variable from program code.
 */
class Variable
{
public:
	enum VariableType
	{
		MEM_VAR,
		REG_VAR,
		NO_TYPE
	};

	Variable() : m_type(NO_TYPE), m_name(""), m_position(-1), m_assignment(no_assign) {}
	Variable(std::string name, int pos) : m_type(NO_TYPE), m_name(name), m_position(pos), m_assignment(no_assign) {}
	Variable(std::string name, int pos, VariableType tip) : m_type(tip), m_name(name), m_position(pos), m_assignment(no_assign) {}

	std::string getName() const{
		return m_name;
	}

	VariableType getType() const
	{
		return m_type;
	}

	int m_position;					//pozicija promenljive
	Regs m_assignment;				//boja promenljive, tj registar koji ista koristi
private:
	VariableType m_type;
	std::string m_name;				//naziv promenljive
};


/**
 * This type represents list of variables from program code.
 */
typedef std::list<Variable*> Variables;

std::string vratiListuPromenljivih(list<Variable*>& varijable);

/**
 * This class represents one instruction in program code.
 */
class Instruction
{
public:
	Instruction () : m_position(0), m_type(I_NO_TYPE) {}
	Instruction (int pos, InstructionType type, Variables& dst, Variables& src, std::string s, int value = 0) : m_position(pos), m_type(type), id(s), vrednost(value) {
		m_dst = dst;
		m_src = src;
	}

	int m_position;
	InstructionType m_type;								//tip instrukcije
	
	int vrednost;
	std::string id;

	Variables m_dst;									//prosledim preko parametara
	Variables m_src;									//

	Variables m_use;									//lista prom. koje se koriste
	Variables m_def;									//lista prom. koje se definisu
	Variables m_in;
	Variables m_out;
	std::list<Instruction*> m_succ;						//lista narednih instrukcija
	std::list<Instruction*> m_pred;						//lista prethodnih instrukcija

	InstructionType getType() const
	{
		return m_type;
	}

	int getType2() const
	{
		return m_type;
	}

	std::string getId() const
	{
		return id;
	}

	std::string vratiTipInstrukcije(list<Instruction*>& lista)			//koristim za succ i pred, da ispisem tip
	{
		std::string ret = "";
		std:string pom;
		for (auto it = lista.begin(); it != lista.end(); ++it)
		{
			pom = std::to_string( (*it)->getType2() );
			ret += pom + " ";
		}

		return ret;
	}


	void ispisiInstrukciju()				//ispis radim po ugledu na domaci zadatak sa 9. vezbi
	{
		cout << "-------------------------------------" << endl;
		cout << m_position << endl;
		cout << "Type: " << m_type << endl << endl;

		//cout << "DST: " << vratiListuPromenljivih(m_dst) << endl << endl;
		//cout << "SRC: " << vratiListuPromenljivih(m_src) << endl << endl;

		cout << "PRED: " << vratiTipInstrukcije(m_pred) << endl << endl;
		cout << "SUCC: " << vratiTipInstrukcije(m_succ) << endl << endl;

		cout << "USE: " << vratiListuPromenljivih(m_use) << endl << endl;
		cout << "DEF: " << vratiListuPromenljivih(m_def) << endl << endl;
		cout << "IN: " << vratiListuPromenljivih(m_in) << endl << endl;
		cout << "OUT: " << vratiListuPromenljivih(m_out) << endl;
		cout << "-------------------------------------" << endl;
	}
};


/**
 * This type represents list of instructions from program code.
 */
typedef std::list<Instruction*> Instructions;												

//FUNKCIJE KOJE KORISTIM U "LivenessAnalysis.cpp" !!!
Instruction* nadjiInstrukciju(std::string id, Instructions& instructions);
void prethodnici(Instructions& instructions);
void sledbenici(Instructions& instructions);
void ubaciVarijable(Instructions& instructions);
bool variableExists(Variable* varijabla, Variables promenljive);
void livenessAnalysis(Instructions& instructions);
void ispisiInstrukcije(Instructions& instructions);
#endif
