#include "SParser.h"
#include <iostream>
#include "Database.h"

#include <iostream>
#include <map>
#include <string>
#include <stack>
using namespace std;

#define ITERATION 2

// constructor

SimpleParser::SimpleParser() {
	sym = defaultStart; // curlyopen
	errString = "start of program";
	lineNo = 1;
	tokencounter = 0; // 3
	blockcounter = 0;
	parentLine = 0;
	initMap();
	real_expression = false;

}
// destructor
SimpleParser::~SimpleParser() {}

int SimpleParser::simpleparse(vector<string>& tokens) {
	
	fetchToken(tokens);
	while (tokens.size() > tokencounter) {	 // ** ITR2_add loop till end of vector string; cast int 
		if (procBlock(tokens)) {
			if (sym == curlyClose) {		// if i dont see a curly close give a error 
				if (tokencounter >= tokens.size())
				{
					return 1;
				}
				else {
					fetchToken(tokens);
					procID++;
					if (tokens.size() > tokencounter) {				//** ITR2_change why FailExit when curlyClose is the right response	"
						if (sym == keyProcedure) {
							continue;
						}
						else {
							lookAhead(tokens, curlyClose);
							continue;
						}
					}
				}
			}
		}
		else {			
			FailExit("simpleparser fail");
			return 0;						// ** ITR2_add must do this to break while loop 
		}
	}										// ** ITR2_add END while loop over entire vector of strings 
	return 1;

}

int SimpleParser::initMap() {


	//map for maths operators 

	key_map.insert(std::pair<string, Symbol>("*", times));
	key_map.insert(std::pair<string, Symbol>("+", plus));
	key_map.insert(std::pair<string, Symbol>("-", minus));
	key_map.insert(std::pair<string, Symbol>("/", divide));
	key_map.insert(std::pair<string, Symbol>("=", equal));
	key_map.insert(std::pair<string, Symbol>("<", lessThan));
	key_map.insert(std::pair<string, Symbol>(">", greaterThan));
	key_map.insert(std::pair<string, Symbol>(";", semiColon));
	key_map.insert(std::pair<string, Symbol>("{", curlyOpen));
	key_map.insert(std::pair<string, Symbol>("}", curlyClose));
	key_map.insert(std::pair<string, Symbol>(",", comma));
	key_map.insert(std::pair<string, Symbol>("%", modulo));			// ** ITR2_add
	key_map.insert(std::pair<string, Symbol>("(", bracketOpen));	// ** ITR2_add
	key_map.insert(std::pair<string, Symbol>(")", bracketClose));	// ** ITR2_add


	// logical operator map // 


	logic_map.insert(std::pair<string, Symbol>("&&", logicAnd));
	logic_map.insert(std::pair<string, Symbol>("||", logicOr));
	logic_map.insert(std::pair<string, Symbol>("!=", notEqual));
	logic_map.insert(std::pair<string, Symbol>(">=", greaterEqual));
	logic_map.insert(std::pair<string, Symbol>("<=", lessEqual));
	logic_map.insert(std::pair<string, Symbol>("==", doubleEqual));



	return 0;
}

int SimpleParser::fetchToken(vector<string>& tokens) {

	if (tokens.size() == 0) {
		FailExit("empty tokens at source parser fetchToken");
		return 0; 
	}

	if (tokencounter > tokens.size()) {

		FailExit("tokencounter bigger than tokens size logic for source not handled");
		return 0;
	}

	for (auto& t : key_map) {

		if (t.first == tokens.at(tokencounter)) {
			sym = t.second;
			tokencounter++;
			return 1;
		}
	}

	// if logical operator 
	for (auto& t : logic_map) {

		if (t.first == tokens.at(tokencounter)) {
			sym = t.second;
			tokencounter++;
			return 1;
		}
	}

	if (tokens.at(tokencounter) == "procedure") {
		sym = keyProcedure;
		tokencounter++;
		return 1;
	}
	if (tokens.at(tokencounter) == "read") {
		sym = keyRead;
		tokencounter++;
		return 1;
	}
	if (tokens.at(tokencounter) == "print") {
		sym = keyPrint;
		tokencounter++;
		return 1;
	}

	if (tokens.at(tokencounter) == "while") {	// ** ITR2_add
		sym = keyWhile;							// ** ITR2_add
		tokencounter++;							// ** ITR2_add
		return 1;								// ** ITR2_add
	}											// ** ITR2_add

	if (tokens.at(tokencounter) == "if") {		// ** ITR2_add
		sym = keyIf;							// ** ITR2_add
		tokencounter++;							// ** ITR2_add
		return 1;								// ** ITR2_add
	}											// ** ITR2_add

	if (tokens.at(tokencounter) == "then") {
		sym = keyThen;
		tokencounter++;
		return 1;
	}

	if (tokens.at(tokencounter) == "else") {
		sym = keyElse;
		tokencounter++;
		return 1;
	}

	int var = tokens.at(tokencounter)[0];

	if (((var >= 65) && (var <= 90)) || ((var >= 97) && (var <= 122))) {  // letters a-z & A-Z 

		sym = name;
		tokencounter++;
		return 1;
	}


	if (((var >= 48) && (var <= 57))) {

		for (auto& t : tokens.at(tokencounter)) {			// loop through the entire string to make sure its a real integer 
			if (!((t >= 48) && (t <= 57))) {

				return 0;
			}
		}
		sym = const_value;
		tokencounter++;
		return 1;
	}
	// increment counter
	return 0;

}


