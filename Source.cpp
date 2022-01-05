#include <iostream>
#include <fstream>
#include<string.h>
#include <vector>
#include <string>
using namespace std;
ofstream parsetree("parsetree.txt");
ofstream parsymtable("parser-symboltable.txt");
ofstream transsymtable("translator-symboltable.txt");
ofstream tactxt("tac.txt");
ofstream codetxt("machine-code.txt");
class token
{
public:
	string type;
	string detail;
};

class instruction
{
public:
	string opcode;
	string a;
	string b;
	string c;
};
class variable
{
public:
	string type;
	string name;
	string address;
	string value;
};
class Compiler
{
public:
	vector<token*> tks;
	int indx = 0;
	token look; 
	//DataType of varialbe
	string dataType = "";
	bool findVariable = false;
	//Addresses of Varialbes
	int address = 0;
	int nextaddress = 0;
	vector<string> tac;
    vector<instruction> code;
	string temptac;
	int LineNo = 0;
	bool flag;
	int count;
	vector<variable> variables;
	void nextTok(int depth)
	{
		indx++;
		if (indx < tks.size())
		{
			look.type = tks[indx]->type;
			look.detail = tks[indx]->detail;
		}
		else if (depth > 1)
		{
			cout << "Syntax Error Expected another }";
			exit(0);
		}
	}
};

class LexicalAnalysizer :public Compiler
{

public:
	void FindTokens()
	{

		token* tk;

		string word;
		string prev_word;
		string not_word;
		char ch;


		ifstream fin = OpenFile();
		if (!fin.is_open())
		{
			cout << "File Not Found" << endl;
			return;
		}

		ofstream fout;
		fout.open("words.txt");
		if (!fin.is_open())
		{
			cout << "Error creating words.txt file" << endl;
			return;
		}
		while (!fin.eof())
		{
			int asci = fin.peek();

			if (not_word == "" && CheckAlphaNumeric(asci))
			{
				fin >> noskipws >> ch;
				word = word + ch;
			}
			else
			{
				if (!word.empty())
				{
					tk = new token();

					if (CheckDataType(word))
					{
						fout << "( DT , " << '\'' + word + '\'' << " )\n";
						tk->type = "DT";
						tk->detail = word;
						tks.push_back(tk);


					}
					else if (CheckKeywords(word))
					{
						fout << "( KW , " << '\'' + word + '\'' << " )\n";
						tk->type = "KW";
						tk->detail = word;
						tks.push_back(tk);
					}
					else if (CheckID(word))
					{
						fout << "( ID , " << '\'' + word + '\'' << " )\n";
						tk->type = "ID";
						tk->detail = word;
						tks.push_back(tk);
					}
					else if (CheckNumericConst(word))
					{
						fout << "( NUM , " << '\'' + word + '\'' << " )\n";
						tk->type = "NUM";
						tk->detail = word;
						tks.push_back(tk);
					}
					prev_word = word;
					word = "";
				}
				else
				{
					fin >> ch;

					if (ch != '\n' && ch != ' ' && ch != '\t')
					{
						tk = new token();
						not_word = not_word + ch;
						string str = "";
						if (CheckComment(not_word, str, fin))
						{
							fout << "( COM , " << '\'' + str + '\'' << " )\n";
							tk->type = "COM";
							tk->detail = str;
							tks.push_back(tk);
							not_word = "";
							str = "";

						}
						else if (CheckString(not_word, str, fin))
						{
							fout << "( ST , " << '\'' + str + '\'' << " )\n";
							tk->type = "ST";
							tk->detail = str;
							tks.push_back(tk);
							not_word = "";
							str = "";

						}
						else if (CheckLiteralConst(not_word))
						{

							fout << "( LC , " << '\'' + not_word + '\'' << " )\n";
							tk->type = "LC";
							tk->detail = not_word;
							tks.push_back(tk);
							not_word = "";

						}
						else if (CheckInputOps(not_word, fin))
						{
							fout << "( IO , " << "\'>>\'" << " )\n";
							tk->type = "IO";
							tk->detail = ">>";
							tks.push_back(tk);
							not_word = "";

						}
						else if (CheckAssignmentOps(not_word, fin))
						{
							fout << "( AS , " << '\'' + not_word + "=\'" << " )\n";
							tk->type = "AS";
							tk->detail = ":=";
							tks.push_back(tk);
							not_word = "";

						}
						else if (CheckArithmeticOps(not_word))
						{
							fout << "( AO , " << '\'' + not_word + '\'' << " )\n";
							tk->type = "AO";
							tk->detail = not_word;
							tks.push_back(tk);
							not_word = "";

						}
						else if (CheckRelationalOps(not_word))
						{
							fout << "( RO , " << '\'' + not_word + '\'' << " )\n";
							tk->type = "RO";
							tk->detail = not_word;
							tks.push_back(tk);
							not_word = "";

						}
						else if (CheckVarDecOp(prev_word, not_word))
						{
							fout << "( VO , " << '\'' + not_word + '\'' << " )\n";
							tk->type = "VO";
							tk->detail = not_word;
							tks.push_back(tk);
							not_word = "";

						}
						else if (CheckBrackets(not_word))
						{
							fout << "( BR , " << '\'' + not_word + '\'' << " )\n";
							tk->type = "BR";
							tk->detail = not_word;
							tks.push_back(tk);
							not_word = "";

						}
						else if (CheckPunctution(prev_word, not_word))
						{
							fout << "('" + not_word + "'" << "^" << " )\n";
							tk->type = not_word;
							tk->detail = "^";
							tks.push_back(tk);
							not_word = "";

						}
					}
					else
					{
						not_word = "";

					}
					ch = ' ';
				}
			}
		}
	}

private:
	bool CheckDataType(string str)
	{
		if (str == "Integer" || str == "char")
		{
			return true;
		}
		return false;
	}

