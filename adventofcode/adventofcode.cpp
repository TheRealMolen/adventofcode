//
//   https://adventofcode.com/2017
//

// --- by @TheRealMolen ---

#include "stdafx.h"


// ------------------------------------------

int ms1_invcaptcha(const string& code, bool deluxeMode = false)
{
	uint32_t answer = 0;
	size_t inc = 1;
	if (deluxeMode)
		inc = code.size() / 2;

	for (uint32_t i = 0; i < code.size(); i++)
	{
		uint32_t j = (i + inc) % code.size();
		if (code[i] == code[j])
		{
			answer += ((uint32_t)code[i]) - ((uint32_t)'0');
		}
	}
	return answer;
}


bool test_ms1()
{
	if (ms1_invcaptcha("1111") != 4)
		return false;
	if (ms1_invcaptcha("1122") != 3)
		return false;
	if (ms1_invcaptcha("1234") != 0)
		return false;
	if (ms1_invcaptcha("91212129") != 9)
		return false;

	if (ms1_invcaptcha("1212", true) != 6)
		return false;
	if (ms1_invcaptcha("1221", true) != 0)
		return false;
	if (ms1_invcaptcha("123425", true) != 4)
		return false;
	if (ms1_invcaptcha("123123", true) != 12)
		return false;
	if (ms1_invcaptcha("12131415", true) != 4)
		return false;

	return true;
}

// ------------------------------------------------------

bool _ms2_parse_sheet(const string& in, list<vector<int32_t>>& out)
{
	out.clear();
	vector<int32_t> row;
	size_t pix = 0;
	size_t ix = in.find_first_of(" \t;", pix);
	for (; ix != string::npos; ix = in.find_first_of(" \t;", pix))
	{
		string cellStr = in.substr(pix, ix - pix);
		int32_t cell = atoi(cellStr.c_str());
		row.push_back(cell);
		
		if (in[ix] == ';')
		{
			out.push_back(row);
			row.clear();
		}
		else if(in[ix] != ' ' && in[ix] != '\t')
			return false;

		pix = ix + 1;
	}

	return true;
}

int ms2_chexum(const string& spreadsheetTxt)
{
	int chex = 0;

	list<vector<int32_t>> sheet;
	_ms2_parse_sheet(spreadsheetTxt, sheet);

	for (auto& row : sheet)
	{
		auto maxC = *max_element(row.cbegin(), row.cend());
		auto minC = *min_element(row.cbegin(), row.cend());
		chex += maxC - minC;
	}

	return chex;
}

bool test_ms2()
{
	if (ms2_chexum("5\t1\t9\t5;7 5 3;2 4 6 8;") != 18)
		return false;
	return true;
}

int ms2b_multiplechex(const string& spreadsheetTxt)
{
	list<vector<int32_t>> sheet;
	_ms2_parse_sheet(spreadsheetTxt, sheet);

	int chex = 0;
	for (auto& row : sheet)
	{
		// find pair of elements where one is a multiple of the other
		for (auto cell : row)
		{
			bool found = false;
			for (auto tcell : row)
			{
				if (cell == tcell)
					continue;

				auto hi = max(cell, tcell);
				auto lo = min(cell, tcell);
				if (hi % lo == 0)
				{
					chex += hi / lo;
					found = true;
					break;
				}
			}
			if (found)
				break;
		}
	}

	return chex;
}

bool test_ms2b()
{
	if (ms2b_multiplechex("5 9 2 8;9 4 7 3;3 8 6 5;") != 9)
		return false;
	return true;
}

//-------------------------------------------------------------------------------

int square(int n) { return n*n; }

int _ms3_calc_shell_size(int shell) { return (2 * shell) + 1; }
int _ms3_calc_shell_max(int shell) { return square(_ms3_calc_shell_size(shell)); }
int _ms3_calc_shell_min(int shell) { return _ms3_calc_shell_max(shell - 1) + 1; }
int _ms3_calc_shell_len(int shell) { return 8 * shell; }

// figure out the shell we're in
int _ms3_get_shell_for_loc(int location)
{
	for (int shell = 0; ; shell++)
	{
		int shellMax = _ms3_calc_shell_max(shell);
		if (shellMax >= location)
			return shell;
	}

	return -1;
}

// get the end location of the quadrant for a location
int _ms3_get_end_quad_for_loc(int location, int* pOutQuadrant = nullptr)
{
	int shell = _ms3_get_shell_for_loc(location);
	int shellMax = _ms3_calc_shell_max(shell);
	int shellSize = _ms3_calc_shell_size(shell);

	int quadEnd = shellMax;
	int quadStart, quad;
	for (quad = 3; quad >= 0; quad--)
	{
		quadStart = quadEnd - (shellSize - 1);
		if (location > quadStart)
			break;
		quadEnd = quadStart;
	}

	if (pOutQuadrant)
		*pOutQuadrant = quad;

	return quadEnd;
}


int ms3_spiral_dist(int location)
{
	int shell = _ms3_get_shell_for_loc(location);
	int shellMax = _ms3_calc_shell_max(shell);
	int shellSize = _ms3_calc_shell_size(shell);

	// figure out which quadrant of the shell we're in
	int quadEnd = _ms3_get_end_quad_for_loc(location);

	// figure out our distance from the middle of the quadrant
	int shellHw = (shellSize - 1) / 2;
	int quadMid = quadEnd - shellHw;
	int quadPos = abs(location - quadMid);

	return shell + quadPos;
}

// returns the first number bigger than test
int ms3b_spiral_seq(int test)
{
	vector<int> seq;
	seq.reserve(1000);
	seq.push_back(1);
	seq.push_back(1);
	
	// NB: locations are 1-based. i might hate myself...
	for (auto loc = seq.size() + 1; seq.back() <= test; loc++)
	{
		// calc the next value and store it
		int val = seq.back();	// always includes n-1

		// classify location relative to previous corner
		int shell = _ms3_get_shell_for_loc(loc);
		int shellMax = _ms3_calc_shell_max(shell);
		int shellMin = _ms3_calc_shell_max(shell - 1) + 1;

		if (loc == shellMax)
		{
			// just need to add in end of prev shell and start of this one
			int ix = _ms3_calc_shell_max(shell - 1);
			val += seq[ix - 1];
			val += seq[shellMin - 1];
		}
		else if (loc == shellMin)
		{
			// just need to add in start of prev shell
			int ix = _ms3_calc_shell_min(shell - 1);
			val += seq[ix - 1];
		}
		else {
			int shellSize = _ms3_calc_shell_size(shell);
			int quadrant;
			int quadEnd = _ms3_get_end_quad_for_loc(loc, &quadrant);

			// linear dist to equiv point in onner shell is (1 + len(shell-1) + 2*quad)
			int deltaInner = 1 + _ms3_calc_shell_len(shell - 1) + (2 * quadrant);
			int ix = loc - deltaInner;

			// if we're not the first in a quadrant, add previous val from inner shell
			if ((loc != quadEnd - (shellSize - 2)) && (loc != shellMin+1))
				val += seq[(ix - 1) - 1];
			else
				// if we are, add the element that's 2 behind us as it's diagonally linked
				val += seq[(loc - 2) - 1];

			if (loc != quadEnd)
			{
				// also add neighbour
				val += seq[(ix)-1];

				// and then add next if we're not at penultimate
				if (loc + 1 != quadEnd)
					val += seq[(ix + 1) - 1];
			}

			// if we're the penultimate value in a shell, we also add the first val from the shell as it's diagonally connected
			if (loc + 1 == shellMax)
				val += seq[shellMin - 1];
		}

		seq.push_back(val);
	}

	return seq.back();
}

bool test_ms3()
{
	if (ms3_spiral_dist(1) != 0)
		return false;
	if (ms3_spiral_dist(12) != 3)
		return false;
	if (ms3_spiral_dist(23) != 2)
		return false;
	if (ms3_spiral_dist(1024) != 31)
		return false;

	if (ms3b_spiral_seq(2) != 4)
		return false;
	if (ms3b_spiral_seq(4) != 5)
		return false;
	if (ms3b_spiral_seq(8) != 10)
		return false;
	if (ms3b_spiral_seq(300) != 304)
		return false;
	if (ms3b_spiral_seq(351) != 362)
		return false;
	if (ms3b_spiral_seq(805) != 806)
		return false;
	
	return true;
}

// ------------------------------------------------

void readfile_lines(const string& filename, list<string>& outlines)
{
	outlines.clear();

	ifstream infile(filename);
	string line;
	while (getline(infile, line))
	{
		outlines.push_back(line);
	}
}

bool ms4_chkphrase(const string& phrase, bool anagramAllowed = true)
{
	list<string> words;

	size_t pix = 0;
	for (size_t ix = phrase.find(' '); ix != string::npos; ix = phrase.find(' ', pix))
	{
		string currWord = phrase.substr(pix, ix - pix);

		if (!anagramAllowed)
			sort(currWord.begin(), currWord.end());

		words.push_back(currWord);

		pix = ix + 1;
	}
	string lastWord = phrase.substr(pix);
	if (!anagramAllowed)
		sort(lastWord.begin(), lastWord.end());
	words.push_back(lastWord);

	words.sort();

	for (auto it = words.cbegin(); it != words.end(); ++it)
	{
		auto nit = it;
		++nit;
		if (nit != words.end())
		{
			if (*it == *nit)
				return false;
		}
	}

	return true;
}

int ms4_countvalid(const list<string>& phrases, bool anagramAllowed=true)
{
	int n = 0;
	for (const auto& phrase : phrases)
	{
		if (ms4_chkphrase(phrase, anagramAllowed))
			n++;
	}
	return n;
}

bool test_ms4()
{
	if (!ms4_chkphrase("aa bb cc dd ee"))
		return false;
	if ( ms4_chkphrase("aa bb cc dd aa"))
		return false;
	if (!ms4_chkphrase("aa bb cc dd aaa"))
		return false;

	if (!ms4_chkphrase("abcde fghij", false))
		return false;
	if ( ms4_chkphrase("abcde xyz ecdab", false))
		return false;
	if (!ms4_chkphrase("a ab abc abd abf abj", false))
		return false;
	if (!ms4_chkphrase("iiii oiii ooii oooi oooo", false))
		return false;
	if ( ms4_chkphrase("oiii ioii iioi iiio", false))
		return false;

	return true;
}

// -------------------------------------------------

void ms5_initmachine(const list<string>& program, vector<int32_t>& state)
{
	state.clear();
	state.reserve(program.size());
	for (auto instr : program)
		state.push_back(atoi(instr.c_str()));
}

int ms5_run(vector<int32_t>& machine, bool deluxemode=false)
{
	uint32_t runtime = 0;
	size_t pc = 0;
	while (pc < machine.size())
	{
		int dist = machine[pc];

		if (!deluxemode || dist < 3)
			++machine[pc];
		else
			--machine[pc];

		pc += (size_t)dist;
		runtime++;
	}

	return runtime;
}

int ms5_boot(const list<string>& program, bool deluxemode=false)
{
	vector<int32_t> machine;
	ms5_initmachine(program, machine);

	return ms5_run(machine, deluxemode);
}