int SimpleParser::lookAhead(vector<string>& tokens, Symbol look_sym) {

	if (fetchToken(tokens)) {

		if (sym == look_sym) {
			return 1;
		}
		else {
			return 0;
		}
	}
	else {
		return 0;
	}

}



int SimpleParser::statementList(vector<string>& tokens, int nexttoken) {  // ** ITR2_change nexttoken not used?

	while (!lookAhead(tokens, curlyClose)) {
		// PRODUCTION	read: 'read' var_name ';'	
		if (sym == keyRead) {

			if (lookAhead(tokens, name)) {
				Database::insertRead(tokens.at(tokencounter - 1), lineNo);
				Database::insertVariable(tokens.at(tokencounter - 1));  // ToDo: will be a STRING from the vector itself 
				if (lookAhead(tokens, semiColon)) {
					Database::insertStatement(tokens.at(tokencounter - 1), lineNo, parentLine); // ToDo: check tokens.at is correct position 
					lineNo++;
				}
				else {		
					FailExit("keyread fail - line 221");
					return 0;
				}
			}
		}
		// PRODUCTION	print: 'print' var_name;
		if (sym == keyPrint) {

			if (lookAhead(tokens, name)) {
				Database::insertVariable(tokens.at(tokencounter - 1));  //#tofix  | will be a STRING from the vector itself 
				Database::insertPrint(tokens.at(tokencounter - 1), lineNo);
				if (lookAhead(tokens, semiColon)) {
					Database::insertStatement(tokens.at(tokencounter - 1), lineNo, parentLine); // ToDo: check tokens.at is correct position 
					lineNo++;
				}
				else {
					FailExit("print fail line 226");
					return 0;
				}
			}
		}

		// PRODUCTION	assign: var_name ‘=’ expr ‘;’
		if (sym == name) {

			Database::insertVariable(tokens.at(tokencounter - 1));

			if (lookAhead(tokens, equal)) {
				start_expression = tokencounter;
				pattern_before_equal = tokencounter - 2;
				if (Database::varCheck(tokens, tokens.at(tokencounter - 2))) {
					Database::insertVariableMSV(tokens.at(tokencounter - 2), lineNo);
				
				}
				else {
				
					Database::insertVariableMSVNULL(tokens.at(tokencounter - 2), lineNo);
				}
				Database::insertBlockMPV(lineNo, tokens.at(tokencounter - 2), procID);
				Database::insertAssign(lineNo, tokens.at(tokencounter), tokens.at(tokencounter - 2));
				int mavcheck = tokens.at(tokencounter)[0];
				if(((mavcheck >= 65) && (mavcheck <= 90)) || ((mavcheck >= 97) && (mavcheck <= 122))) {  // letters a-z & A-Z 
					
					Database::insertAssignMSVR(lineNo,tokens.at(tokencounter));
				}


				if (expr(tokens)) {

					if (real_expression == true) {
						SimpleParser::expression(tokens);
						Database::insertPattern(lineNo, tokens.at(pattern_before_equal), patternRHS);
						// inserting of rhs after changing to posfix/prefix 
						Database::insertStatement(tokens.at(tokencounter - 1), lineNo, parentLine); // ToDo: check tokens.at is correct position 
						tokencounter = end_expression;
						real_expression = false;
					}
					Database::insertStatement(tokens.at(tokencounter - 1), lineNo, parentLine);
					lineNo++;
				}

			}
			else {
					return 0;
			}
		}

			// PRODUCTION	if: ‘if’ ‘(’ rel_expr ‘)’ ‘then’ ‘{‘ stmtLst ‘}’ ‘else’ ‘{‘ stmtLst ‘}’		
		if (sym == keyIf) {																	//	** ITR2_add
																							//	** ITR2_add
		// If GRAMMAR:  if: ‘if’ ‘(’ rel_expr ‘)’ ‘then’ ‘{‘ stmtLst ‘}’ ‘else’ ‘{‘ stmtLst ‘}’ ** ITR2_add
		// rel_expr  :  rel_factor ‘>’ rel_factor | rel_factor ‘<’ rel_factor					** ITR2_add
		// rel_factor:  rel_factor: var_name | const_value | expr								** ITR2_add
		// ------------------------------------------------------------------------------------ ** ITR2_add

			if (lookAhead(tokens, bracketOpen)) {
				if (relExpr(tokens)) { // ** ITR2_add ToDo: complete relExpr 
					if (lookAhead(tokens, bracketClose)) {
						if (lookAhead(tokens, keyThen)) {
							if (lookAhead(tokens, curlyOpen)) {
								Database::insertStatementIf(tokens.at(tokencounter - 1), lineNo, parentLine,1);
								parentLine = lineNo;
								lineNo++;
								if (statementList(tokens, curlyClose)) {
									if (lookAhead(tokens, keyElse)) {
										if (lookAhead(tokens, curlyOpen)) {
											if (statementList(tokens, curlyClose)) {
												if (sym == curlyClose) {
													parentLine = 0;
													
												}
											}
											else {
												FailExit("keyIF statement list fail line 301");
												return 0;
											}
										}
										else {
											FailExit("keyIF lookahead curly open fail line 300");
											return 0;
										}
									}
									else {
										// completed if (rel_expr) then {stmtLst}
										return 1;
									}
								}
								else {
									FailExit("keyIF curly close fail line 288");
									
									return 0;
								}
							}
							else {
								FailExit("keyif curly open fail line 284");
								return 0;
							}
						}
						else {
							FailExit("keyIf THEN clause fail line 283");
							return 0;
						}
					}
					else { 
						FailExit("keyIf bracketOpen fail line 282");
						return 0;
					}
				}
				else {
					FailExit("keyIf relExpr fail line 281");
					return 0;
				}
			}
			else {
				FailExit("keyIf lookahead bracketOpen fail line 280");
				return 0;
			}
		}

		// PRODUCTION	while: ‘while’ ‘(’ rel_expr ‘)’ ‘{‘ stmtLst ‘}’  THIS IS WHILE LOOP  
		if (sym == keyWhile) {
			if (lookAhead(tokens, bracketOpen)) {
				if (relExpr(tokens)) {
					if (lookAhead(tokens, bracketClose)) {
						if (lookAhead(tokens, curlyOpen)) {
							Database::insertStatementWhile(tokens.at(tokencounter - 1), lineNo, parentLine,1);
							parentLine = lineNo;
							lineNo++;
							if (statementList(tokens, curlyClose)) {
								if (sym == curlyClose) {
									parentLine = 0;
								

								}
								else {
									FailExit("keyWhile statementList curlyclose line 354");
									return 0;
								}
							}
							else {
								FailExit("keyWhile statementList curlyclose line 353");
								return 0;
							}
						}
						else {
							FailExit("keywhile lookahead curly open line 249");
							return 0;
						}
					}
					else {
						FailExit("keywhile bracketclose line 349");
						return 0;
					}
				}
				else {
					FailExit("keywhile relExpr line 347");
					return 0;
				}
			}
			else {
				FailExit("keywhile bracketopen line 346");
				return 0;
			}
		}
	}
	return 1;
}