	bool CheckKeywords(string str)
	{
		if (str == "if" || str == "elif" || str == "else" || str == "while" || str == "In" || str == "func" || str == "print" || str == "println" || str == "true" || str == "false" || str == "OR" || str == "AND")
		{
			return true;
		}
		return false;
	}

	bool CheckArithmeticOps(string str)
	{
		if (str == "+" || str == "-" || str == "*" || str == "/")
		{
			return true;
		}
		return false;
	}

	bool CheckRelationalOps(string str)
	{
		if (str == ">" || str == ">=" || str == "<" || str == "<=" || str == "==" || str == "!=")
		{
			return true;
		}
		return false;
	}

	bool CheckComment(string ch, string& str, ifstream& fin)
	{
		int asci;
		bool flag = false;
		char c;
		if (ch[0] == '/')
		{
			str = '/';
			asci = fin.peek();
			if (asci == '*')
			{
				fin >> c;
				str = str + c;
				while (!fin.eof())
				{
					fin >> c;
					str = str + c;
					if (c == '*')
					{
						fin >> c;
						str = str + c;
						if (c == '/')
						{
							return true;
						}
					}
				}
				return true;

			}
			else
			{
				return false;
			}
		}
		return false;
	}

	bool CheckID(string str)
	{
		int st = 1;
		for (int i = 0; i < str.size(); i++)
		{
			if (i == 0)
			{
				if (isalpha(str[i]))
				{
					st = 2;
					if (i == str.size() - 1)
					{
						return true;
					}
					break;
				}
				else
				{
					return false;
				}
			}
			else
			{

				if (isalpha(str[i]) || isdigit(str[i]))
				{
					st = 2;
					if (i == str.size() - 1)
					{
						return true;
					}
					break;
				}
				else
				{
					return true;
				}
			}
		}
	}

	bool CheckNumericConst(string str)
	{
		for (int i = 0; i < str.size(); i++)
		{
			if (str[i] < '0' || str[i] > '9')
			{
				return false;
			}
		}
		return true;
	}

	bool CheckVarDecOp(string prev, string str)
	{
		if (CheckDataType(prev))
		{
			if (str[0] == ':')
			{
				return true;
			}
		}

		return false;
	}

	bool CheckLiteralConst(string str)
	{
		if (str.size() >= 3)
		{
			if (str[0] == '\'')
			{
				if (isalpha(str[1]) || isdigit(str[1]))
				{
					if (str[2] == '\'')
					{
						return true;
					}
				}
			}
		}
		return false;
	}

	bool CheckString(string ch, string& str, ifstream& fin)
	{
		int st = 1, f;
		char c;
		bool flag = false;

		if (ch[0] == '\"')
		{
			str = '\"';
			while (!fin.eof())
			{
				fin >> c;
				if (c == '\"')
				{

					str = str + c;
					return true;
				}
				else
				{
					str = str + c;
					st = 2;
				}
			}
		}
		return false;
	}

	bool CheckBrackets(string str)
	{
		if (str[0] == '(' || str[0] == ')' || str[0] == '{' || str[0] == '}' || str[0] == '[' || str[0] == ']')
		{
			return true;
		}
		return false;
	}