bool test_ms5()
{
	list<string> program = { "0", "3", "0", "1", "-3" };
	vector<int32_t> machine;

	ms5_initmachine(program, machine);
	if (ms5_run(machine) != 5)
		return false;

	ms5_initmachine(program, machine);
	if (ms5_run(machine, true) != 10)
		return false;

	return true;
}

// -------------------------------------------------

struct ms6_state_less
{
	bool operator()(const vector<uint8_t>& a, const vector<uint8_t>& b) const
	{
		auto ait = a.begin();
		auto bit = b.begin();

		while (true)
		{
			if (ait == a.end())
			{
				if (bit != b.end())
					return true;
				else
					return false;
			}
			else if (bit == b.end())
				return false;

			if (*ait > *bit)
				return false;
			if (*ait < *bit)
				return true;

			++ait;
			++bit;
		}

		return true;
	}
};

struct ms6_info
{
	vector<uint8_t> state;
	int firstseen;

	ms6_info(const vector<uint8_t>& s, int fs)
	{
		state = s;
		firstseen = fs;
	}
};

struct ms6b_state_less {
	bool operator()(const ms6_info& a, const ms6_info& b) const
	{
		ms6_state_less less;
		return less(a.state, b.state);
	}
};

struct ms6b_state_eq_f
{
	const vector<uint8_t>& ref;

	ms6b_state_eq_f(const vector<uint8_t>& s)
		: ref(s)
	{
	}

	bool operator()(const ms6_info& b)
	{
		ms6_state_less lt;
		return !(lt(ref, b.state) || lt(b.state, ref));
	}
};

void ms6_rebalance_state(vector<uint8_t>& state)
{
	auto slot = max_element(state.begin(), state.end());
	int blocks = *slot;
	*slot = 0;

	while (blocks > 0)
	{
		++slot;
		if (slot == state.end())
			slot = state.begin();

		++(*slot);
		--blocks;
	}
}

int ms6_rebalance(vector<uint8_t>& state, bool loopLen=false)
{
	set<ms6_info, ms6b_state_less> history;

	int steps = 0;
	history.insert(ms6_info(state, steps));

	while (true)
	{
		ms6_rebalance_state(state);
		steps++;

		ms6b_state_eq_f pred(state);
		auto prev = find_if(history.begin(), history.end(), pred);
		if( prev != history.end())
		{
			if (!loopLen)
				return steps;
			else
				return steps - prev->firstseen;
		}

		history.insert(ms6_info(state, steps));
	}

	return -1;
}

int ms6_run(const string& input, bool loopLen=false)
{
	vector<uint8_t> state;

	// parse
	size_t pix = 0;
	for (size_t ix = input.find('\t'); ix != string::npos; ix = input.find('\t', pix))
	{
		string currWord = input.substr(pix, ix - pix);

		state.push_back((uint8_t)atoi(currWord.c_str()));

		pix = ix + 1;
	}
	string lastWord = input.substr(pix);
	state.push_back((uint8_t)atoi(lastWord.c_str()));
	
	// run
	return ms6_rebalance(state, loopLen);
}

bool test_ms6()
{
	if (ms6_rebalance(vector<uint8_t>{0, 2, 7, 0}) != 5)
		return false;

	if (ms6_rebalance(vector<uint8_t>{0, 2, 7, 0}, true) != 4)
		return false;

	return true;
}

// -------------------------------------------------

struct ms7_program
{
	string name;
	int weight;
	int towerweight;
	ms7_program* parent;
	list<string> childnames;
	vector<ms7_program*> children;

	bool operator<(const ms7_program& other) const {
		return towerweight < other.towerweight;
	}

	ms7_program() {}
	ms7_program(const string& state)
		: parent(nullptr)
	{
		auto ix = state.find(' ');
		name = state.substr(0, ix);
		ix++;

		_ASSERT(state[ix] == '(');
		ix++;
		auto pix = ix;
		ix = state.find(')', ix);
		weight = atoi(state.substr(pix, ix - pix).c_str());
		
		// there may be a list of children
		ix = state.find("-> ", ix);
		if ( ix == string::npos)
			return;

		ix += 3;
		while (true)
		{
			pix = ix;
			ix = state.find(", ", pix);
			if (ix == string::npos)
			{
				childnames.push_back(state.substr(pix));
				break;
			}

			childnames.push_back(state.substr(pix, ix - pix));

			ix += 2;
		}
	}
};

void ms7_weigh(ms7_program* root)
{
	int totalweight = root->weight;
	for (auto child : root->children)
	{
		ms7_weigh(&*child);
		totalweight += child->towerweight;
	}
	root->towerweight = totalweight;
}

bool ms7_found_wonk = false;
void ms7_balance(ms7_program* root)
{
	for (auto child : root->children)
	{
		ms7_balance(&*child);
	}

	if (ms7_found_wonk)
		return;

	if (root->children.size() < 3)
		return;

	sort(root->children.begin(), root->children.end());

	// a mismatch would be either first or last
	if (root->children.front()->towerweight == root->children.back()->towerweight)
		return;

	cout << "found wonk under " << root->name << endl;
	ms7_found_wonk = true;

	ms7_program* wonk;
	ms7_program* ref = root->children[1];
	if (root->children[0]->towerweight != ref->towerweight)
		wonk = root->children[0];
	else
		wonk = root->children.back();

	int error = wonk->towerweight - ref->towerweight;
	cout << wonk->name << " should weigh " << (wonk->weight - error) << endl;
}

string ms7_assemble(const list<string>& input)
{
	vector<ms7_program> programs;
	programs.reserve(input.size());

	for (auto line : input)
	{
		ms7_program p(line);
		programs.push_back(p);
	}

	map<string, ms7_program*> byname;
	for( auto it = programs.begin(); it != programs.end(); ++it)
		byname.insert(make_pair(it->name, &*it));

	for (auto it = programs.begin(); it != programs.end(); ++it)
	{
		for (const auto& childname : it->childnames)
		{
			auto child = byname.at(childname);
			it->children.push_back(child);
			child->parent = &*it;
		}
	}

	ms7_program* root = nullptr;
	for (auto it = programs.begin(); it != programs.end(); ++it)
	{
		if (it->parent == nullptr)
		{
			root = &*it;
			break;
		}
	}

	ms7_weigh(root);
	ms7_found_wonk = false;
	ms7_balance(root);

	return root->name;
}

bool test_ms7()
{
	list<string> input;
	readfile_lines("day7_test.txt", input);

	if (ms7_assemble(input) != "tknk")
		return false;

	return true;
}

// _,.~^~.,_,.~^~.,_,.~^~.,_,.~^~.,_,.~^~.,_,.~^~.,_,.~^~.,

struct ms8_instr
{
	string rt;
	string comp;
	int vt;
	string ro;
	int io;


	ms8_instr(const string& input)
	{
		istringstream is(input);
		string mb_if, op;
		is >> ro >> op >> io >> mb_if >> rt >> comp >> vt;

		if (op == "dec")
			io = -io;
	}

	map<string,int>::iterator find_reg(const string& r, map<string, int>& regs)
	{
		auto it = regs.find(r);
		if (it == regs.end())
			it = regs.insert(make_pair(r, 0)).first;
		return it;
	}

	bool comp_true(map<string, int>& regs)
	{
		auto test_it = find_reg(rt, regs);
		int curr = test_it->second;

		if (comp == "<")
			return curr < vt;
		if (comp == "<=")
			return curr <= vt;
		if (comp == "==")
			return curr == vt;
		if (comp == "!=")
			return curr != vt;
		if (comp == ">=")
			return curr >= vt;
		if (comp == ">")
			return curr > vt;

		_ASSERT(false);
		return false;
	}

	int ex(map<string, int>& regs)
	{
		if (!comp_true(regs))
			return -1000000;

		auto out_it = find_reg(ro, regs);
		out_it->second += io;

		return out_it->second;
	}
};

int ms8_run(const list<string>& input)
{
	map<string, int> regs;
	int max_intermediate = 0;
	for (auto line : input)
	{
		ms8_instr instr(line);
		int newval = instr.ex(regs);

		if (newval > max_intermediate)
			max_intermediate = newval;
	}

	cout << "---max intermediate=" << max_intermediate << endl;

	int max_val = -1000000;
	for (auto reg : regs)
	{
		if (reg.second > max_val)
			max_val = reg.second;
	}

	return max_val;
}

bool test_ms8()
{
	list<string> input;
	readfile_lines("day8_test.txt", input);

	if (ms8_run(input) != 1)
		return false;

	return true;
}


// __'^'_____'^'_____'^'_____'^'_____'^'_____'^'_____'^'___

int ms9_total_garbage_eaten = 0;
int ms9_eat_garbage(istringstream& s)
{
	int eaten = 0;
	char c = s.get();
	while (c != '>')
	{
		if (c == '!')
		{
			s.get();
			eaten--;
		}

		c = s.get();
		eaten++;
	}
	ms9_total_garbage_eaten += eaten;
	return eaten;
}

int ms9_count_groups(istringstream& s, int depth=1)
{
	int ngroups = depth;

	// eat {
	char c = s.get();
	_ASSERT(c == '{');

groupstart:
	c = s.get();
	if (c == '{')
	{
		s.putback(c);
		ngroups += ms9_count_groups(s, depth+1);
		goto groupstart;
	}
	else if (c == '<')
	{
		ms9_eat_garbage(s);
		goto groupstart;
	}
	else if (c == ',')
		goto groupstart;
	else
	{
		_ASSERT(c == '}');
	}

	return ngroups;
}

int test_ms9_garbage(const string& str)
{
	istringstream s(str);
	char c = s.get();
	_ASSERT(c == '<');
	int eaten = ms9_eat_garbage(s);
	s.get();
	_ASSERT(s.eof());
	return eaten;
}

int test_ms9_groups(const string& str)
{
	istringstream s(str);
	int ngroups = ms9_count_groups(s);
	s.get();
	_ASSERT(s.eof());
	return ngroups;
}