int SimpleParser::procBlock(vector<string>&tokens) {
	// starting with procedure																																																													
	if (sym == keyProcedure) {
		if (lookAhead(tokens, name)) {	// this name will be procedure as it is the start of procblock
			
			Database::insertProcedure(tokens.at(tokencounter - 1),procID);

			if (lookAhead(tokens, curlyOpen)) {

				if (statementList(tokens, tokencounter)) {						
					return 1;	// end of 1 procedure 
				}
				else {
					FailExit("keyProc statementList line 402");
					return 0; // no return 0 from iteration 2 base file
				}
			}
			else {
				FailExit("keyProc curlyopen line 400");
				return 0; // no return 0 from iteration 2 base file
			}
		}
		else {
			FailExit("keyProc lookahead name line 396");
			return 0;
		}			
	}
	else {	
		FailExit("keyProc fail"); // ** ITR2_add assumption: perfect input syntax 
		return 0;	// ** ITR2_add keyword not 'procedure' i.e. wrong
	}				// ** ITR2_add
}

int SimpleParser::relExpr(vector<string>&tokens) {		// ** ITR2_add

	// PRODUCTION rel_expr: rel_factor ‘>’ rel_factor | rel_factor ‘<’ rel_factor

	if (relFactor(tokens)) {
		lookAhead(tokens, curlyClose);
		if ((sym == greaterThan) || (sym == lessThan) || (sym == logicAnd) || (sym == logicOr) || (sym == notEqual) || (sym == greaterEqual) || (sym == lessEqual) || (sym == doubleEqual)) {
			if (relFactor(tokens)) {
				return 1;
			}
			else {
				FailExit("relFactor > < == || !=  after curlyclose fail");
				return 0;
			}
		}
		else {
			FailExit("relFactor curlyClose fail");
			return 0;
		}
	}

	return 1; // dummy success																 
}																	