	bool CheckAssignmentOps(string str, ifstream& fin)
	{

		if (str[0] == ':')
		{
			int c = fin.peek();
			if (c == '=')
			{
				char temp;
				fin >> temp;
				return true;
			}
			else
				return false;
		}
		return false;
	}

	bool CheckInputOps(string str, ifstream& fin)
	{
		if (str == ">")
		{
			int c = fin.peek();
			if (c == '>')
			{
				char temp;
				fin >> temp;
				return true;
			}
		}
		return false;

	}

	bool CheckPunctution(string prev, string str)
	{
		if (str[0] == ';' || str[0] == ',')
		{
			return true;
		}
		else if (!CheckDataType(prev))
		{
			if (str[0] == ':')
			{
				return true;
			}
		}
		return false;
	}

	bool CheckAlphaNumeric(int asci)
	{
		if (isdigit(asci) || isalpha(asci))
		{
			return true;
		}
		return false;
	}

	bool checkFileExtension(string file)
	{
		int len = file.size();
		if (len >= 4)
		{


			if (file[len - 1] == '\0' || file[len - 1] == '\n' || file[len - 1] == ' ')
			{
				if (file[len - 4] == '.' && file[len - 3] == 'g' && file[len - 2] == 'o')
					return true;
			}
			else
			{
				if (file[len - 3] == '.' && file[len - 2] == 'g' && file[len - 1] == 'o')
					return true;
			}
			return false;
		}
	}

	ifstream OpenFile()
	{
		string path;
		cout << "Please enter path of code file" << endl;
		cin >> path;
		while (!checkFileExtension(path))
		{
			cout << "Wrong File Extension! File must be .go. Return Path ";
			cout << endl;
			cin >> path;
		}
		ifstream fin(path);
		if (fin)
		{
			return fin;
		}
		else
		{
			cout << "File Not Found";
			exit(0);
		}
		
	}


};