bool test_ms9()
{
	if (test_ms9_garbage("<>") != 0)
		return false;
	if (test_ms9_garbage("<random characters>") != 17)
		return false;
	if (test_ms9_garbage("<<<<>") != 3)
		return false;
	if (test_ms9_garbage("<{!>}>") != 2)
		return false;
	if (test_ms9_garbage("<!!>") != 0)
		return false;
	if (test_ms9_garbage("<!!!>>") != 0)
		return false;
	if (test_ms9_garbage("<{o\"i!a,<{i<a>") != 10)
		return false;

#if 0
	if (test_ms9_groups("{}") != 1)
		return false;
	if (test_ms9_groups("{{{}}}") != 3)
		return false;
	if (test_ms9_groups("{{},{}}") != 3)
		return false;
	if (test_ms9_groups("{{{},{},{{}}}}") != 6)
		return false;
	if (test_ms9_groups("{<{},{},{{}}>}") != 1)
		return false;
	if (test_ms9_groups("{<a>,<a>,<a>,<a>}") != 1)
		return false;
	if (test_ms9_groups("{{<a>},{<a>},{<a>},{<a>}}") != 5)
		return false;
	if (test_ms9_groups("{{<!>},{<!>},{<!>},{<a>}}") != 2)
		return false;
#endif

	if (test_ms9_groups("{}") != 1)
		return false;
	if (test_ms9_groups("{{{}}}") != 6)
		return false;
	if (test_ms9_groups("{{},{}}") != 5)
		return false;
	if (test_ms9_groups("{{{},{},{{}}}}") != 16)
		return false;
	if (test_ms9_groups("{<a>,<a>,<a>,<a>}") != 1)
		return false;
	if (test_ms9_groups("{{<ab>},{<ab>},{<ab>},{<ab>}}") != 9)
		return false;
	if (test_ms9_groups("{{<!!>},{<!!>},{<!!>},{<!!>}}") != 9)
		return false;
	if (test_ms9_groups("{{<a!>},{<a!>},{<a!>},{<ab>}}") != 3)
		return false;

	return true;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

typedef vector<uint8_t> ms10_t;

void ms10_init(ms10_t& v, int n)
{
	v.clear();
	v.reserve(n+1);
	for (int i = 0; i <= n; i++)
		v.push_back((uint8_t)i);
}

void ms10_splits_from_text(const string& text, ms10_t& splits)
{
	splits.clear();
	for (auto c : text)
		splits.push_back((uint8_t)c);
	splits.insert(splits.end(), { 17, 31, 73, 47, 23 });
}

void ms10_tighten_hash(const ms10_t& v, ms10_t& hash)
{
	hash.clear();
	hash.reserve(16);

	auto it = v.begin();
	for (int i = 0; i < 16; i++)
	{
		uint8_t h = 0;
		for (int j = 0; j < 16; j++)
		{
			h ^= *it;
			++it;
		}
		hash.push_back(h);
	}
}

const char ms10_hex[] = { '0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f' };
string ms10_hash_to_string(const ms10_t& hash)
{
	ostringstream s;
	for (auto h : hash)
	{
		s << (ms10_hex[h >> 4]) << ms10_hex[h & 15];
	}
	return s.str();
}

void ms10_split_hash(ms10_t& v, const ms10_t& splits, int rounds=1)
{
	ms10_t n;
	n.reserve(v.size());

	int skip = 0;
	int offset = 0;

	for (int round = 0; round < rounds; round++)
	{
		for (auto split : splits)
		{
			reverse(v.begin(), v.begin() + split);

			int move = (split + skip) % v.size();
			n.insert(n.end(), v.begin() + move, v.end());
			n.insert(n.end(), v.begin(), v.begin() + move);
			offset += move;

			v.swap(n);
			n.clear();
			skip++;
		}
	}

	offset %= v.size();
	int shunt = v.size() - offset;
	n.insert(n.end(), v.begin() + shunt, v.end());
	n.insert(n.end(), v.begin(), v.begin() + shunt);
	v.swap(n);
}

int ms10_do_the_splits(ms10_t& v, const ms10_t& splits)
{
	ms10_split_hash(v, splits);
	return v[0] * v[1];
}

string ms10_hash(const string& in)
{
	ms10_t splits;
	ms10_splits_from_text(in, splits);

	ms10_t sparse;
	ms10_init(sparse, 255);

	ms10_split_hash(sparse, splits, 64);

	ms10_t dense;
	ms10_tighten_hash(sparse, dense);

	return ms10_hash_to_string(dense);
}

bool test_ms10()
{
	ms10_t v;
	ms10_init(v,4);
	if (ms10_do_the_splits(v, ms10_t({ 3 })) != 2)
		return false;
	ms10_init(v, 4);
	if (ms10_do_the_splits(v, ms10_t({ 3,4 })) != 12)
		return false;
	ms10_init(v, 4);
	if (ms10_do_the_splits(v, ms10_t({ 3,4,1 })) != 12)
		return false;
	ms10_init(v, 4);
	if (ms10_do_the_splits(v, ms10_t({ 3,4,1,5 })) != 12)
		return false;

	if (ms10_hash("") != "a2582a3a0e66e6e86e3812dcb672a272")
		return false;
	if (ms10_hash("AoC 2017") != "33efeb34ea91902bb2f59c9920caa6cd")
		return false;
	if (ms10_hash("1,2,3") != "3efbe78a8d82f29979031a4aa0b16a9d")
		return false;
	if (ms10_hash("1,2,4") != "63960835bcdc130f0b66d7ff4f6a5a8e")
		return false;

	return true;
}

// ^ v ^ v ^ v ^ v ^ v ^ v ^ v ^ v ^ v ^ v ^ v ^ v ^ v ^ v ^ v

struct v2h
{
	int x, y;

	void set(int _x, int _y)
	{
		x = _x, y = _y;
	}
	v2h() {}
	v2h(int _x, int _y) :x(_x), y(_y) {}

	bool operator==(const v2h& other)
	{
		return x == other.x && y == other.y;
	}

	void move(const string& dir)
	{
		if (dir == "n")
		{
			x += 1;
			y -= 1;
		}
		else if (dir == "s")
		{
			x -= 1;
			y += 1;
		}
		else if (dir == "ne")
			x += 1;
		else if (dir == "se")
			y += 1;
		else if (dir == "nw")
			y -= 1;
		else if (dir == "sw")
			x -= 1;
	}

	int multimove(const string& moves)
	{
		int max_dist = 0;
		size_t pix = 0;
		for (size_t ix = moves.find(','); ; ix = moves.find(',', pix))
		{
			string dir = moves.substr(pix, ix - pix);
			move(dir);

			int cdist = dist();
			max_dist = max(max_dist, cdist);

			if (ix == string::npos)
				break;

			pix = ix + 1;
		}

		return max_dist;
	}

	int dist() const
	{
		// d = max(|x|,|y|,|z|) where z = -x - y
		int z = -x - y;
		return max(max(abs(x), abs(y)), abs(z));
	}
};

int ms11_hex(const string& moves)
{
	v2h v(0, 0);
	v.multimove(moves);
	return v.dist();
}

int ms11_furthest(const string& moves)
{
	v2h v(0, 0);
	return v.multimove(moves);
}

bool test_m11_move(const string& moves, const v2h& dest)
{
	v2h v(0, 0);
	v.multimove(moves);
	return v == dest;
}

bool test_ms11()
{
	if (!test_m11_move("ne,ne,ne,ne", v2h(4, 0)))
		return false;
	if (!test_m11_move("se,n,se,s,sw,nw,n", v2h(0, 0)))
		return false;
	if (!test_m11_move("ne,se,ne,se,ne,se,ne,se", v2h(4,4)))
		return false;

	if (v2h(1, 1).dist() != 2)
		return false;
	if (v2h(-1,0).dist() != 1)
		return false;
	if (v2h(0,-1).dist() != 1)
		return false;
	if (v2h(1, 2).dist() != 3)
		return false;
	if (v2h(2, -1).dist() != 2)
		return false;
	if (v2h(1, -1).dist() != 1)
		return false;
	if (v2h(2,2).dist() != 4)
		return false;
	if (v2h(-2,2).dist() != 2)
		return false;
	if (v2h(-2,-2).dist() != 4)
		return false;

	return true;
}

// ____"'~;~'"____"'~;~'"____"'~;~'"____"'~;~'"____"'~;~'"____

void ms12_read_pipes(const list<string>& input, vector<vector<int>>& pipes)
{
	pipes.clear();
	pipes.reserve(input.size());

	for (auto line : input)
	{
		istringstream is(line);
		int pno;
		is >> pno;
		_ASSERT(pno == pipes.size());
		vector<int> dests;

		string tok;
		while (!is.eof())
		{
			char c = is.get();
			if (c == ' ' || c == '<' || c == '-' || c == '>')
				continue;

			if (c == ',' || is.eof())
			{
				dests.push_back(atoi(tok.c_str()));
				tok.clear();
			}
			else
				tok += c;
		}

		pipes.push_back(dests);
	}
}

int ms12_count_group(int start, const vector<vector<int>>& pipes)
{
	vector<bool> visited;
	visited.resize(pipes.size(), false);

	vector<int> to_visit;
	to_visit.push_back(start);
	int groupsize = 0;
	while (!to_visit.empty())
	{
		int p = to_visit.back();
		to_visit.pop_back();

		if (visited[p])
			continue;

		visited[p] = true;
		groupsize++;
		for (auto next : pipes[p])
		{
			if (!visited[next])
				to_visit.push_back(next);
		}
	}

	return groupsize;
}

int ms12_pipes(const list<string>& input)
{
	vector<vector<int>> pipes;
	ms12_read_pipes(input, pipes);

	return ms12_count_group(0, pipes);
}

int ms12_count_piping_groups(const list<string>& input)
{
	vector<vector<int>> pipes;
	ms12_read_pipes(input, pipes);

	vector<int> groups;
	groups.resize(pipes.size(), -1);

	int currgroup = 0;

	for (size_t p = 0; p < pipes.size(); ++p)
	{
		if (groups[p] >= 0)
			continue;

		vector<int> to_visit;
		to_visit.push_back(p);
		while (!to_visit.empty())
		{
			int p = to_visit.back();
			to_visit.pop_back();

			if (groups[p] >= 0)
				continue;

			groups[p] = currgroup;
			for (auto next : pipes[p])
			{
				if (groups[next] < 0)
					to_visit.push_back(next);
			}
		}

		currgroup++;
	}

	return currgroup;
}

bool test_ms12()
{
	list<string> input;
	readfile_lines("day12_test.txt", input);

	vector<vector<int>> pipes;
	ms12_read_pipes(input, pipes);

	if (ms12_count_group(0, pipes) != 6)
		return false;

	readfile_lines("day12_test2.txt", input);
	if (ms12_count_piping_groups(input) != 3)
		return false;

	return true;
}

// ---<{O}>----<{O}>----<{O}>----<{O}>----<{O}>----<{O}>----<{O}>---

struct scanner
{
	int8_t range;
	int8_t loop;
	int8_t pos;

	scanner() {};
	scanner(int r, int p)	:  pos(p) 
	{
		setrange(r);
	}

	void setrange(int r)
	{
		range = r;

		if (range >= 2)
			loop = (range - 1) * 2;
		else
			loop = range;
	}
};

void ms13_init_firewall(vector<scanner>& firewall, const list<string>& input)
{
	int numlayers;
	{
		istringstream is(input.back());
		is >> numlayers;
	}

	firewall.clear();
	firewall.resize(numlayers+1, scanner(0, -1));

	string dummy;
	for (auto line : input)
	{
		istringstream is(line);
		int layer, depth;
		is >> skipws >> layer >> dummy >> depth;

		firewall[layer].setrange(depth);
		firewall[layer].pos = 0;
	}
}

void ms13_dump_firewall(const vector<scanner>& firewall, int tick, int packetlayer)
{
	cout << "Picosecond " << tick << ":" << endl;
	for(auto l = 0u; l<firewall.size(); ++l)
		cout << right << setw(2) << l << "  ";
	cout << endl;

	int maxrange = max_element(firewall.begin(), firewall.end(), [](const scanner& a, const scanner& b) { return a.range < b.range; })->range;
	for (int r = 0; r < maxrange; r++)
	{
		for (auto l = 0u; l < firewall.size(); ++l)
		{
			const scanner& s = firewall[l];
			bool scanhere = (s.pos == r);
			bool packhere = (r==0) && (packetlayer == l);

			if (s.pos >= 0)
			{
				if (s.range > r)
					cout << (packhere ? '(' : '[') << (scanhere ? 'S' : ' ') << (packhere ? ')' : ']') << ' ';
				else
					cout << "    ";
			}
			else if (r==0)
				cout << (packhere ? '(' : '.') << '.' << (packhere ? ')' : '.') << ' ';
			else
				cout << "    ";
		}

		cout << endl;
	}

	cout << endl << endl;
}

void ms13_tick_firewall(vector<scanner>& firewall)
{
	for (auto it = firewall.begin(); it != firewall.end(); ++it)
	{
		if (it->range == 0)
			continue;

		it->pos++;
		if (it->pos == it->loop)
			it->pos = 0;
	}
}

void ms13_skip_to_the_start(vector<scanner>& firewall, int64_t delay)
{
	for (auto& scanner : firewall)
	{
		if (scanner.range == 0)
			continue;

		scanner.pos = delay % (int64_t)scanner.loop;
	}
}

int ms13_breakin(vector<scanner>& firewall, bool* pcaught = NULL, int64_t delay=0)
{
	int severity = 0;
	int layer = -1;
	int tick = 0;

	if (pcaught) *pcaught = false;

	ms13_skip_to_the_start(firewall, delay);

	while (layer < (int)firewall.size()-1)
	{
		layer++;

		// were we spotted?
		if (firewall[layer].pos == 0)
		{
			int s = layer * firewall[layer].range;
			severity += s;
			if (pcaught)
			{
				*pcaught = true;
				return severity;
			}
		}

		ms13_tick_firewall(firewall);

		//if (delay == 4)
			//ms13_dump_firewall(firewall, tick, layer);

		tick++;
	}

	return severity;
}

int ms13_go_go_go(const list<string>& input)
{
	vector<scanner> firewall;
	ms13_init_firewall(firewall, input);
	return ms13_breakin(firewall);
}

void ms13_thready(const list<string>& input, int64_t delaystart, int step, bool& done, int64_t& outdelay)
{
	int64_t delay = delaystart;
	while (!done)
	{
		vector<scanner> firewall;
		ms13_init_firewall(firewall, input);
		bool caught;
		ms13_breakin(firewall, &caught, delay);
		if (!caught)
		{
			cout << "FOUND A SAFE DELAY! " << delay << "ps" << endl;
			outdelay = delay;
			done = true;
			return;
		}

		delay += step;
		if (delay % 50000 == 0)
			cout << "attempting delay of " << delay << "ps..." << endl;
	}
}

int64_t ms13_not_too_steady(const list<string>& input)
{
	bool done = false;
	int64_t delays[4] = { 0x7fffffffffffffffll,0x7fffffffffffffffll,0x7fffffffffffffffll,0x7fffffffffffffffll };
	int64_t startdelay = 280000;
	thread w0(&ms13_thready, input, startdelay + 0, 4, ref(done), ref(delays[0]));
	thread w1(&ms13_thready, input, startdelay + 1, 4, ref(done), ref(delays[1]));
	thread w2(&ms13_thready, input, startdelay + 2, 4, ref(done), ref(delays[2]));
	thread w3(&ms13_thready, input, startdelay + 3, 4, ref(done), ref(delays[3]));
	w0.join();
	w1.join();
	w2.join();
	w3.join();
	return *min_element(delays, delays + 4);
}

int ms13_steady_dot_dot_dot(const list<string>& input)
{
	int delay = 0;
	while (true)
	{
		vector<scanner> firewall;
		ms13_init_firewall(firewall, input);
		bool caught;
		ms13_breakin(firewall, &caught, delay);
		if (!caught)
			return delay;

		delay++;
		if (delay % 1000 == 0)
			cout << "attempting delay of " << delay << "ps..." << endl;
	}

	return -1;
}

bool test_ms13()
{
	list<string> input;
	readfile_lines("day13_test.txt", input);

	vector<scanner> firewall;
	ms13_init_firewall(firewall, input);
	if (ms13_breakin(firewall) != 24)
		return false;

	if (ms13_steady_dot_dot_dot(input) != 10)
		return false;

	return true;
}

// ,...,i.,.i`.,.;,,.,.'i...,.,.,,.;,..;,.;.;:.,,:`;,..,.,;..,.;;,.;:.

int ms14_hex2dec(char c)
{
	int n;
	if (c >= '0' && c <= '9')
		n = c - '0';
	else if (c >= 'a' && c <= 'f')
		n = 10 + (c - 'a');
	else if (c >= 'A' && c <= 'F')
		n = 10 + (c - 'A');
	else
	{
		_ASSERT(false);
	}

	return n;
}

int ms14_count_bits(const string& hex)
{
	int bits = 0;
	for (char c : hex)
	{
		int n = ms14_hex2dec(c);

		for (int b = 8; b; b >>= 1)
		{
			if (n & b)
				bits++;
		}
	}
	return bits;
}

string ms14_rowhash(const string& input, int row)
{
	ostringstream s;
	s << input << '-' << row;
	return ms10_hash(s.str());
}

int ms14_count_used(const string& input)
{
	int used = 0;
	for (int i = 0; i < 128; i++)
	{
		used += ms14_count_bits(ms14_rowhash(input, i));
	}
	return used;
}

void ms14_init_disk_row(int16_t* pdiskrow, const string& rowhash)
{
	int16_t* pcurr = pdiskrow;

	for (char c : rowhash)
	{
		int n = ms14_hex2dec(c);

		for (int b = 8; b; b >>= 1)
		{
			*pcurr = (n&b) ? -1 : 0;
			pcurr++;
		}
	}
}

struct v2u8
{
	uint8_t r, c;

	v2u8() {}
	v2u8(int row, int col) : r(row), c(col) {}
};

int ms14_count_regions(const string& input)
{
	int16_t disk[128][128];

	for (int row = 0; row < 128; row++)
		ms14_init_disk_row(disk[row], ms14_rowhash(input, row));

	int cregion = 1;
	vector<v2u8> to_visit;
	for (int row = 0; row < 128; row++)
	{
		for (int col = 0; col < 128; col++)
		{
			// if there's nothing here or we've already grouped it, move on
			if (disk[row][col] >= 0)
				continue;

			to_visit.push_back(v2u8(row, col));
			while (!to_visit.empty())
			{
				v2u8 c = to_visit.back();
				to_visit.pop_back();

				if (disk[c.r][c.c] > 0)
					continue;

				disk[c.r][c.c] = (int16_t)cregion;

				// add any un-added neighbours
				if (c.r < 127 && disk[c.r + 1][c.c] < 0)
					to_visit.push_back(v2u8(c.r + 1, c.c));
				if (c.c < 127 && disk[c.r][c.c + 1] < 0)
					to_visit.push_back(v2u8(c.r, c.c + 1));
				if (c.r > 0 && disk[c.r - 1][c.c] < 0)
					to_visit.push_back(v2u8(c.r - 1, c.c));
				if (c.c > 0 && disk[c.r][c.c - 1] < 0)
					to_visit.push_back(v2u8(c.r, c.c - 1));
			}

			cregion++;
		}
	}

	return cregion - 1;
}

bool test_ms14()
{
	if (ms14_count_used("flqrgnkx") != 8108)
		return false;

	if (ms14_count_regions("flqrgnkx") != 1242)
		return false;

	return true;
}

// ##.#..#.#.#.##....##.#.###.#.#.#.#.#.#....##.###.#..#.##.#...###..##.

class ms15_gen
{
	uint64_t curr;
	uint32_t factor;

public:
	ms15_gen(uint32_t start, uint32_t fact) : curr(start), factor(fact) {}

	uint16_t next()
	{
		curr *= factor;
		curr %= 2147483647;
		return (uint16_t)curr;
	}

	uint16_t next_mult(uint16_t mask)
	{
		uint16_t val;
		do
		{
			val = next();
		} while (val&mask);

		return val;
	}
};


void print_binary(uint16_t n)
{
	for (uint16_t m = 0x8000; m; m >>= 1)
	{
		cout << (n&m ? '1' : '0');
	}
}

int ms15_judge(int num, uint32_t init_a, uint32_t init_b)
{
	ms15_gen ga(init_a, 16807);
	ms15_gen gb(init_b, 48271);

	int matches = 0;
	for (int i = 0; i < num; i++)
	{
		uint16_t ra = ga.next();
		uint16_t rb = gb.next();

		if (ra == rb)
			matches++;
	}

	return matches;
}

int ms15_judger(int num, uint32_t init_a, uint32_t init_b)
{
	ms15_gen ga(init_a, 16807);
	ms15_gen gb(init_b, 48271);

	int matches = 0;
	for (int i = 0; i < num; i++)
	{
		uint16_t ra = ga.next_mult(3);
		uint16_t rb = gb.next_mult(7);

		if (ra == rb)
			matches++;
	}

	return matches;
}

bool test_ms15()
{
	if (ms15_judge(5, 65, 8921) != 1)
		return false;
	if (ms15_judge(40 * 1000 * 1000, 65, 8921) != 588)
		return false;

	if (ms15_judger(5 * 1000 * 1000, 65, 8921) != 309)
		return false;

	return true;
}

// 010100011010111110100101001010100101101011011010001010111101010101010

string ms16_init(char lastchar)
{
	string programs;
	for (char c = 'a'; c <= lastchar; c++)
		programs.push_back(c);
	return programs;
}

class ms16_instr
{
public:
	virtual void move(string& progs) = 0;
};

typedef vector<ms16_instr*> ms16_program;


class ms16_spin : public ms16_instr
{
	int32_t dist;

public:

	ms16_spin() {}
	ms16_spin(const string& args)
	{
		dist = atoi(args.c_str());
	}

	virtual void move(string& progs) override
	{
		rotate(progs.begin(), progs.end() - dist, progs.end());
		//string np = progs.substr(progs.size() - dist) + progs.substr(0, progs.size() - dist);
		//progs.swap(np);
	}
};

class ms16_exchange : public ms16_instr
{
	int16_t from, to;

public:

	ms16_exchange() {}
	ms16_exchange(const string& args)
	{
		istringstream is(args);
		char dummy;
		is >> from >> dummy >> to;
	}

	virtual void move(string& progs) override
	{
		swap(*(progs.begin() + from), *(progs.begin() + to));
	}
};

class ms16_partner : public ms16_instr
{
	char a, b;

public:

	ms16_partner() {}
	ms16_partner(const string& args)
	{
		a = args[0];
		b = args[2];
	}

	virtual void move(string& progs) override
	{
		auto ita = find(progs.begin(), progs.end(), a);
		auto itb = find(progs.begin(), progs.end(), b);
		swap(*ita, *itb);
	}
};



void ms16_compile(const string& instructions, ms16_program& compiled)
{
	compiled.clear();

	size_t pix = 0;
	for (size_t ix = instructions.find(','); ; ix = instructions.find(',', pix))
	{
		string instr = instructions.substr(pix, ix - pix);
		char cmd = instr[0];
		string args = instr.substr(1);

		if (cmd == 's')
			compiled.push_back(new ms16_spin(args));
		else if (cmd == 'x')
			compiled.push_back(new ms16_exchange(args));
		else if(cmd == 'p')
			compiled.push_back(new ms16_partner(args));

		if (ix == string::npos)
			break;
		pix = ix + 1;
	}
}

string ms16_dance(char lastprog, const string& instructions, int rounds = 1)
{
	string progs = ms16_init(lastprog);

	ms16_program dance;
	ms16_compile(instructions, dance);

	vector<string> history;
	history.reserve(100000);

	bool skipped = false;
	for (int round = 1; round <= rounds; round++)
	{
		for (auto instr : dance)
			instr->move(progs);

		if (!skipped)
		{
			auto found = find(history.begin(), history.end(), progs);
			if (found != history.end())
			{
				cout << "loop found after " << (history.size() - distance(history.begin(), found)) << endl;
				size_t loop = history.size();
				// this maths is bobbins but whatevs.
				round = (rounds / loop) * loop;
				skipped = true;
			}

			history.push_back(progs);
		}

		if (round % 10000 == 0)
			cout << "round " << round << endl;
	}

	for (auto instr : dance)
		delete instr;
	dance.clear();

	return progs;
}

bool test_ms16()
{
	if (ms16_dance('e', "s1,x3/4,pe/b") != "baedc")
		return false;
	if (ms16_dance('e', "s1,x3/4,pe/b", 2) != "ceadb")
		return false;

	return true;
}

// |..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|

int ms17_spinlock(size_t advance)
{
	vector<int16_t> circle;
	circle.reserve(2048);

	circle.push_back(0);
	size_t pos = 0;
	for (int16_t i = 1; i < 2018; i++)
	{
		pos = (pos + advance) % circle.size();
		pos++;
		circle.insert(circle.begin() + pos, i);
	}

	auto next = (pos + 1) % circle.size();

	return circle[next];
}

int ms17_afterzero(size_t advance, size_t loops)
{
	int afterzero = 0;
	size_t pos = 0;
	size_t size = 1;
	for (size_t i = 1; i <= loops; i++)
	{
		pos = (pos + advance) % size;
		pos++;
		if (pos == 1)
			afterzero = i;
		size++;
	}

	return afterzero;
}

bool test_ms17()
{
	if (ms17_spinlock(3) != 638)
		return false;

	if (ms17_afterzero(3, 1) != 1)
		return false;
	if (ms17_afterzero(3, 2) != 2)
		return false;
	if (ms17_afterzero(3, 3) != 2)
		return false;
	if (ms17_afterzero(3, 9) != 9)
		return false;

	return true;
}

// %------------------------------------------------------------------al %


class ms18_proc
{
	map<char, int64_t> regs;
	deque<int64_t> msgs;
	int num_sends;

	ms18_proc* pother;

	const vector<string>& prog;
	size_t pc;

	bool waiting_message;


	map<char, int64_t>::iterator getreg(char r)
	{
		auto regit = regs.find(r);
		if (regit == regs.end())
			regit = regs.insert(make_pair(r, 0)).first;

		return regit;
	}

	int64_t getval(const string& arg)
	{
		if (isdigit(arg[0]) || arg[0] == '-')
			return atoi(arg.c_str());

		_ASSERT(arg.length() == 1);
		auto regit = getreg(arg[0]);
		return regit->second;
	}

	void send(int64_t val)
	{
		num_sends++;

		if (pother)
			pother->msgs.push_back(val);
		else
			msgs.push_back(val);
	}

	bool receive(int64_t& val)
	{
		if (!msgs.empty())
		{
			val = msgs.front();
			msgs.pop_front();
			waiting_message = false;
			return true;
		}

		waiting_message = true;
		return false;
	}

public:
	ms18_proc(int64_t pid, const vector<string>& program) : num_sends(0), pother(nullptr), prog(program), pc(0), waiting_message(false)
	{
		getreg('p')->second = pid;
	}

	void set_partner(ms18_proc* other)
	{
		pother = other;
	}

	bool is_blocked() const
	{
		return waiting_message;
	}

	bool can_run() const
	{
		if (is_blocked())
			return false;

		if (pc >= prog.size())
			return false;

		return true;
	}

	int get_num_sends() const
	{
		return num_sends;
	}

	bool tick()
	{
		if (pc >= prog.size())
			return false;

		istringstream is(prog[pc]);
		string instr, arg0, arg1;
		is >> instr >> arg0;
		if (!is.eof())
			is >> arg1;

		if (instr == "snd")
		{
			send(getval(arg0));
		}
		else if (instr == "set")
		{
			int64_t val = getval(arg1);
			auto regit = getreg(arg0[0]);
			regit->second = val;
		}
		else if (instr == "add")
		{
			int64_t val = getval(arg1);
			auto regit = getreg(arg0[0]);
			regit->second += val;
		}
		else if (instr == "mul")
		{
			int64_t val = getval(arg1);
			auto regit = getreg(arg0[0]);
			regit->second *= val;
		}
		else if (instr == "mod")
		{
			int64_t val = getval(arg1);
			auto regit = getreg(arg0[0]);
			regit->second %= val;
		}
		else if (instr == "rcv")
		{
			int64_t val;
			if (!receive(val))
				return false;

			auto regit = getreg(arg0[0]);
			regit->second = val;
		}
		else if (instr == "jgz")
		{
			int64_t test = getval(arg0);
			int64_t jmp = getval(arg1);
			if (test > 0)
			{
				pc += (int)jmp;
				return true;
			}
		}

		pc++;
		return true;
	}
};

int64_t ms18_duet(const list<string>& input)
{
	vector<string> prog;
	prog.reserve(input.size());
	for (auto line : input)
		prog.push_back(line);

	ms18_proc proc0(0, prog);
	ms18_proc proc1(1, prog);

	proc0.set_partner(&proc1);
	proc1.set_partner(&proc0);

	while (proc0.can_run() || proc1.can_run())
	{
		proc0.tick();
		proc1.tick();
	}

	return proc1.get_num_sends();
}

bool test_ms18()
{
	list<string> input;
	readfile_lines("day18_test.txt", input);

	if (ms18_duet(input) != 3)
		return false;

	return true;
}

// *|_*|  ..... d d  |O| ... *|_*|  ..... d d  |O| ... *|_*|  ..... d d  |O|

struct v2i
{
	int32_t x, y;

	v2i() {}
	v2i(int32_t _x, int32_t _y) : x(_x), y(_y) {}

	v2i& operator+= (const v2i& other)
	{
		x += other.x;
		y += other.y;
		return *this;
	}

	v2i operator-() const
	{
		return v2i(-x, -y);
	}

	v2i operator+(const v2i& other) const
	{
		return v2i(x + other.x, y + other.y);
	}

	v2i operator-(const v2i& other) const
	{
		return v2i(x - other.x, y - other.y);
	}

	v2i& transpose()
	{
		swap(x, y);
		return *this;
	}

	v2i& right()
	{
		int ox = x;
		x = -y;
		y = ox;
		return *this;
	}

	v2i& left()
	{
		int ox = x;
		x = y;
		y = -ox;
		return *this;
	}
};

class ms19_map
{
	size_t width, height;
	char* buf;

	v2i start;

public:
	ms19_map(const list<string>& input)
	{
		width = input.front().size();
		height = input.size();
		buf = new char[width*height];

		char* prow = buf;
		for (auto lineit = input.begin(); lineit != input.end(); ++lineit, prow+=width)
			copy(lineit->begin(), lineit->end(), prow);

		// start pos is the first (only) '|' on row 0
		start = v2i(~0u, ~0u);
		for (size_t x = 0; x < width; ++x)
		{
			if (buf[x] == '|')
			{
				start = v2i(x, 0);
				break;
			}
		}
	}

	ms19_map()
	{
		delete[] buf;
	}

	inline char get(const v2i& v) const
	{
		_ASSERT((size_t)v.x < width);
		_ASSERT((size_t)v.y < height);
		return buf[v.y*width + v.x];
	}

	inline bool is_inside(const v2i& v) const
	{
		return ((size_t)v.x < width) && ((size_t)v.y < height);
	}

	string traverse(int* pnsteps) const
	{
		v2i pos = start;
		v2i dir(0, 1);
		string journey;

		int nsteps = 0;

		while (true)
		{
			nsteps++;
			pos += dir;
			char curr = get(pos);

			if (curr == '-' || curr == '|')
			{
				continue;
			}
			else if (isalpha(curr))
			{
				journey += curr;
			}
			else if (curr == '+')
			{
				dir.transpose();

				v2i nxt = pos + dir;
				if (!is_inside(nxt) || get(nxt) == ' ')
					dir = -dir;
			}
			else
			{
				_ASSERT(curr == ' ');
				break;
			}
		}

		if (pnsteps)
			*pnsteps = nsteps;

		return journey;
	}
};

string ms19_pipes(const list<string>& input, int* pnsteps)
{
	ms19_map map(input);
	return map.traverse(pnsteps);
}

bool test_ms19()
{
	list<string> input;
	readfile_lines("day19_test.txt", input);

	int nsteps;
	if (ms19_pipes(input, &nsteps) != "ABCDEF")
		return false;
	if (nsteps != 38)
		return false;

	return true;
}

// ----|_______|-----`-^----+__________+---------+''''''''|__________+--------

struct v3i
{
	int x, y, z;

	v3i() {}
	v3i(int _x, int _y, int _z) : x(_x), y(_y), z(_z) {}

	v3i(const string& s)
	{
		auto pix = 0;
		auto ix = s.find(',');
		x = atoi(s.substr(pix, ix - pix).c_str());

		pix = ix + 1;
		ix = s.find(',', pix);
		y = atoi(s.substr(pix, ix - pix).c_str());

		pix = ix + 1;
		z = atoi(s.substr(pix).c_str());
	}

	int32_t mag_man() const
	{
		return abs(x) + abs(y) + abs(z);
	}

	bool operator<(const v3i& other) const
	{
		if (x < other.x)
			return true;
		if (x > other.x)
			return false;

		if (y < other.y)
			return true;
		if (y > other.y)
			return false;

		if (z < other.z)
			return true;

		return false;
	}

	v3i operator+(const v3i& other) const
	{
		return v3i(x + other.x, y + other.y, z + other.z);
	}

	v3i& operator+=(const v3i& other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}
};

struct ms20_part
{
	v3i	pos, vel, acc;

	ms20_part() {}
	ms20_part(const string& line)
	{
		auto pix = line.find('<');
		pix++;
		auto ix = line.find('>', pix);
		pos = v3i(line.substr(pix, ix - pix));

		pix = line.find('<', ix + 1);
		pix++;
		ix = line.find('>', pix);
		vel = v3i(line.substr(pix, ix - pix));

		pix = line.find('<', ix + 1);
		pix++;
		ix = line.find('>', pix);
		acc = v3i(line.substr(pix, ix - pix));
	}

	void update()
	{
		vel += acc;
		pos += vel;
	}
};

size_t ms20_slowest_particle(const list<string>& input, int iters)
{
	size_t slowest = ~0u;
	v3i min_pos(0x7fffffff, 0x7fffffff, 0x7fffffff);

	vector<ms20_part> parts;
	parts.reserve(input.size());

	for (auto& line : input)
		parts.emplace_back(line);

	map<v3i, size_t> bypos;
	vector<size_t> todelete;
	for (int i = 0; i < iters; i++)
	{
		for( size_t p = 0; p<parts.size(); p++)
		{
			ms20_part& part = parts[p];
			part.update();

			auto collision = bypos.find(part.pos);
			if (collision != bypos.end())
			{
				if (find(todelete.begin(), todelete.end(), collision->second) == todelete.end())
					todelete.push_back(collision->second);
				todelete.push_back(p);
			}
			else
				bypos.insert(make_pair(part.pos, p));
		}

		for (auto it = todelete.rbegin(); it != todelete.rend(); ++it)
			parts.erase(parts.begin() + *it);

		bypos.clear();
		todelete.clear();
	}

	//cout << "num particles left = " << parts.size() << endl;
	return parts.size();

	/*
	size_t pnum = 0;
	for (size_t pnum = 0; pnum < parts.size(); ++pnum)
	{
		const ms20_part& part = parts[pnum];

		if (part.pos.mag_man() < min_pos.mag_man())
		{
			slowest = pnum;
			min_pos = part.pos;
		}
	}*/

	return slowest;
}

bool test_ms20()
{
	list<string> input;
	readfile_lines("day20_test.txt", input);

	if (ms20_slowest_particle(input, 50) != 1)
		return false;

	return true;
}

// ,   . *  ;, '   `- :   { (o) } ; : "' ,  - ,,   . ; ' '    . *  `

class ms21_art
{
	typedef map<uint16_t, uint16_t> ruleset;
	ruleset rules2;
	ruleset rules3;

	int size;
	vector<bool> buf;


	uint16_t pack_buf2(int x, int y)
	{
		uint16_t k = 0;
		if (buf[(y*size) + x])
			k |= 1;
		if (buf[(y*size) + x + 1])
			k |= 2;
		if (buf[((y + 1)*size) + x])
			k |= 4;
		if (buf[((y + 1)*size) + x + 1])
			k |= 8;
		return k;
	}

	uint16_t pack_buf3(int x, int y)
	{
		uint16_t k = 0;
		if (buf[(y*size) + x])
			k |= 1;
		if (buf[(y*size) + x + 1])
			k |= 2;
		if (buf[(y*size) + x + 2])
			k |= 4;
		if (buf[((y + 1)*size) + x])
			k |= 8;
		if (buf[((y + 1)*size) + x + 1])
			k |= 0x10;
		if (buf[((y + 1)*size) + x + 2])
			k |= 0x20;
		if (buf[((y + 2)*size) + x])
			k |= 0x40;
		if (buf[((y + 2)*size) + x + 1])
			k |= 0x80;
		if (buf[((y + 2)*size) + x + 2])
			k |= 0x100;
		return k;
	}

	void unpack_buf3(uint32_t k, int x, int y, vector<bool>& b, int sz)
	{
		uint32_t mask = 1;
		for (int r = 0; r < 3; r++)
		{
			for (int c = 0; c < 3; c++)
			{
				b[(y + r)*sz + c + x] = ((k&mask) != 0);
				mask <<= 1;
			}
		}
	}
	void unpack_buf4(uint32_t k, int x, int y, vector<bool>& b, int sz)
	{
		uint32_t mask = 1;
		for (int r = 0; r < 4; r++)
		{
			for (int c = 0; c < 4; c++)
			{
				b[(y + r)*sz + c + x] = ((k&mask) != 0);
				mask <<= 1;
			}
		}
	}


	inline char hd(uint32_t bit)
	{
		return (bit ? '#' : '.');
	}

	void print2(uint16_t rule)
	{
		cout << hd(rule & 1) << hd(rule & 2) << endl << hd(rule & 4) << hd(rule & 8) << endl;
	}
	void print3(uint16_t rule)
	{
		cout << hd(rule & 0x01) << hd(rule & 0x02) << hd(rule & 0x04) << '\n';
		cout << hd(rule & 0x08) << hd(rule & 0x10) << hd(rule & 0x20) << '\n';
		cout << hd(rule & 0x40) << hd(rule & 0x80) << hd(rule & 0x100) << endl;
	}
	void printbuf()
	{
		printbuf(buf, size);
	}
	void printbuf(const vector<bool>& b, uint32_t sz)
	{
		for (uint32_t c = 0; c < sz; c++)
		{
			for (uint32_t r = 0; r < sz; r++)
			{
				cout << hd(b[c*sz + r]);
			}
			cout << endl;
		}
	}

	void add_rule2(uint16_t from, uint16_t to)
	{
		uint16_t r = from;

		for (int flip = 0; flip < 2; flip++)
		{
			for (int rot = 0; rot < 4; rot++)
			{
				// add
				rules2.insert(make_pair(r, to));

				// DCBA -> BDAC
				r = ((r & 1) << 1) | ((r & 2) << 2) | ((r & 4) >> 2) | ((r & 8) >> 1);
			}

			// DCBA -> CDAB
			r = ((r & 5) << 1) | ((r & 10) >> 1);
		}
	}

	void add_rule3(uint16_t from, uint16_t to)
	{
		uint16_t r = from;

		for (int flip = 0; flip < 2; flip++)
		{
			for (int rot = 0; rot < 4; rot++)
			{
				rules3.insert(make_pair(r, to));

				// IHGFEDCBA -> CFIBEHADG
				r = ((r & 0x21) << 2) | ((r & 2) << 4) | ((r & 4) << 6) |
					(r & 0x10) |
					((r & 0x40) >> 6) | ((r & 0x80) >> 4) | ((r & 0x108) >> 2);
			}

			// IHGFEDCBA -> GHIDEFABC
			r = ((r & 0x49) << 2) | (r & 0x92) | ((r & 0x124) >> 2);
		}
	}

	static uint16_t parse_rule(const string& raw)
	{
		uint16_t rule = 0;
		uint16_t mask = 1;
		for (char c : raw)
		{
			if (c == '#')
				rule |= mask;
			if (c != '/')
				mask <<= 1;
		}
		return rule;
	}

	void init()
	{
		size = 3;
		buf.clear();
		buf.resize(size*size, false);
		unpack_buf3(0x1e2, 0, 0, buf, size);	// initial pattern
	}

	uint16_t get_prod2(uint16_t rule)
	{
		auto it = rules2.find(rule);
		_ASSERT(it != rules2.end());
		return it->second;
	}

	uint16_t get_prod3(uint16_t rule)
	{
		auto it = rules3.find(rule);
		_ASSERT(it != rules3.end());
		return it->second;
	}

	void evolve2()
	{
		size_t nsize = (size / 2) * 3;
		vector<bool> nbuf;
		nbuf.resize(nsize*nsize, 0);

		for (int oy = 0, ny=0; oy < size; oy += 2, ny += 3)
		{
			for (int ox = 0, nx=0; ox < size; ox += 2, nx += 3)
			{
				uint16_t current = pack_buf2(ox, oy);
				uint16_t next = get_prod2(current);
				unpack_buf3(next, nx, ny, nbuf, nsize);
			}
		}

		size = nsize;
		buf.swap(nbuf);
	}

	void evolve3()
	{
		size_t nsize = (size / 3) * 4;
		vector<bool> nbuf;
		nbuf.resize(nsize*nsize, 0);

		for (int oy = 0, ny = 0; oy < size; oy += 3, ny += 4)
		{
			for (int ox = 0, nx = 0; ox < size; ox += 3, nx += 4)
			{
				uint16_t current = pack_buf3(ox, oy);
				uint16_t next = get_prod3(current);
				unpack_buf4(next, nx, ny, nbuf, nsize);
			}
		}

		size = nsize;
		buf.swap(nbuf);
	}

	void evolve()
	{
		// is this an 2 or an 3 step?
		if ((size & 1) == 0)
		{
			evolve2();
		}
		else
		{
			_ASSERT(size % 3 == 0);
			evolve3();
		}
	}

	int countbits()
	{
		return accumulate(buf.begin(), buf.end(), 0, [](int a, bool b) {return a + (b ? 1 : 0); });
	}

public:
	ms21_art(const list<string>& rulestext)
	{
		list<pair<uint16_t, uint16_t>> raw2;
		list<pair<uint16_t, uint16_t>> raw3;
		for (auto line : rulestext)
		{
			istringstream is(line);
			string from, dummy, to;
			is >> from >> dummy >> to;

			if (from.size() == 5)
			{
				add_rule2(parse_rule(from), parse_rule(to));
			}
			else
			{
				_ASSERT(from.size() == 11);
				add_rule3(parse_rule(from), parse_rule(to));
			}
		}
	}

	int run(int iterations)
	{
		init();
		//printbuf();

		for (int i = 0; i < iterations; ++i)
		{
			evolve();
			//cout << "---------------" << endl;
			//printbuf();
		}

		return countbits();
	}
};

int ms21_fractise(const list<string>& rules, int iterations)
{
	ms21_art art(rules);
	return art.run(iterations);
}

bool test_ms21()
{
	list<string> input;
	readfile_lines("day21_test.txt", input);
	if (ms21_fractise(input, 2) != 12)
		return false;

	return true;
}

// ..##.#..#.#.#...###.#..#.#.#.##.#.#.####...#.#.#.#....#.#.#.#.##..

class ms22_grid
{
	enum EState {
		CLEAN = 0,
		WEAK,
		INFECTED,
		FLAGGED,
	};

	size_t size;
	size_t width;
	vector<char> buf;

	inline static size_t width2size(size_t width)
	{
		return (width - 1) / 2;
	}
	inline static size_t size2width(size_t size)
	{
		return (size * 2) + 1;
	}

	// returns new size
	static size_t growbuf(vector<char>& b, size_t oldsz)
	{
		size_t nsz = oldsz + 1;
		size_t ow = size2width(oldsz);
		vector<char> n;
		size_t nw = size2width(nsz);
		n.resize(nw*nw, CLEAN);

		auto itold = b.begin();
		auto itnew = n.begin() + nw + 1;
		for (size_t y = 0; y < ow; y++)
		{
			copy(itold, itold + ow, itnew);
			itold += ow;
			itnew += nw;
		}

		b.swap(n);
		return nsz;
	}

	void grow()
	{
		size_t oldsz = size;
		size = growbuf(buf, oldsz);
		width = size2width(size);
	}

	inline vector<char>::iterator node(const v2i& p)
	{
		return buf.begin() + (p.x + p.y*width);
	}

public:
	ms22_grid(const list<string>& start)
	{
		width = start.front().size();
		size = width2size(width);
		buf.resize(width*width, 0);

		auto itbuf = buf.begin();
		for (auto& line : start)
		{
			for (char c : line)
			{
				*itbuf = (c == '#') ? INFECTED : CLEAN;
				++itbuf;
			}
		}
	}

	void dump() const
	{
		char o[] = {'.','W','#','F'};
		auto it = buf.begin();
		for (size_t r = 0; r < width; r++)
		{
			for (size_t c = 0; c < width; c++)
			{
				cout << o[*it] << ' ';
				++it;
			}
			cout << endl;
		}
		cout << "---------------------------" << endl;
	}

	int infect(int iterations)
	{
		v2i pos(size, size);
		v2i dir(0, -1);
		int infections = 0;
		for (int i = 0; i < iterations; i++)
		{
			auto n = node(pos);

			EState state = (EState)*n;
			switch (state)
			{
			case CLEAN:
				dir.left();
				*n = WEAK;
				break;

			case WEAK:
				*n = INFECTED;
				infections++;
				break;

			case INFECTED:
				dir.right();
				*n = FLAGGED;
				break;

			case FLAGGED:
				dir = -dir;
				*n = CLEAN;
				break;

			default:
				_ASSERT(false);
			}

			pos += dir;

			while (pos.x < 0 || pos.y < 0 || pos.x >= (int)width || pos.y >= (int)width)
			{
				grow();
				pos += v2i(1, 1);
			}

			if (iterations < 10)
				dump();
		}

		return infections;
	}
};

int ms22_infect(const list<string>& input, int iterations)
{
	ms22_grid grid(input);
	return grid.infect(iterations);
}

bool test_ms22()
{
	list<string> input;
	readfile_lines("day22_test.txt", input);

	//ms22_grid grid(input);
	//grid.infect(7);
	//if (grid.infect(7) != 5)
		//return false;

	ms22_grid grid2(input);
	if (grid2.infect(100) != 26)
		return false;

	//ms22_grid grid3(input);
	//if (grid3.infect(10000000) != 2511944)
	//	return false;

	return true;
}

// --{}--{}--{}--{}--{}--{}--{}--{}--{}--{}--{}--{}--{}--

class ms23_copper
{
	int64_t regs[8];

	const vector<string>& prog;
	size_t pc;

	inline int64_t* getreg(char r)
	{
		_ASSERT(r >= 'a');
		_ASSERT(r <= 'h');
		return &regs[r - 'a'];
	}

	inline void setreg(char r, int64_t val)
	{
		_ASSERT(r >= 'a');
		_ASSERT(r <= 'h');
		regs[r - 'a'] = val;
	}

	int64_t getval(const string& arg)
	{
		if (isdigit(arg[0]) || arg[0] == '-')
			return atoi(arg.c_str());

		_ASSERT(arg.length() == 1);
		return *getreg(arg[0]);
	}

public:
	ms23_copper(const vector<string>& program) : prog(program), pc(0)
	{
		for (auto& reg : regs)
			reg = 0;
	}

	void tick()
	{
		istringstream is(prog[pc]);
		string instr, arg0, arg1;
		is >> instr >> arg0;
		if (!is.eof())
			is >> arg1;

		if (instr == "set")
		{
			int64_t val = getval(arg1);
			setreg(arg0[0], val);
		}
		else if (instr == "sub")
		{
			int64_t val = getval(arg1);
			auto regit = getreg(arg0[0]);
			*regit -= val;
		}
		else if (instr == "mul")
		{
			int64_t val = getval(arg1);
			auto regit = getreg(arg0[0]);
			*regit *= val;
			mul_count++;
		}
		else if (instr == "jnz")
		{
			int64_t test = getval(arg0);
			int64_t jmp = getval(arg1);
			if (test != 0)
			{
				pc += (int)jmp;
				return;
			}
		}

		pc++;
	}

	int mul_count;
	int run_debug()
	{
		mul_count = 0;

		while (pc >= 0 && pc < prog.size())
			tick();

		return mul_count;
	}
};

int ms23_run_debug()
{
	list<string> input;
	readfile_lines("day23.txt", input);
	vector<string> prog(input.begin(), input.end());
	ms23_copper copper(prog);
	return copper.run_debug();
}

/*
	// original algo - count primes in range [108100,125100)::step 17

	c = 125100;
	for( b=108100; b!=c; b+=17)     // x 1000
	{
		f = 1;
		d = 2;

		do
		{
			e = 2;
			do
			{
				if( (d*e) == b )
					f = 0;

				e++;

			} while( e != b );

			d++;
		} while( d != b );

		if( !f )
			h++;
	}
 */
int ms23_run()
{
	int nprimes = 0;
	int start = (81 * 100) + 100000;
	int end = start + 17000;

	int i = 0;
	int64_t b = start;
	for( ;;)
	{
		bool f = true;
		int64_t d = 2;
		do
		{
			int64_t e = d;
			do
			{
				int64_t g = d*e;
				if (g == b)
				{
					f = false;
					break;
				}
				if (g > b)
					break;

				e++;
			} while (e != b);

			d++;
		} while (f && (d != b));

		if (!f)
			nprimes++;

		i++;
		if ((i & 63) == 0)
			cout << i << endl;

		if (b == end)
			break;

		b += 17;
	}

	return nprimes;
}

// ----------------======================-----------------

struct ms24_lump
{
	int16_t a, b;

	ms24_lump() {}
	ms24_lump(const string& s)
	{
		istringstream is(s);
		char dummy;
		is >> a >> dummy >> b;
	}

	void reverse()
	{
		swap(a, b);
	}

	bool operator==(const ms24_lump& other) const
	{
		if (a == other.a && b == other.b)
			return true;
		if (a == other.b && b == other.a)
			return true;
		return false;
	}
};

vector<ms24_lump> ms24_get_possibles(const vector<ms24_lump>& all, deque<ms24_lump> curr)
{
	vector<ms24_lump> possibles;

	int end = 0;
	if (!curr.empty())
		end = curr.back().b;

	copy_if(all.begin(), all.end(), back_inserter(possibles), [=](const ms24_lump& l) {return (l.a == end || l.b == end); });
	auto used = [&](const ms24_lump& l) { return find(curr.begin(), curr.end(), l) != curr.end(); };
	auto it = remove_if(possibles.begin(), possibles.end(), used);
	possibles.erase(it, possibles.end());

	for (auto& l : possibles)
	{
		if (l.a != end)
			l.reverse();
	}

	return possibles;
}

int ms24_strength(deque<ms24_lump>& bridge)
{
	return accumulate(bridge.begin(), bridge.end(), 0, [](int total, const ms24_lump& l) {return total + l.a + l.b; });
}

int ms24_rec(const vector<ms24_lump>& all, deque<ms24_lump>& curr, deque<ms24_lump>& best)
{
	auto possibles = ms24_get_possibles(all, curr);
	int strongest = ms24_strength(curr);

	for (auto& l : possibles)
	{
		curr.push_back(l);

		if ((curr.size() > best.size()) ||
			(curr.size() == best.size() && ms24_strength(curr) > ms24_strength(best)))
		{
			best = curr;
		}

		strongest = max(strongest, ms24_rec(all, curr, best));
		curr.pop_back();
	}

	return strongest;
}

int ms24_strongest(const list<string>& input)
{
	vector<ms24_lump> lumps;
	for (auto lump : input)
		lumps.emplace_back(lump);


	deque<ms24_lump> bridge;
	deque<ms24_lump> best;
	int strongest = ms24_rec(lumps, bridge, best);
	cout << "longest str = " << ms24_strength(best) << endl;
	return strongest;
}

bool test_ms24()
{
	list<string> input;
	readfile_lines("day24_test.txt", input);
	if (ms24_strongest(input) != 31)
		return false;

	return true;
}

// --- @ --- @ --- @ --- @ --- @ --- @ --- @ --- @ ---

struct ms25_state
{
	struct action
	{
		int8_t new_value;
		int8_t dir;
		uint8_t new_state;
	};
	
	action actions[2];

	ms25_state( list<string>::const_iterator& desc)
	{
		// If the current value is 0:
		++desc;
		// -Write the value 1.
		actions[0].new_value = (desc->at(desc->size() - 2) == '1') ? 1 : 0;
		++desc;
		//	- Move one slot to the right.
		actions[0].dir = (desc->at(desc->size() - 3) == 'h') ? 1 : -1;
		++desc;
		//	- Continue with state B.
		actions[0].new_state = (desc->at(desc->size() - 2) - 'A');
		++desc;
		//	If the current value is 1 :
		++desc;
		//	-Write the value 0.
		actions[1].new_value = (desc->at(desc->size() - 2) == '1') ? 1 : 0;
		++desc;
		//	- Move one slot to the left.
		actions[1].dir = (desc->at(desc->size() - 3) == 'h') ? 1 : -1;
		++desc;
		//	- Continue with state B.
		actions[1].new_state = (desc->at(desc->size() - 2) - 'A');
		++desc;
	}
};

class ms25_machine
{
	vector<ms25_state> states;
	vector<int8_t> tape;

	int64_t ticks;

	int pos;
	int state;


	void tick()
	{
		int8_t curr = tape[pos];

		ms25_state& s = states[state];
		auto action = s.actions[curr];
		tape[pos] = action.new_value;
		state = action.new_state;
		pos += action.dir;

		if (pos < 0)
		{
			tape.insert(tape.begin(), 0);
			pos++;
		}
		else if ((size_t)pos >= tape.size())
		{
			tape.push_back(0);
		}
	}

public:
	ms25_machine(const list<string>& desc) : pos(0)
	{
		list<string>::const_iterator it = desc.begin();

		// Begin in state A.
		state = 0;
		++it;

		//	Perform a diagnostic checksum after 12261543 steps.
		auto start = it->find_first_of("0123456789");
		auto end = it->find(' ', start + 1);
		string tickstr = it->substr(start, end - start).substr();
		ticks = atoi(tickstr.c_str());
		++it;

		while (it != desc.end())
		{
			//
			++it;
			//	In state A :
			++it;

			states.emplace_back(it);
		}

		tape.push_back(false);
	}

	int run_diag()
	{
		for (int i = 0; i < ticks; i++)
		{
			tick();
		}
		cout << endl;

		return accumulate(tape.begin(), tape.end(), 0, [](int tot, int8_t curr) { return tot + curr; });
	}
};

int ms25_diag(const list<string>& input)
{
	ms25_machine machine(input);
	return machine.run_diag();
}

bool test_ms25()
{
	list<string> input;
	readfile_lines("day25_test.txt", input);
	ms25_machine machine(input);

	if (machine.run_diag() != 3)
		return false;

	return true;
}

// :-:-:-:-:-:-:-:-:-:-:-:-:-:-:-:-:-:-:-:-:-:-:-:-:-:-:


int main()
{
	string inp;

	cout << "i'm back" << endl;

	cout << "ms1..." << endl;
	string input_day1 = "5994521226795838486188872189952551475352929145357284983463678944777228139398117649129843853837124228353689551178129353548331779783742915361343229141538334688254819714813664439268791978215553677772838853328835345484711229767477729948473391228776486456686265114875686536926498634495695692252159373971631543594656954494117149294648876661157534851938933954787612146436571183144494679952452325989212481219139686138139314915852774628718443532415524776642877131763359413822986619312862889689472397776968662148753187767793762654133429349515324333877787925465541588584988827136676376128887819161672467142579261995482731878979284573246533688835226352691122169847832943513758924194232345988726741789247379184319782387757613138742817826316376233443521857881678228694863681971445442663251423184177628977899963919997529468354953548612966699526718649132789922584524556697715133163376463256225181833257692821331665532681288216949451276844419154245423434141834913951854551253339785533395949815115622811565999252555234944554473912359674379862182425695187593452363724591541992766651311175217218144998691121856882973825162368564156726989939993412963536831593196997676992942673571336164535927371229823236937293782396318237879715612956317715187757397815346635454412183198642637577528632393813964514681344162814122588795865169788121655353319233798811796765852443424783552419541481132132344487835757888468196543736833342945718867855493422435511348343711311624399744482832385998592864795271972577548584967433917322296752992127719964453376414665576196829945664941856493768794911984537445227285657716317974649417586528395488789946689914972732288276665356179889783557481819454699354317555417691494844812852232551189751386484638428296871436139489616192954267794441256929783839652519285835238736142997245189363849356454645663151314124885661919451447628964996797247781196891787171648169427894282768776275689124191811751135567692313571663637214298625367655969575699851121381872872875774999172839521617845847358966264291175387374464425566514426499166813392768677233356646752273398541814142523651415521363267414564886379863699323887278761615927993953372779567675";
	cout << (test_ms1() ? "passed!" : "nope :(") << endl;
	cout << "Final answer: " << ms1_invcaptcha(input_day1) << endl;
	cout << "Final answer B: " << ms1_invcaptcha(input_day1, true) << endl;
	cout << "--------" << endl;

	cout << "ms2..." << endl;
	cout << (test_ms2() ? "passed!" : "nope :(") << endl;
	string input_day2 = "5048	177	5280	5058	4504	3805	5735	220	4362	1809	1521	230	772	1088	178	1794;6629	3839	258	4473	5961	6539	6870	4140	4638	387	7464	229	4173	5706	185	271;5149	2892	5854	2000	256	3995	5250	249	3916	184	2497	210	4601	3955	1110	5340;153	468	550	126	495	142	385	144	165	188	609	182	439	545	608	319;1123	104	567	1098	286	665	1261	107	227	942	1222	128	1001	122	69	139;111	1998	1148	91	1355	90	202	1522	1496	1362	1728	109	2287	918	2217	1138;426	372	489	226	344	431	67	124	120	386	348	153	242	133	112	369;1574	265	144	2490	163	749	3409	3086	154	151	133	990	1002	3168	588	2998;173	192	2269	760	1630	215	966	2692	3855	3550	468	4098	3071	162	329	3648;1984	300	163	5616	4862	586	4884	239	1839	169	5514	4226	5551	3700	216	5912;1749	2062	194	1045	2685	156	3257	1319	3199	2775	211	213	1221	198	2864	2982;273	977	89	198	85	1025	1157	1125	69	94	919	103	1299	998	809	478;1965	6989	230	2025	6290	2901	192	215	4782	6041	6672	7070	7104	207	7451	5071;1261	77	1417	1053	2072	641	74	86	91	1878	1944	2292	1446	689	2315	1379;296	306	1953	3538	248	1579	4326	2178	5021	2529	794	5391	4712	3734	261	4362;2426	192	1764	288	4431	2396	2336	854	2157	216	4392	3972	229	244	4289	1902;";
	cout << "Final answer: " << ms2_chexum(input_day2) << endl;
	cout << "test b: " << (test_ms2b() ? "passed!" : "nope :(") << endl;
	cout << "Final answer B: " << ms2b_multiplechex(input_day2) << endl;
	cout << "--------" << endl;

	cout << "ms3..." << endl;
	cout << (test_ms3() ? "passed :D" : "nope :(") << endl;
	int input_day3 = 277678;
	cout << "Final answer: " << ms3_spiral_dist(input_day3) << endl;
	cout << "Final answer B: " << ms3b_spiral_seq(input_day3) << endl;
	cout << "--------" << endl;

	cout << "ms4..." << endl;
	cout << (test_ms4() ? "looking good" : "oh no") << endl;
	list<string> input_day4;
	readfile_lines("day4.txt", input_day4);
	cout << "Final answer: " << ms4_countvalid(input_day4) << endl;
	cout << "Final answer B: " << ms4_countvalid(input_day4, false) << endl;
	cout << "--------" << endl;

	cout << "ms5..." << endl;
	cout << (test_ms5() ? "looking good" : "oh no") << endl;
	list<string> input_day5;
	readfile_lines("day5.txt", input_day5);
	cout << "Final solution: " << ms5_boot(input_day5) << endl;
	cout << "Final solution B SKIPPED due to impatience" << endl;
	//cout << "Final solution B: " << ms5_boot(input_day5, true) << endl;
	cout << "--------" << endl;

	cout << "ms6..." << endl;
	cout << (test_ms6() ? "shiny" : "crusty :/") << endl;
	string input_day6 = "11	11	13	7	0	15	5	5	4	4	1	1	7	1	15	11";
	cout << "Both final solutions skipped due to IMMMMPATIENCE" << endl;
	//cout << "Final solution: " << ms6_run(input_day6) << endl;
	//cout << "Final solution B: " << ms6_run(input_day6, true) << endl;
	cout << "--------" << endl;

	cout << "ms7..." << endl;
	cout << (test_ms7() ? "oh, we happy." : "oh no") << endl;
	list<string> input_day7;
	readfile_lines("day7.txt", input_day7);
	cout << "Final solution: " << ms7_assemble(input_day7) << endl;
	cout << "--------" << endl;

	cout << "ms8..." << endl;
	cout << (test_ms8() ? "zing!" : "uf") << endl;
	list<string> input_day8;
	readfile_lines("day8.txt", input_day8);
	cout << "Final solution: " << ms8_run(input_day8) << endl;
	cout << "--------" << endl;

	cout << "ms9..." << endl;
	cout << (test_ms9() ? "woop" : "wah") << endl;
	list<string> input_day9;
	readfile_lines("day9.txt", input_day9);
	ms9_total_garbage_eaten = 0;
	cout << "Final solution: " << test_ms9_groups(input_day9.front()) << endl;
	cout << "I HAVE EATEN " << ms9_total_garbage_eaten << " OF GARBAGE OM NOM NOM NOM NOM" << endl;
	cout << "--------" << endl;

	cout << "ms10..." << endl;
	cout << (test_ms10() ? "zing!" : "uf") << endl;
	ms10_t v10;
	ms10_init(v10, 255);
	cout << "Final solution: " << ms10_do_the_splits(v10, ms10_t({ 18,1,0,161,255,137,254,252,14,95,165,33,181,168,2,188 })) << endl;
	cout << "Final solution B: " << ms10_hash("18,1,0,161,255,137,254,252,14,95,165,33,181,168,2,188") << endl;
	cout << "--------" << endl;

	cout << "ms11..." << endl;
	cout << (test_ms11() ? "hexed it" : "bum") << endl;
	list<string> input_day11;
	readfile_lines("day11.txt", input_day11);
	cout << "Final solution: " << ms11_hex(input_day11.front()) << endl;
	cout << "Final solutionB : " << ms11_furthest(input_day11.front()) << endl;
	cout << "--------" << endl;

	cout << "ms12..." << endl;
	cout << (test_ms12() ? "piping" : "leaky") << endl;
	list<string> input_day12;
	readfile_lines("day12.txt", input_day12);
	cout << "Final solution: " << ms12_pipes(input_day12) << endl;
	cout << "Final solution B: " << ms12_count_piping_groups(input_day12) << endl;
	cout << "--------" << endl;

	cout << "ms13 - halfway there!!" << endl;
	cout << (test_ms13() ? "l33t" : "iced") << endl;
	list<string> input_day13;
	readfile_lines("day13.txt", input_day13);
	cout << "Finale: " << ms13_go_go_go(input_day13) << endl;
	cout << "Encore: " << "FUCK. THAT." /*ms13_not_too_steady(input_day13)*/ << endl;
	cout << "--------" << endl;

	cout << "ms14..." << endl;
	//cout << (test_ms14() ? "nice" : "frag") << endl;
	cout << "Finale: " << "NOPE" /*ms14_count_used("uugsqrei")*/ << endl;
	cout << "Encore: " << "NOPER" /*ms14_count_regions("uugsqrei")*/ << endl;
	cout << "--------" << endl;

	cout << "ms15..." << endl;
	cout << "skip ALL of these..." << endl;
	/*cout << (test_ms15() ? "tidy" : "oh dear") << endl;
	cout << "flib: " << ms15_judge(40 * 1000 * 1000, 512, 191) << endl;
	cout << "flimber: " << ms15_judger(5 * 1000 * 1000, 512, 191) << endl;*/
	cout << "--------" << endl;

	cout << "ms16..." << endl;
	//cout << (test_ms16() ? "dance!" : "plummet :(") << endl;
	list<string> input_day16;
	readfile_lines("day16.txt", input_day16);
	cout << "after the dance: " << ms16_dance('p', input_day16.front()) << endl;
	//cout << "past everyone's bedtime: " << ms16_dance('p', input_day16.front(), 1000*1000*1000) << endl;
	cout << "past everyone's bedtime: " << "GO TO BED." /*ms16_dance('p', input_day16.front(), 40)*/ << endl;
	cout << "--------" << endl;

	cout << "ms17..." << endl;
	cout << (test_ms17() ? "spinnin'" : "locked") << endl;
	cout << "spun: " << ms17_spinlock(367) << endl;
	cout << "angry: " << ms17_afterzero(367, 50*1000*1000) << endl;
	cout << "--------" << endl;

	cout << "ms18..." << endl;
	cout << (test_ms18() ? "bangin" : "farty noises") << endl;
	list<string> input_day18;
	readfile_lines("day18.txt", input_day18);
	cout << "duended: " << "skipping all the way" /*ms18_duet(input_day18)*/ << endl;
	cout << "~~~~~~~~" << endl;

	cout << "ms19..." << endl;
	cout << (test_ms19() ? "piping hot" : "soggy leak") << endl;
	list<string> input_day19;
	readfile_lines("day19.txt", input_day19);
	int nsteps = 0;
	cout << "journey -=- " << ms19_pipes(input_day19, &nsteps) << endl;
	cout << "went " << nsteps << endl;
	cout << "~~~~~~~~" << endl;

	cout << "ms20..." << endl;
	cout << (test_ms20() ? "party-cal" : "pooped") << endl;
	list<string> input_day20;
	readfile_lines("day20.txt", input_day20);
	cout << "centre particle was " << "A MYSTERY NEVER TO BE SOLVED" /* ms20_slowest_particle(input_day20,1000) */<< endl;
	cout << "-=-=--=-=--==-==--=---" << endl;

	cout << "ms21..." << endl;
	cout << (test_ms21() ? "fracTAL" : "fractAWFUL") << endl;
	list<string> input_day21;
	readfile_lines("day21.txt", input_day21);
	cout << "fractitious : " << ms21_fractise(input_day21, 5) << endl;
	cout << "- = - = - = - = - = - = -" << endl;

	cout << "ms22..." << endl;
	cout << (test_ms22() ? "pathoLOGICAL" : "sick :/") << endl;
	list<string> input_day22;
	readfile_lines("day22.txt", input_day22);
	cout << "final: " << " N O P E " /*ms22_infect(input_day22, 10000000)*/ << endl;
	cout << "-{o}-----{o}-----{o}-----{o}-----{o}-----{o}-" << endl;

	cout << "ms23..." << endl;
	//cout << "DBG: " << ms23_run_debug() << endl;
	//cout << "RUN: " << ms23_run() << endl;
	cout << "---------------------------------" << endl;

	cout << "ms24..." << endl;
	cout << (test_ms24() ? "safe croissant" : "crocotastrophe") << endl;
	list<string> input_day24;
	readfile_lines("day24.txt", input_day24);
	//cout << "take it to the " << ms24_strongest(input_day24) << endl;
	cout << "-----------------------------" << endl;

	cout << "msXMAS..." << endl;
	cout << (test_ms25() ? "turing COMPLETE" : "probleme") << endl;
	list<string> input_day25;
	readfile_lines("day25.txt", input_day25);
	cout << "ttttuuuurrririiinnnngggssggg " << ms25_diag(input_day25) << endl;
	cout << "...\n\n - D - O - N - E -" << endl;

    return 0;
}