int SimpleParser::relFactor(vector<string>&tokens) {	// ** ITR2_add

	// PRODUCTION rel_factor: var_name | const_value | expr

	lookAhead(tokens, name);
	if (sym == name) {
		if (Database::varCheck(tokens, tokens.at(tokencounter - 1))) {

			Database::insertVariableUSV(tokens.at(tokencounter-1),lineNo);
		}
		else {
			Database::insertVariableUSVNULL(tokens.at(tokencounter - 1), lineNo);
		}

		Database::insertVariable(tokens.at(tokencounter - 1)); // ADD THE VAR 

		return 1;
	}
	if (sym == const_value) {

		Database::insertConstant(lineNo, tokens.at(tokencounter - 1)); // put in the value 
		return 1;
	}
	if (expr(tokens)) // ALERT let expr handle whatever next token is
		return 1;
	else {
		FailExit("relFactor not name, const_value or expr-- fail");
		return 0;
	}
}

	int SimpleParser::term(vector<string>&tokens, int nextToken) {		

		// ENTRY POINTS	expr: term exprPrime
		// ENTRY POINTS	exprPrime: '+' term exprPrime | '-' term exprPrime | epsilon

		// PRODUCTION	term: factor termPrime
		if (factor(tokens, semiColon)) {
			if (termPrime(tokens, semiColon)) {
				// ASSIGN	unwind, nothing to handle
				return 1;
			}
			else {
				FailExit("termPrime semicolon fail");
				return 0;
			}
		}
		else {
			FailExit("term semicolon fail");
			return 0;
		}
	}

	int SimpleParser::termPrime(vector<string>&tokens, int nextToken) {	// ** ITR2_add

		// ENTRY POINTS	term: factor termPrime

		// PRODUCTION	termPrime: '*' factor termPrime | '/' factor termPrime |  '%' factor termPrime | epsilon

		lookAhead(tokens, times);
		if ((sym == times) || (sym == divide) || (sym == modulo)) { // ToDo: need break ORs to 3 ifs?
			real_expression = true;
			if (factor(tokens, semiColon)) {
				if (termPrime(tokens, semiColon))
					return 1; // ToDo: check- DB writting all done by factor
				else {
					FailExit("termPrime in factor fail");
					return 0;
				}
			}
		}
		// ALERT treat epsilon as semiColon
		// ALERT let expr handle all cases besides * / % 
		// ALERT nothing else to handle here

		return 1; // dummy success
	}

	int SimpleParser::factor(vector<string>&tokens, int nexttoken) {		// ** ITR2_add

		// PRODUCTION factor: var_name | const_value | ‘(’ expr ‘)’

		lookAhead(tokens, name);
		if (sym == name) {
			if (!Database::blockUPVCheck(tokens, tokens.at(tokencounter - 1), procID)) { 
				Database::insertBlockUPV(lineNo, tokens.at(tokencounter - 1), procID); // ELSE add UPV variable to block table 
			}
			Database::insertVariable(tokens.at(tokencounter - 1));	
			return 1; // dummy success
		}
		if (sym == const_value) {
			Database::insertConstant(lineNo,tokens.at(tokencounter-1));				// ToDo: still valid? int conver to string	
			return 1; // dummy success
		}
		if (sym == bracketOpen) {
			expr(tokens);
			// **** INSERT LOOKAHEAD HERE IF NOT SPACE IS REQUIRE FOR EXPRESSION EG NUM2 = A + (B + C); ***
			if (sym == bracketClose) {
				return 1;
			}
			else {
				FailExit("factor no bracketClose fail");
				return 0;
			}
		}
		else {
			FailExit("factor not name, const_value or bracketOpen fail");
			return 0;
		}
	}

	int SimpleParser::expr(vector<string>&tokens) {			// ** ITR2_add

		// expr: term exprPrime
		// exprPrime: '+' term exprPrime | '-' term exprPrime | epsilon
		// term: factor termPrime
		// termPrime: '*' factor termPrime | '/' factor termPrime |  '%' factor termPrime | epsilon
		// factor: var_name | const_value | ‘(’ expr ‘)’

		// sym = bracketOpen if called from assign
		// sym = ? if called from factor
		// sym = ? if called from relFactor

		
		if (term(tokens, semiColon)) {
			if (exprPrime(tokens, semiColon)) {
				// ASSIGN	nothing to do, just unwind
				end_expression = tokencounter-1; // CHECK this line to ensure it is on ';'
				return 1;

			}
			else {
				FailExit("exprPrime in expr: no semicolon fail");
				return 0;
			}
		}
		else {
			FailExit("expr: term fail");
			return 0;
		}


		// CONDITION ASSIGN must exit with sym = semiColon
		// CONDITION ASSIGN so that stmnt/expr gets written into DB
		// CONDITION ASSIGN in stmtList
		// CONDITION WHILE  ??
		// CONDITION IF		??

		return 1; // dummy success
	}

	int SimpleParser::exprPrime(vector<string>&tokens, int nextToken) {	// ** ITR2_add

		if (sym == semiColon) {
			// ALERT treat semiColon as epsilon case
			// ALERT DB:insert stmt when return to expr 
			return 1;
		}
		else if ((sym == plus) || (sym == minus)) {
			real_expression = true;
			if (term(tokens, semiColon)) { // ToDo: check flow if nextToken doesn't matter
				if (exprPrime(tokens, semiColon)) {
					return 1;
				}
				else {
					FailExit("exprPrime: exprPrime fail");
					return 0;
				}
			}
			else {
				FailExit("exprPrime: term fail");
				return 0;
			}
		}
	}

	
	//Function to return precedence of operators
	int SimpleParser::prec(char c) {
		if (c == '^')
			return 3;
		else if (c == '/' || c == '*' || c == '%')
			return 2;
		else if (c == '+' || c == '-')
			return 1;
		else
			return -1;
	}
	
	//converts infix to postfix || prefer to use prefix
	int SimpleParser::expression(vector<string>& tokens) {
	  
		stack<char> st; //For stack operations, we are using C++ built in stack
		string result;
		while (start_expression < end_expression) {

			string s = tokens.at(start_expression);

			 //for (int i = 0; i < s.length(); i++) {
				char c = s[0];

				// if the scanned character is an operand, add it to output string.
				if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')) {

					if (c >= '0' && c <= '9') {

						// add constant 
					}
					else {
						// varcheck
					}
						expression_result.push_back(s);
						result +=s;
				}
				// if the scanned character is an ‘(‘, push it to the stack.
				else if (c == '(')
					st.push('(');

				// if the scanned character is an ‘)’  pop and to output string from the stack  until an ‘(‘ is encountered.
				else if (c == ')') {
					while (st.top() != '(')
					{
						result += st.top();
						st.pop();

					}
					st.pop();
				}
				//if an operator is scanned
				else {
					while (!st.empty() && prec(s[0]) <= prec(st.top())) {
						std::string char_to_string;
						char_to_string = st.top();
						expression_result.push_back(char_to_string); // double check
						result += st.top();
						st.pop();
					}
					st.push(c);
				}
			//}
				start_expression++;
		}
		// Pop all the remaining elements from the stack
		while (!st.empty()) {
			result += st.top(); // result of the new stack
			st.pop();
		}
		patternRHS = result;
		return 1;
	}
	