class Parser :public LexicalAnalysizer
{

public:
	void writeTacToFile()
	{
		for (int i = 0; i < tac.size(); i++)
		{
			tactxt << (i + 1) << ")\t" << tac[i] << endl;
		}
	}
	void translator(string name)
	{
		if (tac.size() <= LineNo)
		{
			tac.push_back(name);
		}
		else
		{
			tac[LineNo] += name;
		}

	}
	void MachineCode()
	{
		
		for (int i = 0; i < tac.size(); i++)
		{
			string opcode = "";
			string a = "";
			string b = "";
			string c = "";
			string line = "";
			for (int j = 0; j < tac[i].size(); j++)
			{
				if (tac[i][j] == ':')
				{

					c = AssignAdress(line);
					opcode = "0";
					line = "";
				}
				else if (tac[i][j] == '+')
				{
					a = AssignAdress(line);
					opcode = "1";
					line = "";
				}
				else if (tac[i][j] == '-')
				{
					a = AssignAdress(line);
					opcode = "2";
					line = "";
				}
				else if (tac[i][j] == '*')
				{
					a = AssignAdress(line);
					opcode = "3";
					line = "";
				}
				else if (tac[i][j] == '/')
				{
					a = AssignAdress(line);
					opcode = "4";
					line = "";
				}
				else if (tac[i][j] == '>')
				{
					opcode = "9";
					a = AssignAdress(line);
					line = "";
				}
				else if (tac[i][j] == '<')
				{
					opcode = "10";
					a = AssignAdress(line);
					line = "";
				}
				else if (tac[i][j] == '=')
				{
					if (opcode == "9")
					{
						opcode = "11";
						line = "";
					}
					if (opcode == "10")
					{
						opcode = "12";
						line = "";
					}
					else if (opcode == "")
					{
						if (line == "=")
						{
							opcode = "13";
							line = "";
						}
						else
						{
							a = AssignAdress(line);
							line = "=";
						}					
					}
				
				}
				else if (tac[i][j] == ' ')
				{
					if (line == "print")
					{
						opcode = "5";
					}
					else if (line == "println")
					{
						opcode = "6";
					}
					else if (line == "In")
					{
						opcode = "7";
					}
					else if (line == "ret")
					{
						opcode = "14";
					}
					else if (line == "goto")
					{
						if (opcode == "")
						{
							opcode = "8";
						}
					}
					else if (opcode == "9" || opcode == "10" || opcode == "11" || opcode == "12" || opcode == "13")
					{
						if (a != "" && b == "")
						{
							b = AssignAdress(line);
						}

					}
					if (line[0] != '\"')
					{
						line = "";
					}
					else
					{
						line += tac[i][j];
					}
					
				}
				else
				{
					line += tac[i][j];
				}

			}
			if (opcode == "5" || opcode == "6" || opcode == "7" || opcode == "8" || opcode == "9" || opcode == "10" || opcode == "11" || opcode == "12" || opcode == "13" || opcode == "14")
			{
				c = AssignAdress(line);
			}
			if (opcode == "0")
			{
				a = AssignAdress(line);
			}
			if (opcode == "1" || opcode == "2" || opcode == "3" || opcode == "4")
			{
				b = AssignAdress(line);
			}
			instruction ints;
			ints.opcode = opcode;
			ints.a = a;
			ints.b = b;
			ints.c = c;
			code.push_back(ints);
			codetxt << opcode << "	" << a << "	" << b << "	" << c << endl;
		}
	}
	string AssignAdress(string var)
	{
		char* p;
		string t;
		variable v;
		long converted = strtol(var.c_str(), &p, 10);
		if (*p) {
			string adrs = lookforaddress(var);
			if(adrs =="")
			{
				if (var[0] == '\'' || var[0] == '\"')
				{
					string temp = "T" + to_string(variables.size());
					transsymtable << "char	";
					transsymtable << temp << "	";
					v.type = "char";
					v.name = temp;
					v.address = to_string(address);
					transsymtable << address << "	";
					v.value = var;
					variables.push_back(v);
					transsymtable << var << endl;
					nextaddress = 1;
					address = address + nextaddress;
					return v.address;
				}
				else
				{
					cout << "Varaible Not Declared " + var;
					exit(0);
				}
			}
			else
			{
				return adrs;
			}
		}
		else {
			string temp = "T" + to_string(variables.size());
			transsymtable << "Integer	";
			transsymtable << temp << "	";			
			v.type = "Integer";
			v.name = temp;
			v.address = to_string(address);
			transsymtable << address << "	";
			v.value = var;
			variables.push_back(v);
			transsymtable << var << endl;
			nextaddress = 4;
			address = address + nextaddress;
			// use converted
		}
		return v.address;
	}
	string lookforaddress(string var)
	{
		for (int i = 0; i < variables.size(); i++)
		{
			if (variables[i].name == var)
			{
				return variables[i].address;
			}
		}
		return "";
	}
	void ShowDepth(int depth)
	{
		for (int i = 0; i < depth; i++)
		{
			parsetree << ":_ ";
		}
		
	}
	void match(string tok, int depth) 
	{
		ShowDepth(depth);
		if (look.detail == tok || look.type == tok)
		{
			if (look.detail == "^")
			{
				parsetree << look.type << endl;
			}
			else
			{
				parsetree << look.detail << endl;
			}
			//Symbol table
			if (look.type == "ID" && findVariable==true)
			{
				variable v;
				parsymtable << dataType +"	";
				v.type = dataType;
				parsymtable << look.detail +"	" << endl;
				v.name = look.detail;
				v.address = to_string(address);
				transsymtable << dataType + "	";
				transsymtable << look.detail + "	";
				transsymtable << address << "	";
				if (dataType == "Integer")
				{
					v.value="0";
					transsymtable << "0" << endl;
				}
				else
				{
					v.value="";
					transsymtable << "" << endl;
				}
				variables.push_back(v);
				address = address + nextaddress;
			}
			if (look.type == "DT")
			{
				if (look.detail == "char")
				{
					nextaddress = 1;
				}
				else
				{
					nextaddress = 4;
				}					
			}
			nextTok(depth);
		}
		else
		{
			if (look.type == "COM")
			{
				nextTok(depth);
				match(tok, depth);
			}
			else
			{
				cout << "Bad Token ";
				cout << look.type;
				cout << " ";
				cout << look.detail;
				cout << " ";
				cout << indx << endl;
				cout << "Expected: " << tok;
				exit(0);
				
			}
			
		}
	}
	void AddtempVariable(int te1,int te2)
	{
		for (int i = te1; i < te2; i++)
		{
			string temp = "T" + to_string(i);
			transsymtable << "Integer	";
			transsymtable << temp<<"	" ;
			variable v;
			v.type = "Integer";
			v.name = temp;
			v.address = to_string(address);
			v.value = "0";
			variables.push_back(v);
			transsymtable << address <<"	";
			transsymtable << "0" << endl;
			nextaddress = 4;
			address = address + nextaddress;		
		}		
	}
	void E(int depth)
	{
		ShowDepth(depth);
		parsetree << "E()" << endl;
		translator("T" + to_string(LineNo) + ":= ");
		flag = false;
		T(depth + 1);
		E_dash(depth + 1);
	}
	void T(int depth)
	{
		ShowDepth(depth);
		parsetree << "T()" << endl;
		F(depth + 1);
		T_dash(depth + 1);
	}
	void E_dash(int depth)
	{
		ShowDepth(depth);
		parsetree << "E_dash()" << endl;
		if (look.detail == "+")
		{
			temptac += look.detail;
			translator(look.detail);
			match("+", depth + 1);
			T(depth + 1);
			E_dash(depth + 1);
		}
		else if (look.detail == "-")
		{
			temptac += look.detail;
			translator(look.detail);
			match("-", depth + 1);
			T(depth + 1);
			E_dash(depth + 1);
		}
		else
			;
	}
	void F(int depth)
	{
		ShowDepth(depth);
		parsetree << "F()" << endl;
		count++;
		if (look.type == "ID")
		{
			if (flag == false)
			{
				temptac += look.detail;
				translator(look.detail);
				flag = true;
				match("ID", depth + 1);
			}
			else 
			{
				if (tac[LineNo].size() > 5)
				{
					temptac += look.detail;
					translator(look.detail);
					LineNo++;
					match("ID", depth + 1);
					if (look.detail == "+" || look.detail == "-" || look.detail == "+" || look.detail == "*" || look.detail == "/")
					{
						translator("T" + to_string(LineNo) + ":= " + "T" + to_string(LineNo - 1));
					}
					
				}
				else
				{		
					temptac += look.detail;
					translator(look.detail);
					match("ID", depth + 1);
				}
				
			}			
			
		}
		else if (look.type == "NUM")
		{
			if (flag == false)
			{
				temptac += look.detail;
				translator(look.detail);
				flag = true;
				match("NUM", depth + 1);
			}
			else
			{
				if (tac[LineNo].size() > 5)
				{
					temptac += look.detail;
					translator(look.detail);
					LineNo++;
					match("NUM", depth + 1);
					if (look.detail == "+" || look.detail == "-" || look.detail == "+" || look.detail == "*" || look.detail == "/")
					{
						translator("T" + to_string(LineNo) + ":= " + "T" + to_string(LineNo - 1));
						temptac = "T" + to_string(LineNo);
					}
				}
				else
				{
					temptac += look.detail;
					translator(look.detail);
					match("NUM", depth + 1);
				}
			}
			
		}
		else if (look.detail == "(")
		{
			match("(", depth + 1);
			T(depth + 1);
			match(")", depth + 1);
		}
		else
			cout << "Bad Token ";
	}
	void T_dash(int depth)
	{
		ShowDepth(depth);
		parsetree << "T_dash()" << endl;
		if (look.detail == "*")
		{
			temptac += look.detail;
			translator(look.detail);
			match("*", depth + 1);
			F(depth + 1);
			T_dash(depth + 1);
		}
		else if (look.detail == "/")
		{
			temptac += look.detail;
			translator(look.detail);
			match("/", depth + 1);
			F(depth + 1);
			T_dash(depth + 1);
		}
	}
	void S_star(int depth)
	{
		ShowDepth(depth);
		parsetree << "S_star()" << endl;
		match("func", depth + 1);
		match("DT", depth + 1);
		match("VO", depth + 1);
		match("ID", depth + 1);
		match("(", depth + 1);
		findVariable = true;
		while (look.detail != ")")
		{
			dataType = look.detail;
			A(depth + 1);
		}
		findVariable = false;
		match(")", depth + 1);
		match("{", depth + 1);
		while (look.detail != "}")
		{
			S(depth + 1);
		}
		match("}", depth + 1);
		if (indx < tks.size())
		{
			S_star(depth);
		}
		
	}
	void A(int depth)
	{
		ShowDepth(depth);
		parsetree << "A()" << endl;
		if (look.type == "DT")
		{
			match("DT", depth + 1);
			match("VO", depth + 1);
			match("ID", depth + 1);
			if (look.detail != ")")
			{
				match(",", depth + 1);
			}
		}
	}
	void S(int depth)
	{
		ShowDepth(depth);
		parsetree << "S()" << endl;
		if (look.detail == "while")
		{
			match("while", depth + 1);
			int line = LineNo;
			H(depth + 1);
			match(":", depth + 1);
			temptac = "if " + tac[line] + " goto " + to_string(line + 3);
			translator(temptac);
			LineNo++;
			tac.erase(tac.begin() + line);
			LineNo--;
			temptac = "goto ";
			translator(temptac);
			LineNo++;
			match("{", depth + 1);
			while (look.detail != "}")
			{
				S(depth + 1);
			}
			match("}", depth + 1);
			temptac = "goto " + to_string(line + 1);
			tac[line+1]+= to_string(LineNo + 2);
			translator(temptac);
			LineNo++;
		}
		else if (look.detail == "if")
		{
			translator("if ");
			match("if", depth + 1);
			R(depth);
		}
		else if (look.type == "DT")
		{
			dataType = look.detail;
			findVariable = true;
			match("DT", depth + 1);
			match("VO", depth + 1);
			match("ID", depth + 1);
			C(depth + 1);
			findVariable = false;
			match(";", depth + 1);
		}
		else if (look.type == "ID")
		{
			if (look.detail == "write")
			{
				
				match("ID", depth + 1);
				match("(", depth + 1);
				count = 0;
				Y(depth);
				translator("call write "+ to_string(count));
				LineNo++;
				match(")", depth + 1);
				match(";", depth + 1);
			}
			else if (look.detail == "ret")
			{
				string temptac1 = look.detail+ " ";
				match("ret", depth + 1);
				count = 0;
				temptac = "";
				D(depth);
				if (tac.size() > LineNo)
				{
					LineNo++;
				}

				if (count < 3)
				{
					tac.erase(tac.begin() + LineNo - 1);
					LineNo--;
					translator(temptac1 + temptac);
				}
				else
				{
					translator(temptac1 + "T" + to_string(LineNo - 1));
				}

				LineNo++;
				match(";", depth + 1);
			}
			else
			{
			
				string temptac1 = look.detail;
				match("ID", depth + 1);
				temptac1 += look.detail;
				match("AS", depth + 1);
				count = 0;
				temptac = "";
				int templineno = LineNo;
				D(depth);
				if (tac.size() >LineNo)
				{
					LineNo++;
				}
				
				if (count < 3)
				{
					tac.erase(tac.begin() + LineNo - 1);
					LineNo--;
					translator(temptac1 + temptac);
				}
				else
				{
					AddtempVariable(templineno, LineNo);
					translator(temptac1 + "T"+to_string(LineNo-1));
				}
				
				LineNo++;
				match(";", depth + 1);
			}
		}
		else if (look.detail == "print")
		{
			translator("print ");
			match("print", depth + 1);
			match("(", depth + 1);
			if (look.type == "ST")
			{
				translator(look.detail);
				match("ST", depth + 1);
			}
			else if (look.type == "ID")
			{
				translator(look.detail);
				match("ID", depth + 1);
			}
			else if (look.type == "LC")
			{
				translator(look.detail);
				match("LC", depth + 1);
			}
			else if (look.type == "NUM")
			{
				translator(look.detail);
				match("NUM", depth + 1);
			}
			LineNo++;
			match(")", depth + 1);
			match(";", depth + 1);
		}
		else if (look.detail == "println")
		{
			translator("println ");
			match("println", depth + 1);
			match("(", depth + 1);
			if (look.type == "ST")
			{
				translator(look.detail);
				match("ST", depth + 1);
			}
			else if (look.type == "ID")
			{
				translator(look.detail);
				match("ID", depth + 1);
			}
			else if (look.type == "LC")
			{
				translator(look.detail);
				match("LC", depth + 1);
			}
			else if (look.type == "NUM")
			{
				translator(look.detail);
				match("NUM", depth + 1);
			}
			LineNo++;
			match(")", depth + 1);
			match(";", depth + 1);
		}
		else if (look.detail == "In")
		{
		
			match("In", depth + 1);
			match("IO", depth + 1);
			translator("In " + look.detail);
			LineNo++;
			match("ID", depth + 1);
			match(";", depth + 1);
		}
		else if (look.type == "COM")
		{
			match("COM", depth + 1);
		}
	}
	void C(int depth)
	{
		ShowDepth(depth);
		parsetree << "C()" << endl;
		if (look.type != ";")
		{
			C_dash(depth + 1);
		}
	}
	void C_dash(int depth)
	{
		ShowDepth(depth);
		parsetree << "C_dash()" << endl;
		match(",", depth + 1);
		match("ID", depth + 1);
		if (look.type != ";")
		{
			C(depth + 1);
		}
	
	}
	void H(int depth)
	{
		ShowDepth(depth);
		parsetree << "H()" << endl;
		if (look.detail == "true")
		{
			translator("true ");
			match("true", depth + 1);
		}
		else if(look.detail == "false")
		{
			translator("false ");
			match("false", depth + 1);
		}
		else
		{
			K(depth + 1);
			translator(look.detail);
			match("RO", depth + 1);
			K(depth + 1);
			if (look.detail == "OR")
			{
				translator(look.detail);
				match("OR", depth + 1);
				H_dash(depth + 1);
			}
			else if (look.detail == "AND")
			{
				translator(look.detail);
				match("AND", depth + 1);
				H_dash(depth + 1);
			}
		}
		LineNo++;
	}
	void H_dash(int depth)
	{
		ShowDepth(depth);
		parsetree << "H_dash()" << endl;
		K(depth + 1);
		translator(look.detail);
		match("RO", depth + 1);
		K(depth + 1);
		if (look.detail == "OR")
		{
			translator(look.detail);
			match("OR", depth + 1);
			H_dash(depth + 1);
		}
		else if(look.detail == "AND")
		{
			translator(look.detail);
			match("AND", depth + 1);
			H_dash(depth + 1);
		}
	}
	void K(int depth)
	{
		ShowDepth(depth);
		parsetree << "K()" << endl;
		if (look.type == "ID")
		{
			translator(look.detail);
			match("ID", depth + 1);
		}
		else if (look.type == "NUM")
		{
			translator(look.detail);
			match("NUM", depth + 1);
		}
	}
	void R(int depth)
	{
		ShowDepth(depth);
		parsetree << "R()" << endl;
		H(depth + 1);
		match(":", depth + 1);
		int gotoForIf = LineNo;
		tac[LineNo - 1] += " goto " + to_string(LineNo + 2);
		translator("goto ");
		LineNo++;
		match("{", depth + 1);
		while (look.detail != "}")
		{
			S(depth + 1);
		}
		match("}", depth + 1);
		int gotoforelif=-1;
		if (look.detail == "else" || look.detail == "elif")
		{
			gotoforelif = LineNo;
			translator("goto ");
			LineNo++;
		}
		tac[gotoForIf] += to_string(LineNo + 1);
		T2(depth + 1);
		if (gotoforelif != -1)
		{
			tac[gotoforelif] += to_string(LineNo + 1);
		}
		
	}
	void T2(int depth)
	{
		ShowDepth(depth);
		parsetree << "T2()" << endl;
		if (look.detail == "elif")
		{
			translator("elif ");
			match("elif", depth + 1);
			R(depth + 1);
		}
		else if (look.detail == "else")
		{		
			match("else", depth + 1);
			match("{", depth + 1);
			while (look.detail != "}")
			{
				S(depth + 1);
			}
			match("}", depth + 1);
		}


	}
	void Y(int depth)
	{
		ShowDepth(depth);
		parsetree << "Y()" << endl;;
		if (look.type == "ID")
		{
			count++;
			translator("param "+look.detail);
			LineNo++;
			match("ID", depth + 1);
			if (look.type == ",")
			{
				match(",", depth + 1);
				Y(depth + 1);
			}

		}
		else if (look.type == "LC")
		{
			count++;
			translator("param " + look.detail);
			LineNo++;
			match("LC", depth + 1);
			if (look.type == ",")
			{
				match(",", depth + 1);
				Y(depth);
			}
		}
		else if (look.type == "NUM")
		{
			count++;
			translator("param " + look.detail);
			LineNo++;
			match("NUM", depth + 1);
			if (look.type == ",")
			{
				match(",", depth + 1);
				Y(depth + 1);
			}
		}
	}
	void D(int depth)
	{
		ShowDepth(depth);
		parsetree << "D()" << endl;
		if (look.type == "LC")
		{
			translator(look.detail);
			temptac = look.detail;
			match("LC", depth + 1);
		}
		else
		{
			E(depth + 1);
		}
	}
};

class VirtualMachine : public Parser
{
	variable FindVariable(string address)
	{
		int startsearch = (atoi(address.c_str()) / 4);
		for (int i = startsearch; i < variables.size(); i++)
		{
			if (variables[i].address == address)
			{
				return variables[i];
			}
		}
	}
	void StoreValue(int value,string destination)
	{
		int startsearch = atoi(destination.c_str()) / 4;
		string val = to_string(value);
		for (int i = startsearch; i < variables.size(); i++)
		{
			if (variables[i].address == destination)
			{
				variables[i].value = val;
				break;
			}
		}
	}
	void StoreValuestr(string value, string destination)
	{
		int startsearch = atoi(destination.c_str()) / 4;
		for (int i = startsearch; i < variables.size(); i++)
		{
			if (variables[i].address == destination)
			{
				variables[i].value = value;
				break;
			}
		}
	}
public: 
	int RunCode()
	{
		for (int i = 0; i < code.size(); i++)
		{
			variable a;
			variable b;
			variable c;
			int result;
			string in;
			int inter;
			switch (atoi(code[i].opcode.c_str()))
			{
			case 0:
				a = FindVariable(code[i].a);
				StoreValuestr(a.value,code[i].c);
				break;
			case 1:
				a = FindVariable(code[i].a);
				b = FindVariable(code[i].b);
				result = atoi(a.value.c_str()) + atoi(b.value.c_str());
				StoreValue(result, code[i].c);
				break;
			case 2: 
				a = FindVariable(code[i].a);
				b = FindVariable(code[i].b);
				result = atoi(a.value.c_str()) - atoi(b.value.c_str());
				StoreValue(result, code[i].c);
				break;
			case 3: 
				a = FindVariable(code[i].a);
				b = FindVariable(code[i].b);
				result = atoi(a.value.c_str()) * atoi(b.value.c_str());
				StoreValue(result, code[i].c);
				break;
			case 4: 
				a = FindVariable(code[i].a);
				b = FindVariable(code[i].b);
				result = atoi(a.value.c_str()) / atoi(b.value.c_str());
				StoreValue(result, code[i].c);
				break;
			case 5:
				c = FindVariable(code[i].c);
				if (c.type == "char")
				{
					c.value = c.value.substr(1, c.value.size() - 2);
				}
				cout << c.value;
				break;
			case 6:
				c = FindVariable(code[i].c);
				if (c.type == "char")
				{
					c.value = c.value.substr(1, c.value.size() - 2);
				}
				cout << c.value <<endl;
				break;
			case 7:
				c = FindVariable(code[i].c);
				if (c.type == "Integer")
				{
					cin >> inter;
					StoreValue(inter, code[i].c);
				}
				else
				{
					cin >> in;
					StoreValuestr(in, code[i].c);
				}			
				break;
			case 8:
				c = FindVariable(code[i].c);
				i = atoi(c.value.c_str()) - 2;
				break;
			case 9:
				a = FindVariable(code[i].a);
				b = FindVariable(code[i].b);
				c = FindVariable(code[i].c);
				if (atoi(a.value.c_str())> atoi(b.value.c_str()))
				{
					i = atoi(c.value.c_str()) - 2;
				}
				break;
			case 10:
				a = FindVariable(code[i].a);
				b = FindVariable(code[i].b);
				c = FindVariable(code[i].c);
				if (atoi(a.value.c_str()) < atoi(b.value.c_str()))
				{
					i = atoi(c.value.c_str()) - 2;
				}
				break;
			case 11:
				a = FindVariable(code[i].a);
				b = FindVariable(code[i].b);
				c = FindVariable(code[i].c);
				if (atoi(a.value.c_str()) >= atoi(b.value.c_str()))
				{
					i = atoi(c.value.c_str()) - 2;
				}
				break;
			case 12:
				a = FindVariable(code[i].a);
				b = FindVariable(code[i].b);
				c = FindVariable(code[i].c);
				if (atoi(a.value.c_str()) <= atoi(b.value.c_str()))
				{
					i = atoi(c.value.c_str()) - 2;
				}
				break;
			case 13:
				a = FindVariable(code[i].a);
				b = FindVariable(code[i].b);
				c = FindVariable(code[i].c);
				if (atoi(a.value.c_str()) == atoi(b.value.c_str()))
				{
					i = atoi(c.value.c_str()) - 2;
				}
				break;
			case 14: 
				c = FindVariable(code[i].c);
				if (c.type == "Integer")
				{
					i = code.size();
					break;
				}
				else
				{
					i = code.size();
					break;					
				}
			}
		}
		return 0;
	}
};
int main()
{
	VirtualMachine P = VirtualMachine();
	P.FindTokens();
	P.look.type = P.tks[P.indx]->type;
	P.look.detail = P.tks[P.indx]->detail;
	P.S_star(0);
	P.writeTacToFile();
	P.MachineCode();
	P.RunCode();
}