#include "QParser.h"
#include <iostream>
#include "Database.h"
#include <iostream>
#include <map>
#include <string>
#include <stack>
using namespace std;


QueryParser::QueryParser() {

	qsym = QSymbol::qdefaultStart;
	qsearchCl = QClause::cdefaultStart;
	LHStype = QSymbol::qdefaultStart;
	RHStype = QSymbol::qdefaultStart;
	clearqLHS();
	clearqRHS();
	// qrelRef = QSymbol::qdefaultStart; 
	storeqLHSstring("nothing");
	storeqRHSstring("nothing");
	qtblName = "nothing"; 
	qtokencounter = 0;
	initQMap();
	fetch2tokens = true;
	Select_Result_Insert = false;


}

Node::Node() {

}
// destructor
Node::~Node() {

}


// destructor
QueryParser::~QueryParser() {

}

int QueryParser::queryParse(vector<string>& tokens) {


	// select-cl: declaration+ 'Select' result-cl [suchthat-cl | pattern-cl] [suchthat-cl | pattern-cl] 
	// declaration: declaration : design-entity synonym ( ',' synonym )* ';'
	// design-entity: 'stmt' | 'read' | 'print' | 'assign' | 'while' | 
	//					'if' | 'variable' | 'constant' | 'procedure' 

	if (!declaration(tokens)) { 
		// qsym = qSelect when declaration is done 
		qFail("declaration fail");
		return 0;
	}

	if(!isQsymDeclaredSynonym(tokens)) {
		// resultCl checks if synonyms are declared in Select stmnt 
		qFail("result clause fail");
		return 0;
	}
		
	if (!isTwoSyn(tokens)) {

		// handling tuple 
		qFail("tuple not handled");
		qdbResults.at(0) = ""; // returns none to dbresults
		return 1;

	}
	fetchQToken(tokens); // fetch after select 
	select_variable = tokens.at(qtokencounter); // double check this tokencounter 
	set_Select_variable(tokens);

	if(!isSuchThat(tokens)){
		qFail("ONLY GOT SELECT STATEMENT");
		noSuchThatQuery(tokens);
		return 1;
	}

	if((qsearchCl == QClause::csuchthat) ||
		(qsearchCl == QClause::cpatternsuchthat) ||
		(qsearchCl == QClause::csuchthatsuchthat) ){
		suchThat(tokens);
	}
	else if((qsearchCl == QClause::cpattern) ||
		(qsearchCl == QClause::cpatternpattern) ||
		(qsearchCl == QClause::csuchthatsuchthat) ){
		pattern(tokens);
	}
	

	// this won't work - its in PQL_select 


	return 1;
		
}


int QueryParser::isTwoSyn(vector<string>& tokens) {

	if ((qtokencounter+3) >= (int)tokens.size()) {
		qFail("is not Two Synonym");
		return 0;
	}
	if (tokens.at(qtokencounter + 1) == "<") {
		return 0; // we did not handle tuple
	}
	return 1;
}

int QueryParser::noSuchThatQuery(vector<string>& tokens) {

	if ((qtokencounter) >= (int)tokens.size()) {
		qFail("select ended without inputting a variable");
		return 0;
	}

	qtokencounter++; // to move to the letter after select 
	string dbquerytype;

	for (auto& t : PQL_select) {
	
		if (t.first == tokens.at(qtokencounter)) {
			dbquerytype = t.second;
		}
		else {
			qFail("the select statement is not part of SIMPLE grammar");
		}
	}

	// check in the map what type of query will be done based on this variable 
	
	if (dbquerytype == "procedure") {
		Database::getProcedures(qdbResults);
	}

	if (dbquerytype == "variable") {
		Database::getVariable(qdbResults);
	}

	if (dbquerytype == "stmt") {
		Database::getStatement(qdbResults);
	}

	if (dbquerytype == "constant") {
		Database::getConstant(qdbResults);
	}

	if (dbquerytype == "read") {
		Database::getRead(qdbResults);
	}

	if (dbquerytype == "print") {
		Database::getPrint(qdbResults);
	}
	if (dbquerytype == "assign") {
		Database::getAssign(qdbResults);
	}

	return 1;
}




int QueryParser::resultCl(vector<string>& tokens) {
	//PRODUCTION
	// result - cl : tuple
	// tuple : synonym | '<' synonym(',' synonym) * '>'

	if (!fetchQToken(tokens)) {
		qFail("fetchQToken fail in resultCl ");
		return 0;
	}

	// if PQL_select size = 1 - next token must be synonym
	// ITCHY HANDS synonym declared but not used
	// if PQL_select size > 1 - next token must be angleOpen

	if (qsym == QSymbol::qangleOpen) {
		if (PQL_select.size() < 2) {
			qFail("resultCl syntax error");
			return 0;
		}

		if (!fetchQToken(tokens)) {
			qFail("fetchQToken fail in resultCl ");
			return 0;
		}

		// don't want to recurse, iterate!! 
		while (qsym != QSymbol::qangleClose) {
			if (!isSymInPQLSelect(tokens)) {
				qFail("resultCl synonym not declared");
				return 0;
			}

			if (qsym != QSymbol::qcomma) {
				qFail("missing comma in resultCl ");
				return 0;
			}
		}
		return 1;
	}

	if (!isSymInPQLSelect(tokens)) {
		qFail("resultCl synonym not declared");
		return 0;
	}
	return 1;
}

int QueryParser::isSymInPQLSelect(vector<string>& tokens) {
	if (qsym == QSymbol::qsynonym) {
		if (qtokencounter - 1 < 0) {
			qFail("isSymInPQLSelect syntax error");
			return 0;
		}
		map<string, string>::iterator t = PQL_select.find(tokens.at(qtokencounter - 1));
		if (t == PQL_select.end()) {
			// not found
			qFail("isSymInPQLSelect synonym not declared");
			return 0;
		}
		return 1;
	}
	return 0;
}

int QueryParser::declaration(vector<string>& tokens){

	while (1) {
		if(fetchQDeclToken(tokens)){
			if (qsym == QSymbol::qsemicolon)
				continue;
			else if (qsym == QSymbol::qselect)
				return 1;
			else
				break;
		}
		else
			break;				
	} 
	qFail("fail declaration()");
	return 0;
}

int QueryParser::isSuchThat(vector<string>& tokens){

	// if 3 ahead is tail, something is seriously wrong
	/*
	if ((qtokencounter) >= (int)tokens.size()) {
		qFail("qtokencounter bounds violation at isSuchThat()");
		return 0;
	}
	*/

	if(qsearchCl == QClause::cdefaultStart){
		if((tokens.at(qtokencounter+1) == "such") &&
			(tokens.at(qtokencounter+2) == "that")) {
		
			qsearchCl = QClause::csuchthat;
			fetchQToken(tokens); // to move 2 ahead
			fetchQToken(tokens);
			return 1;
		}
		else if(tokens.at(qtokencounter+1) == "pattern"){
			qsearchCl = QClause::cpattern;
			fetchQToken(tokens);
			return 1;
		}
		qFail("suchthat or pattern clause not found");
		return 0;		
	}
	if(qsearchCl == QClause::csuchthat){
		if((tokens.at(qtokencounter+1) == "such") &&
			(tokens.at(qtokencounter+2) == "that")) {
			
			qsearchCl = QClause::csuchthatsuchthat;
			fetchQToken(tokens); // is 2 right? 
			fetchQToken(tokens);
		}
		else if(tokens.at(qtokencounter+1) == "pattern"){
			qsearchCl = QClause::csuchthatpattern;
			fetchQToken(tokens);

		}
		qFail("1st suchthat, 2nd suchthat or pattern clause not found");
		return 0;
	}
	if(qsearchCl == QClause::cpattern){
		if((tokens.at(qtokencounter+1) == "such") &&
			(tokens.at(qtokencounter+2) == "that")) {
		
			qsearchCl = QClause::cpatternsuchthat;
			fetchQToken(tokens);
			fetchQToken(tokens);
			return 1;
		}
		else if(tokens.at(qtokencounter+1) == "pattern"){
			qsearchCl = QClause::cpatternpattern;
			fetchQToken(tokens); 
			return 1;
		}
		qFail("1st pattern, 2nd suchthat or pattern clause not found");
		return 0;
	}
	else {
		qFail("clause missing in isSuchThat()");
		return 0;
	}
}

int QueryParser::suchThat(vector<string>& tokens){

	// suchthat-cl: 'such that' relRef
	fetchQToken(tokens); // fetch token to after that 
	if(!relRef(tokens)){
		qFail("suchThat() failed on relRef()");
		return 0;
	}
	return 1;
}
//#pattern
int QueryParser::pattern(vector<string>& tokens){

	// pattern-cl: 'pattern' syn-assign '('entRef ',' expression-spec ')'
	// syn-assign must be declared as synonym of assignment (design entity 'assign').
	
	qtokencounter++; // check if this is the counter 

	if (!(isSelectValidVariable(tokens, qtokencounter))) {
		qFail("Variable is not assign failing grammar rule");
		return 0;
	}
	fetchQToken(tokens);
	if (!(isSelectValidVariable(tokens, qtokencounter-1))) {
		qFail("Variable is not assign failing grammar rule");
		return 0;
	}
	fetchQToken(tokens);
	if (qsym != QSymbol::qbracketOpen) {
		qFail("not bracket open after select 'x' ");
		return 0;
	}
	
	if (!entRef(tokens)) {
		qFail("failed ent ref in pattern");
		return 0;
	}
	// LHS wildcard
	if (qsym == QSymbol::qwildcard) {
		fetchQToken(tokens);
		if (qsym != QSymbol::qcomma) {
			qFail("no comma after LHS");
			return 0;
		}
		fetchQToken(tokens);
		if (qsym == QSymbol::qwildcard) {
			fetchQToken(tokens);
			if (qsym == QSymbol::qbracketClose) {
				// database insert 1st case LHS wild card RHS wild card (return all assign statements)  1st case

				if (qsearchCl == QClause::cpattern) {
					// either return all assign statements because reference to wiki all pattern statements are assignment statements

					Database::getPattern(qdbResults);

				}

				else if (qsearchCl == QClause::cpatternsuchthat || qsearchCl == QClause::cpatternpattern) {
				
					// plan this 
				}

			}
			else if (qsym == QSymbol::qdblquote) {
				//	database insert 3rd case LHS wild card RHS doublequote HAVE _ INFRONT 3rd case
				// find a new way to input string into pattern instead of using QuoteInput. take note we should use vector of string  
				 // LHS does not matter.
				string treeRHS = infixToPostfix(QuoteInput);
				vector<string> TreeFinalString;
				Database::QueryPatternTree_Only_RHS(TreeFinalString);
				SubTree.construct(treeRHS); // Sub tree
				Node* SubTreeNode = SubTree.s.top(); // leaf that points to a subtree 
				for (int i = 0; i < TreeFinalString.size(); i++) {
					string k = TreeFinalString.at(i);
					Tree.construct(k); // Tree 
					Node* TreeNode = Tree.s.top(); // leaf that points to a tree 
					if (isSubtree(TreeNode, SubTreeNode)) {
						qdbResults.push_back(TreeFinalString.at(i + 1));
					}
					i++; // skip all statement numbers
				}
				return 1;

			}
			else {
				qFail("LHS input wrong");
				return 0;
			}
		}
		else if (qsym == QSymbol::qdblquote) {

			// database insert 2nd case LHS wild card RHS doublequote NO _ inFRONT 2nd case
			// find a new way to input string into pattern instead of using QuoteInput. take note we should use vector of string 
			// LHS does not matter.
			string treeRHS = infixToPostfix(QuoteInput);
			vector<string> TreeFinalString;
			Database::QueryPatternTree_Only_RHS(TreeFinalString);
			SubTree.construct(treeRHS); // Sub tree
			Node* SubTreeNode = SubTree.s.top(); // leaf that points to a subtree 
			for (int i = 0; i < TreeFinalString.size(); i++) {
				string k = TreeFinalString.at(i);
				Tree.construct(k); // Tree 
				Node* TreeNode = Tree.s.top(); // leaf that points to a tree 
				if (isSubtree(TreeNode, SubTreeNode)) {
					qdbResults.push_back(TreeFinalString.at(i + 1));
				}
				i++; // skip all statement numbers
			}
			return 1;
		}
		else {
			qFail("not valid input RHS");
			return 0;
		}

	} // LHS dblquote 
	else if (qsym == QSymbol::qdblquote) {
		fetchQToken(tokens);
		if (qsym != QSymbol::qcomma) {
			qFail("no comma after LHS");
			return 0;
		}
		fetchQToken(tokens);
		QuoteInput = ""; // by time we get here we have to reset quote input so it can be used for the below cases
		if (qsym == QSymbol::qwildcard) {
			fetchQToken(tokens);
			if (qsym == QSymbol::qbracketClose) {
				Database::getPattern_Assign_LHS(qdbResults, getqLHSstring());
			}
			else if (qsym == QSymbol::qdblquote) { // #testing
				//	database insert 6th case LHS syn RHS doublequote HAVE _ INFRONT 
				// find a new way to input string into pattern instead of using QuoteInput. take note we should use vector of string  
				string treeLHS = getqLHSstring();
				string treeRHS = infixToPostfix(QuoteInput);
				vector<string> TreeFinalString;
				Database::QueryPatternTree_With_LHS(TreeFinalString, treeLHS);
				SubTree.construct(treeRHS); // Sub tree
				Node* SubTreeNode = SubTree.s.top(); // leaf that points to a subtree 
				for (int i = 0; i < TreeFinalString.size(); i++) {
					string k = TreeFinalString.at(i);
					Tree.construct(k); // Tree 
					Node* TreeNode = Tree.s.top(); // leaf that points to a tree 
					if (isSubtree(TreeNode, SubTreeNode)) {
						qdbResults.push_back(TreeFinalString.at(i + 1));
					}
					i++;
				}
				return 1;
				
			}
			else {
				qFail("LHS input wrong");
				return 0;
			}
		}
		else if (qsym == QSymbol::qdblquote) {

			// database insert 5th case LHS syn RHS doublequote NO _ INFRONT 
			// find a new way to input string into pattern instead of using QuoteInput. take note we should use vector of string 
			string treeLHS = getqLHSstring();
			string treeRHS = infixToPostfix(QuoteInput);
			vector<string> TreeFinalString;
			Database::QueryPatternTree_With_LHS(TreeFinalString, treeLHS);
			SubTree.construct(treeRHS); // Sub tree
			Node* SubTreeNode = SubTree.s.top(); // leaf that points to a subtree 
			for (int i = 0; i < TreeFinalString.size(); i++) {
				string k = TreeFinalString.at(i);
				Tree.construct(k); // Tree 
				Node* TreeNode = Tree.s.top(); // leaf that points to a tree 
				if (isSubtree(TreeNode, SubTreeNode)) {
					qdbResults.push_back(TreeFinalString.at(i + 1));
				}
				i++;
			}
			return 1;
		}
		else {
			qFail("not valid input RHS");
			return 0;
		}

	
	} // LHS syn
	else if (qsym == QSymbol::qsynonym) {
		fetchQToken(tokens);
		if (qsym != QSymbol::qcomma) {
			qFail("no comma after LHS");
			return 0;
		}
		fetchQToken(tokens);
		if (qsym == QSymbol::qwildcard) {
			fetchQToken(tokens);
			if (qsym == QSymbol::qbracketClose) {
				// database insert 9th case LHS doublequote RHS wildcard 
				if (select_variable_type_LHS == "variable") {
				
					// return all patterns 
					Database::getPattern(qdbResults);
				  // qdbresults will taken in again and modified to new query
				}
				else {
					qFail("Pattern syn not variable , others dont make sense 1");
					return 0;
				
				}
			}
			else if (qsym == QSymbol::qdblquote) {
				//	database insert 8th case LHS doublequote RHS HAVE _ INFRONT 
				// find a new way to input string into pattern instead of using QuoteInput. take note we should use vector of string 

								// database insert 9th case LHS doublequote RHS wildcard 
				if (select_variable_type_LHS == "variable") {

					string treeRHS = infixToPostfix(QuoteInput);
					vector<string> TreeFinalString;
					Database::QueryPatternTree_Only_RHS(TreeFinalString);
					SubTree.construct(treeRHS); // Sub tree
					Node* SubTreeNode = SubTree.s.top(); // leaf that points to a subtree 
					for (int i = 0; i < TreeFinalString.size(); i++) {
						string k = TreeFinalString.at(i);
						Tree.construct(k); // Tree 
						Node* TreeNode = Tree.s.top(); // leaf that points to a tree 
						if (isSubtree(TreeNode, SubTreeNode)) {
							qdbResults.push_back(TreeFinalString.at(i + 1));
						}
						i++; // skip all statement numbers
					}
					return 1;
				}
				else {
					qFail("Pattern syn not variable , others dont make sense 2");
					return 0;

				}

			}
			else {
				qFail("LHS input wrong");
				return 0;
			}
		}
		else if (qsym == QSymbol::qdblquote) {

			if (select_variable_type_LHS == "variable") {
				string treeRHS = infixToPostfix(QuoteInput);
				vector<string> TreeFinalString;
				Database::QueryPatternTree_Only_RHS(TreeFinalString);
				SubTree.construct(treeRHS); // Sub tree
				Node* SubTreeNode = SubTree.s.top(); // leaf that points to a subtree 
				for (int i = 0; i < TreeFinalString.size(); i++) {
					string k = TreeFinalString.at(i);
					Tree.construct(k); // Tree 
					Node* TreeNode = Tree.s.top(); // leaf that points to a tree 
					if (isSubtree(TreeNode, SubTreeNode)) {
						qdbResults.push_back(TreeFinalString.at(i + 1));
					}
					i++; // skip all statement numbers
				}
				return 1;
			}
			else {
				qFail("LHS not variable");
				return 0;
			}
		}
		else {
			qFail("not valid input RHS");
			return 0;
		}

	}

	qFail("pattern() failed");
	return 1;
}

#if 0
int QueryParser::xrelRef(vector<string>& tokens){

	// relRef: ModifiesS | UsesS | Parent | ParentT |

	// Parent: 'Parent' '(' stmtRef ',' stmtRef ')'
	// ParentT: 'Parent*' '(' stmtRef ',' stmtRef ')'
	// ModifiesS: 'Modifies' '(' stmtRef ',' entRef ')'
	// UsesS: 'Uses' '(' stmtRef ',' entRef ')'
	// ModifiesP | UsesP | Calls | CallsT

	if (!fetchQToken(tokens)) {
		qFail("fetchQToken fail - relRef");
		return 0;	
	}

	// PRODUCTION ModifiesS: 'Modifies' '(' stmtRef ',' entRef ')'

	if (qsym == QSymbol::qModifies) {
		if(modifies(tokens)){
			return 1;
		}
	}

	qFail("xrelRef() failed");
	return 0;
}
#endif 

//#stmtref

int QueryParser::stmtRef(vector<string>& tokens){
	// synonym: IDENT
	// stmtRef: synonym | '_' | INTEGER

	// such that (stmtRef, stmt/entRef)
	// stmtRef is LHS of ModifiesS, UsesS, Parent, ParentT
	// stmtRef is RHS of Parent, ParentT

	if(fetchQToken(tokens)){
		if(qsym == QSymbol::qintLineNo){ // ToDo: make qinteger in map and fetchQToken
			if(getLHS()){
				LHStype = qsym;
				storeqLHS(); // NOTE keep everything for SQL string building later
				storeqLHSstring(tokens[qtokencounter-1]);
				set_Select_variable_LHS(tokens);
			}else{
				RHStype = qsym;
				storeqRHS(); 
				storeqRHSstring(tokens[qtokencounter-1]);
				set_Select_variable_RHS(tokens);
			}
			return 1;
		}
		if(qsym == QSymbol::qwildcard){ 
			
			if(getLHS()){
				LHStype = qsym;
				storeqLHS(); // NOTE keep everything for SQL string building later
				storeqLHSstring(tokens[qtokencounter-1]);
				set_Select_variable_LHS(tokens);
			}else{
				RHStype = qsym;
				storeqRHS(); 
				storeqRHSstring(tokens[qtokencounter-1]);
				set_Select_variable_RHS(tokens);
			}
			return 1;
			
		}
		if(isQsymDeclaredSynonym(tokens)){ // need to check 
			// ToDo: so what now that its a synonym?
			if(getLHS()){
				LHStype = qsym;
				storeqLHS(); // NOTE keep everything for SQL string building later
				storeqLHSstring(tokens[qtokencounter-1]);
				set_Select_variable_LHS(tokens);
			}else{
				RHStype = qsym;
				storeqRHS(); 
				storeqRHSstring(tokens[qtokencounter-1]);
				set_Select_variable_RHS(tokens);
			}
			return 1;
		}		
		qFail("fetchQToken fail on stmtRef isQsymDeclaredSynonym");
		return 0;
		
	}
	qFail("fetchQToken fail on stmtRef");
	return 0;
}
// #entref
int QueryParser::entRef(vector<string>& tokens) {
	// synonym: IDENT
	// entRef: synonym | '_' | '"' IDENT '"'

	if (fetchQToken(tokens)) {
		if (qsym == QSymbol::qwildcard) {
			if (qsearchCl == QClause::cpattern || qsearchCl == QClause::cpatternpattern || qsearchCl == QClause::cpatternsuchthat || qsearchCl == QClause::csuchthatpattern) {
				if (getLHS()) {
					storeqLHS();
					storeqLHSstring(tokens[qtokencounter - 1]);
					set_Select_variable_LHS(tokens);
				}
			}
			else if (getLHS()) {
				return 0;
			}
			else {
				RHStype = qsym;
				storeqRHS();
				storeqRHSstring(tokens[qtokencounter - 1]);
				set_Select_variable_RHS(tokens);

			}
			return 1;

		}
		else if (qsym == QSymbol::qsynonym) {
			if (getLHS()) {
				// storeqLHS(); // NOTE keep everything for SQL string building later
				// storeqLHSstring(tokens[qtokencounter-1]);
				if (qsearchCl == QClause::cpatternpattern || qsearchCl == QClause::cpatternsuchthat || qsearchCl == QClause::csuchthatpattern || qsearchCl == QClause::cpattern) {
					if (getLHS()) {
						storeqLHS();
						storeqLHSstring(tokens[qtokencounter - 1]);
						set_Select_variable_LHS(tokens);
					}
					// entRef doesn't appear on LHS
					// in Parent, ParentT, ModifiesT, Modifies, Uses, UsesT
					else {
						qFail("getLHS error in entRef qsynonym");
						return 0;
					}
				}
				else {
					RHStype = qsym;
					storeqRHS();
					storeqRHSstring(tokens[qtokencounter - 1]);
					set_Select_variable_RHS(tokens);
				}
				return 1;
			}
			else if (qsym == QSymbol::qdblquote) { //ToDo: test if fetchQToken can handle
				if (qsearchCl == QClause::cpattern || qsearchCl == QClause::cpatternpattern || qsearchCl == QClause::cpatternsuchthat || qsearchCl == QClause::csuchthatpattern) {
					if (getLHS()) {
						storeqLHS();
						storeqLHSstring(QuoteInput);
					}

				}
				else if (getLHS()) {
					return 0;
				}
				else {
					RHStype = qsym;
					storeqRHS();
					storeqRHSstring(QuoteInput); //  
					set_Select_variable_RHS(tokens);
				}
				return 1;
			}
			qFail("entRef no tokens matched");
			return 0;
		}
		qFail("entRef fetchQtokens failed");
		return 0;
	}
}

int QueryParser::modifiesLsynRsynW(vector<string>& tokens){

	for (auto& t : PQL_select) {
		if (t.first == getqLHSstring()) {
			if (t.second == "assign") {
				if (RHStype == QSymbol::qwildcard) {
					Database::getSynAssign_MAV(qdbResults); // Assume Wildcard to be VAR 
				}
				for (auto& k : PQL_select) {
					if (k.first == getqRHSstring()) {
						if (k.second == "variable") {
							Database::getSynAssign_MAV(qdbResults);																		
						}
					}
				}
			}
			if (t.second == "read") {
				if (RHStype == QSymbol::qwildcard) {
					Database::getSynAssign_MAV(qdbResults); // Assume Wildcard to be VAR |
					return 1;
				}
				for (auto& k : PQL_select) {
					if (k.first == getqRHSstring()) {
						if (k.second == "variable") {
							Database::getSynAssign_MAV(qdbResults);
							return 1;
						}
					}
				}
			}
			if (t.second == "stmt") {
				if (RHStype == QSymbol::qwildcard) {
					Database::getSynStmt_MSV(qdbResults); // Assume Wildcard to be VAR 
					return 1;
				}
				for (auto& k : PQL_select) {
					if (k.first == getqRHSstring()) {
						if (k.second == "variable") {
							Database::getSynStmt_MSV(qdbResults);
							return 1;
						}
					}
				}
			}
			else {
				qFail("modifiesLsynRsynW fail - no valid Modifies combination found");
				return 0; // dont have assign means not inside 
			}
		}
		
	}
	qFail("modifiesLsynRsynW fail - no valid Modifies combination found");
	return 0;

}

int QueryParser::modifiesLsynRdquote(vector<string>& tokens){

	for (auto& t : PQL_select) {
		if (t.first == getqLHSstring()) {
			if (t.second == "assign") {
				Database::getSynAssign_MAV_Varname(qdbResults, QuoteInput);
				return 1;
			}
			else if (t.second == "read") {
				Database::getSynRead_MRV_Varname(qdbResults, QuoteInput);
				return 1;
			}
			else if (t.second == "stmt") {
				Database::getSynStmt_MSV_Varname(qdbResults, QuoteInput);
				return 1;
			}
			qFail("modifiesLsynRdquote no valid combination found");
			return 0;
		}
	}

	qFail("modifiesLsynRdquote fail");
	return 0;
}

int QueryParser::modifies(vector<string>& tokens){

	setLHS(); // qModifiesS LHS = stmtRef
	if (!fetchQToken(tokens)) {
		qFail("fetchQToken failed in modifies()");
		return 0;
	}

	if (qsym != QSymbol::qbracketOpen) {
		qFail("entRef missing bracketOpen");
		return 0;
	}

	if (!stmtRef(tokens)) {
		qFail("stmtRef failed in entRef");
		return 0;
	}

	if (!fetchQToken(tokens)) {	
		qFail("entRef: fetchQToken error after stmtRef");
		return 0;
	}
	if (qsym != QSymbol::qcomma) {
		qFail("entRef: missing comma");
		return 0;
	}
	clearLHS(); // RHS after comma
	if (!entRef(tokens)) {
		qFail("RHS entRef fail in modifies()");
		return 0;
	}
	if (!fetchQToken(tokens)) {
		qFail("entRef: fetchQToken error after RHS entRef");
		return 0;
	}
	if (qsym != QSymbol::qbracketClose) {		
		qFail("entRef: missing bracketClose");
		return 0;
	}
	if (LHStype ==  QSymbol::qsynonym){											
		if (RHStype == QSymbol::qsynonym || RHStype == QSymbol::qwildcard) {
			if(modifiesLsynRsynW(tokens)){
				
				return 1;
			}
		}
		if (RHStype == QSymbol::qdblquote) {
			if(modifiesLsynRdquote(tokens)){
				return 1;
			}
		}

		qFail("Modifies no valid RHS for synonym LHS");
		return 0;
	}
	else if (LHStype == QSymbol::qwildcard){
		if (RHStype == QSymbol::qdblquote) {
			if (select_variable_type == "assign") {
				Database::getSynAssign_MAV_Varname(qdbResults, QuoteInput);
				return 1;
			}
			else if (select_variable_type == "read") {
				Database::getSynRead_MRV_Varname(qdbResults, QuoteInput);
				return 1;
			}
			else if (select_variable_type == "stmt") {
				Database::getSynStmt_MSV_Varname(qdbResults, QuoteInput);
				return 1;
			}
			qFail("Modifies no valid dblquote RHS for wildcard LHS");
			return 0;
		}
		else if(RHStype == QSymbol::qsynonym && select_variable_type_RHS == "variable"){
			if (select_variable_type_LHS == "assign" ) {
				Database::getSynAssign_MAV(qdbResults);
				return 1;
			}
			else if (select_variable_type_LHS == "read" ) {
				Database::getSynRead_MRV(qdbResults);
				return 1;
			}
			else if (select_variable_type_LHS == "stmt" ) {
				Database::getSynStmt_MSV(qdbResults);
				return 1;
			}
			qFail("Modifies no valid synonym/variable RHS for wildcard LHS");
			return 0;
		}
		else if (RHStype == QSymbol::qwildcard && select_variable_type_RHS == "variable") {
			if (select_variable_type_LHS == "assign") {
				Database::getSynAssign_MAV(qdbResults);
				return 1;
			}
			else if (select_variable_type_LHS == "read") {
				Database::getSynRead_MRV(qdbResults);
				return 1;
			}
			else if (select_variable_type_LHS == "stmt") {
				Database::getSynStmt_MSV(qdbResults);
				return 1;
			}
		}
		else {
			return 0;
		}
	}
	else if (LHStype == QSymbol::qintLineNo && select_variable_type_RHS == "variable") {
		if (select_variable_type_LHS == "assign" && (RHStype == QSymbol::qsynonym || RHStype == QSymbol::qwildcard)) {
			Database::getLineNoAssign_MAV(qdbResults, getqLHSstring());
			return 1;
		}
		else if (select_variable_type == "read" && (RHStype == QSymbol::qsynonym || RHStype == QSymbol::qwildcard)) {
			Database::getLineNoRead_MRV(qdbResults, getqLHSstring());
			return 1;
		}
		else if (select_variable_type == "stmt" && (RHStype == QSymbol::qsynonym || RHStype == QSymbol::qwildcard)) {
			Database::getLineNoStmt_MSV(qdbResults, getqLHSstring());
			return 1;
		}
		else {
			return 0;
		}
	}
}

int QueryParser::modifiesP(vector<string>& tokens){

	// COPIED FROM modifies()
	// CHECK algo ok
	setLHS(); // qModifiesP LHS = stmtRef
	if (!fetchQToken(tokens)) {
		qFail("fetchQToken failed in modifies()");
		return 0;
	}

	if (qsym != QSymbol::qbracketOpen) {
		qFail("entRef missing bracketOpen");
		return 0;
	}

	if (!stmtRef(tokens)) {
		qFail("stmtRef failed in entRef");
		return 0;
	}

	if (!fetchQToken(tokens)) {	
		qFail("entRef: fetchQToken error after stmtRef");
		return 0;
	}
	if (qsym != QSymbol::qcomma) {
		qFail("entRef: missing comma");
		return 0;
	}
	clearLHS(); // RHS after comma
	if (!entRef(tokens)) {
		qFail("RHS entRef fail in modifies()");
		return 0;
	}
	if (!fetchQToken(tokens)) {
		qFail("entRef: fetchQToken error after RHS entRef");
		return 0;
	}
	if (qsym != QSymbol::qbracketClose) {		
		qFail("entRef: missing bracketClose");
		return 0;
	}
}

int QueryParser::usesLsyn(vector<string>& tokens){
	
	for (auto& t : PQL_select ) {
		if (t.first == getqLHSstring()) {
			if (t.second == "assign") {// UAV
				if (select_variable_type_RHS == "variable") {
					Database::getSynSyn_UAV_Assign(qdbResults);
					return 1;
				}
				else if (RHStype == QSymbol::qdblquote) {
					Database::getSynCount_UAV_Assign(qdbResults, getqRHSstring()); 
					return 1;
				}
				else if (RHStype == QSymbol::qwildcard) {
					Database::getSynSyn_UAV_Assign(qdbResults); // assume best case
					return 1;
				}
				qFail("usesLsyn() - no matching entity for assign");
				return 0;
			}
			else if (t.second == "print") { //PNV
				if (select_variable_type == "print") {
					Database::getSynSyn_UPV_Print(qdbResults);
					return 1;
				}
				else if (RHStype == QSymbol::qdblquote) {
					Database::getSynCount_UPV_Print(qdbResults, getqRHSstring());
					return 1;
				}
				else if (RHStype == QSymbol::qwildcard) {
					Database::getSynSyn_UPV_Print(qdbResults); // assume best case
					return 1;
				}
				qFail("usesLsyn() - no matching matching entity for print");
				return 0;
			}
			else if (t.second == "stmt") { // UAV + USV
				// add condition for USV 
				if (select_variable_type_RHS == "variable") {
					Database::getSynSyn_MAV_Stmt(qdbResults);
					return 1;
					//
				}
			} // insert here for Uses Procedure V			
			qFail("usesLsyn() - no matching RHSstring");
			return 0;
		}
		qFail("usesLsyn() - no matching LHSstring");
		return 0;
	}
}

int QueryParser::usesLlineno(vector<string>& tokens){

	// UAV 
	if (select_variable_type == "assign" || select_variable_type == "stmt") {
		if (select_variable_type_RHS == "variable"){
			Database::getLineSyn_UAV_Assign(qdbResults);
			return 1;
		}
		qFail("usesLlineno() UAV assign/stmt failed");
		return 0;
	}
	else if (select_variable_type == "variable") {
		if (RHStype == QSymbol::qdblquote) {
			Database::getLineSyn_UAV_Variable(qdbResults, getqRHSstring());
			return 1;
			//count
		}
		qFail("usesLlineno() UAV variable failed");
		return 0;
	}
	else if (RHStype == QSymbol::qwildcard) {
		Database::getLineSyn_UAV_Assign(qdbResults);
		return 1;
	}
	//PNV 
	else if (select_variable_type == "print") {
		if (select_variable_type_RHS == "variable") {
			Database::getLineSyn_UPV_Print(qdbResults,getqRHSstring());
			return 1;
		}
		else if (RHStype == QSymbol::qdblquote) {
			Database::getLineSyn_UPV_Variable(qdbResults, getqRHSstring());
			return 1;
		}
		else if (RHStype == QSymbol::qwildcard) {
			Database::getLineSyn_UPV_Print(qdbResults, getqRHSstring());
			return 1;
		}
		qFail("usesLlineno() UAV variable failed");
		return 0;
	}
	qFail("uses() - usesLlineno failed");
	return 0;
}

int QueryParser::usesLwildcard(vector<string>& tokens){

	// uav
	if (select_variable_type == "assign" || select_variable_type == "stmt") {
		if (select_variable_type_RHS == "variable"){
		Database::getSynSyn_UAV_Assign(qdbResults);
		return 1;
		}
		else if (RHStype == QSymbol::qdblquote) {
			Database::getSynCount_UAV_Assign(qdbResults, getqRHSstring());
			return 1;
		}
		else if (RHStype == QSymbol::qwildcard) {
			Database::getSynSyn_UAV_Assign(qdbResults);
			return 1;
		}
		qFail("usesLwildcard failed - no match for assign/stmt");
		return 0;
	}
	// upv
	else if (select_variable_type == "print") {
		if (select_variable_type_RHS == "variable") {
			Database::getSynSyn_UPV_Print(qdbResults);
			return 1;
		}
		else if (RHStype == QSymbol::qdblquote) {
			Database::getSynCount_UPV_Print(qdbResults, getqRHSstring());
			return 1;
		}
		else if (RHStype == QSymbol::qwildcard) {
			Database::getSynSyn_UPV_Print(qdbResults);
			return 1;
		}
		qFail("usesLwildcard failed - no match for print");
		return 0;
	}
	qFail("usesLwildcard failed - no select var type");
	return 0;
}

int QueryParser::uses(vector<string>& tokens){

	setLHS(); // qUsesS LHS = stmtRef
	if (!fetchQToken(tokens)) {	
		qFail("uses() fetchQToken failed");
		return 0;	
	}
	if (qsym != QSymbol::qbracketOpen) {
		qFail("uses() failed- missing bracketOpen");
		return 0;	
	}
	if (!stmtRef(tokens)) {
		qFail("stmtRef in uses() failed");
		return 0;	
	}

	if (!fetchQToken(tokens)) {
		qFail("uses() fetchQToken after stmtRef failed");
		return 0;	
	}

	if (qsym != QSymbol::qcomma) {
		qFail("uses() comma missing");
		return 0;	
	}

	clearLHS(); // RHS after comma
	if (!entRef(tokens)) {
		qFail("uses() - RHS entRef missing");
		return 0;	
	}

	if (!fetchQToken(tokens)) {
		qFail("uses() - fetchQToken after entRef failed");
		return 0;	
	}

	if (qsym != QSymbol::qbracketClose) {
		qFail("uses() - bracketClose missing");
		return 0;		
	}
	
	if (LHStype == QSymbol::qsynonym) {
		if(usesLsyn(tokens))
			return 1;		
	}
	else if (LHStype == QSymbol::qintLineNo) {
		if(usesLlineno(tokens))
			return 1;				
	}
	else if (LHStype == QSymbol::qwildcard) {
		if(usesLwildcard(tokens))
			return 1;		
	}
	qFail("Use failed");
	return 0;	
}

int QueryParser::usesP(vector<string>& tokens){
	// Uses : 'Uses' '(' entRef ',' entRef ')'

	setLHS();

	if (!fetchQToken(tokens)) {
		qFail("fetchQToken fail in usesP()");
		return 0;
	}
	if (qsym != QSymbol::qbracketOpen) {
		qFail("missing qbracketOpen in usesP()");
		return 0;
	}
	if (!stmtRef(tokens)) {
		qFail("stmtRef failed in usesP()");
		return 0;
	}
	if (!fetchQToken(tokens)) {
		qFail("fetchQToken after stmtRef failed in usesP()");
		return 0;
	}
	if (qsym != QSymbol::qcomma) {
		qFail("missing qcomma in usesP()");
		return 0;
	}
	clearLHS();
	if (!entRef(tokens)) {
		qFail("entRef failed in usesP()");
		return 0;
	}
	if (fetchQToken(tokens)) {
		qFail("fetchQToken after entRef failed in usesP()");
		return 0;
	}
	if (qsym == QSymbol::qbracketClose) {
		qFail("missing qbracketClose in usesP()");
		return 0;
	}

	// start processing LHS and RHS
}

int QueryParser::parent(vector<string>& tokens){

	setLHS();

	if (!fetchQToken(tokens)) {
		qFail("fetchQToken fail in parent()");
		return 0;
	}
	if (qsym != QSymbol::qbracketOpen) {
		qFail("missing qbracketOpen in parent()");
		return 0;
	}
	if (!stmtRef(tokens)) {
		qFail("stmtRef failed in parent()");
		return 0;
	}
	if (!fetchQToken(tokens)) {
		qFail("fetchQToken after stmtRef failed in parent()");
		return 0;
	}
	if (qsym != QSymbol::qcomma) {
		qFail("missing qcomma in parent()");
		return 0;
	}
	clearLHS();
	if (!stmtRef(tokens)) {
		qFail("entRef failed in parent()");
		return 0;
	}
	if (fetchQToken(tokens)) {
		qFail("fetchQToken after entRef failed in parent()");
		return 0;
	}
	if (qsym == QSymbol::qbracketClose) {
		qFail("missing qbracketClose in parent()");
		return 0;
	}
	// do what?

	// call some DB function like this -> Database::getProcedures(qdbResults);
	// ToDo: put for loop here? or back in QueryProcessor::evaluate?
	// for (string databaseResult : databaseResults) {
	// 	output.push_back(qp.getDBresults());
	// }
	// leave this as loop see got issue 
	if (LHStype == QSymbol::qsynonym && RHStype == QSymbol::qsynonym) {
		for (auto& t : PQL_select) {
			if (t.first == getqLHSstring()) {
				if (t.second == "stmt") {
					for (auto& k : PQL_select) {
						if (k.first == getqRHSstring()) {
							if (k.second == "stmt") {
								Database::getSynSyn_Parent(qdbResults);
								return 1;
							}
							if (k.second == "read") {
								Database::getSynSyn_Parent_Read(qdbResults);
								return 1;
							}
							if (k.second == "print") {
								Database::getSynSyn_Parent_Print(qdbResults);
								return 1;
							}
							if (k.second == "assign") {
								Database::getSynSyn_Parent_Assign(qdbResults);
								return 1;
							}
							else {
								return 0;
							}
						}
					}
				}
			}
		}
	}//insert SYN, SYN 1
	else if (LHStype == QSymbol::qsynonym && RHStype == QSymbol::qwildcard ) {
		for (auto& t : PQL_select) {
			if (t.first == getqLHSstring()) {
				if (t.second == "stmt") {
					Database::getSynSyn_Parent(qdbResults);
					return 1;
				}
				else {
					return 0; // we take stmt stmt as best case. so only need to return 1 type 
				}
			}
		}
	} // insert SYN, wildcard 

	else if (LHStype == QSymbol::qsynonym && RHStype == QSymbol::qintLineNo) {
		for (auto& t : PQL_select) {
			if (t.first == getqLHSstring()) {
				if (t.second == "stmt") {
					Database::getSynLine_Parent(qdbResults, getqRHSstring());
					return 1;
				}
				else {
					return 0; // Only stmt make sense in our design 
				}
			}
		}

	}// insert syn,LineNo 3

	else if (LHStype == QSymbol::qintLineNo && RHStype == QSymbol::qsynonym) {
		for (auto& t : PQL_select) {
			if (t.first == getqRHSstring()) {
				if (t.second == "stmt") {
					Database::getLineSyn_Parent(qdbResults, getqLHSstring());
					return 1;
				}
				return 0; // Only stmt make sense in our design 
			}
		}
		// database query need to take in line no getqLHSstring();
	}// insert LineNo,syn 4

	else if (LHStype == QSymbol::qintLineNo && RHStype == QSymbol::qwildcard ) {
		Database::getLineSyn_Parent(qdbResults, getqLHSstring()); // WE ASSUME THAT WILDCARD IS GOING TO GIVE BEST CASE WHICH IS SYN STMT
		return 1;

	}// insert LineNo,wildcard 5

	else if (LHStype == QSymbol::qintLineNo && RHStype == QSymbol::qintLineNo) {

		Database::getLineLine_Parent(qdbResults, getqLHSstring(), getqRHSstring());
		return 1;
		// db will do the query no check required

		// return a boolean expression of a query to check if S2 is a child of S1 
		// check table 
	}// insert LineNo,lineno	6
	else if (LHStype == QSymbol::qwildcard  && RHStype == QSymbol::qsynonym) {
		for (auto& t : PQL_select) {
			if (t.first == getqRHSstring()) {
				if (t.second == "stmt") {
					Database::getWildSyn_Parent(qdbResults);
					return 1;
				}
				return 0;
			}
		}
	}
	else if (LHStype == QSymbol::qwildcard && RHStype == QSymbol::qwildcard ) {
			
		Database::getWildSyn_Parent(qdbResults);
		return 1;
	}
	else if (LHStype == QSymbol::qwildcard  && RHStype == QSymbol::qintLineNo) {
			
		Database::getWildLine_Parent(qdbResults, getqRHSstring());
		return 1; 
	}
	qFail("combination not handled in parent()");
	return 0;

}

int QueryParser::parentT(vector<string>& tokens){

	setLHS();

	if (!fetchQToken(tokens)) {
		qFail("fetchQToken fail in parentT()");
		return 0;
	}
	if (qsym != QSymbol::qbracketOpen) {
		qFail("missing qbracketOpen in parentT()");
		return 0;
	}
	if (!stmtRef(tokens)) {
		qFail("stmtRef failed in parentT()");
		return 0;
	}
	if (!fetchQToken(tokens)) {
		qFail("fetchQToken after stmtRef failed in parentT()");
		return 0;
	}
	if (qsym != QSymbol::qcomma) {
		qFail("missing qcomma in parentT()");
		return 0;
	}

	clearLHS();

	if (!stmtRef(tokens)) {
		qFail("entRef failed in parentT()");
		return 0;
	}

	if (!fetchQToken(tokens)) {
		qFail("fetchQToken after entRef failed in parentT()");
		return 0;
	}

	if (qsym != QSymbol::qbracketClose) {
		qFail("missing qcomma in parentT()");
		return 0;
	}		
}
//#next
int QueryParser::next(vector<string>& tokens){

	// Next : 'Next' '(' stmtRef ',' stmtRef ')'

	setLHS();

	if (!fetchQToken(tokens)) {
		qFail("fetchQToken fail in next()");
		return 0;
	}
	if (qsym != QSymbol::qbracketOpen) {
		qFail("missing qbracketOpen in next()");
		return 0;
	}
	if (!stmtRef(tokens)) {
		qFail("stmtRef failed in next()");
		return 0;
	}
	if (!fetchQToken(tokens)) {
		qFail("fetchQToken after stmtRef failed in next()");
		return 0;
	}
	if (qsym != QSymbol::qcomma) {
		qFail("missing qcomma in next()");
		return 0;
	}
	clearLHS();
	if (!stmtRef(tokens)) {
		qFail("entRef failed in next()");
		return 0;
	}

	if (qsym == QSymbol::qbracketClose) {
		qFail("missing qbracketClose in next()");
		return 0;
	}

	// variable, constant, procedure
	if (LHStype == QSymbol::qsynonym ) {
		if (select_variable_type_LHS == "variable" || select_variable_type_LHS == "constant" || select_variable_type_LHS == "procedure") {
			qFail("next cannot be variable constant and procedure");
			return 0;
		}
		if (RHStype == QSymbol::qsynonym) {
			if (select_variable_type_RHS == "variable" || select_variable_type_LHS == "constant" || select_variable_type_LHS == "procedure") {
				qFail("next cannot be variable constant and procedure");
				return 0;
			}// stmt LHS || stmt,read,print,assign
			else if (select_variable_type_LHS == "stmt" && (select_variable_type_RHS == "stmt" || select_variable_type_RHS == "read" || select_variable_type_RHS == "print" || select_variable_type_RHS == "assign" )) {
				Database::getNext_SYN_SYN_stmt(qdbResults);
				if (!qdbResults.empty()) {
					// if qdb is 0 
					qdbResults.clear();
					qdbResults.push_back("true");
					return 1;
				}
				else {
					qdbResults.clear();
					qdbResults.push_back("false");
					return 1;
				}
			
			}// stmt LHS || while if 
			else if (select_variable_type_LHS == "stmt" && (select_variable_type_RHS == "while" || select_variable_type_RHS == "if")) {

				Database::getNext_SYN_SYN_stmt_loop(qdbResults);
				if (!qdbResults.empty()) {
					// if qdb is 0 
					qdbResults.clear();
					qdbResults.push_back("true");
					return 1;
				}
				else {
					qdbResults.clear();
					qdbResults.push_back("false");
					return 1;
				}

			}
			// read LHS || stmt, read , print, assign
			else if (select_variable_type_LHS == "read" && (select_variable_type_RHS == "stmt" || select_variable_type_RHS == "read" || select_variable_type_RHS == "print" || select_variable_type_RHS == "assign")) {
				Database::getRead(qdbResults);
				if (qdbResults.empty()) {
					return 1;
				}
				// increment the qdbresults which is all the read lines to +1 to check the next statement 
				string linecheck;
				int cast_linecheck;
				vector<int> qdbResultsNew;
				for (auto& t : qdbResults) {
				
					linecheck = t.at(0);
					cast_linecheck = stoi(linecheck);
					cast_linecheck++;
					qdbResultsNew.push_back(cast_linecheck);

				}
				qdbResults.clear();
				Database::getNext_SYN_SYN_read(qdbResultsNew,qdbResults);
				if (!qdbResults.empty()) {
					// if qdb is 0 
					qdbResults.clear();
					qdbResults.push_back("true");
					return 1;
				}
				else {
					qdbResults.clear();
					qdbResults.push_back("false");
					return 1;
				}



			}// read LHS || while if 
			else if (select_variable_type_LHS == "read" && (select_variable_type_RHS == "while" || select_variable_type_RHS == "if")) {
				Database::getRead(qdbResults);
				if (qdbResults.empty()) {
					return 1;
				}
				// increment the qdbresults which is all the read lines to +1 to check the next statement 
				string linecheck;
				int cast_linecheck;
				vector<int> qdbResultsNew;
				for (auto& t : qdbResults) {

					linecheck = t.at(0);
					cast_linecheck = stoi(linecheck);
					cast_linecheck++;
					qdbResultsNew.push_back(cast_linecheck);

				}
				qdbResults.clear();
				if (select_variable_type_RHS == "while") {
					Database::getNext_SYN_SYN_read_while(qdbResultsNew, qdbResults);
				}
				else if (select_variable_type_RHS == "if") {
					Database::getNext_SYN_SYN_read_if(qdbResultsNew, qdbResults);
				}
				if (!qdbResults.empty()) {
					// if qdb is 0 
					qdbResults.clear();
					qdbResults.push_back("true");
					return 1;
				}
				else {
					qdbResults.clear();
					qdbResults.push_back("false");
					return 1;
				}

			}




			//  print LHS  || stmt, read , print, assign
			else if (select_variable_type_LHS == "print" && (select_variable_type_RHS == "stmt" || select_variable_type_RHS == "read" || select_variable_type_RHS == "print" || select_variable_type_RHS == "assign")) {
				Database::getPrint(qdbResults);
				if (qdbResults.empty()) {
					return 1;
				}
				// increment the qdbresults which is all the read lines to +1 to check the next statement 
				string linecheck;
				int cast_linecheck;
				vector<int> qdbResultsNew;
				for (auto& t : qdbResults) {

					linecheck = t.at(0);
					cast_linecheck = stoi(linecheck);
					cast_linecheck++;
					qdbResultsNew.push_back(cast_linecheck);

				}
				qdbResults.clear();
				Database::getNext_SYN_SYN_read(qdbResultsNew, qdbResults);
				if (!qdbResults.empty()) {
					// if qdb is 0 
					qdbResults.clear();
					qdbResults.push_back("true");
					return 1;
				}
				else {
					qdbResults.clear();
					qdbResults.push_back("false");
					return 1;
				}

			}// assign LHS || while if 
			else if (select_variable_type_LHS == "print" && (select_variable_type_RHS == "while" || select_variable_type_RHS == "if")) {
			Database::getPrint(qdbResults);
			if (qdbResults.empty()) {
				return 1;
			}

			// increment the qdbresults which is all the read lines to +1 to check the next statement 
			string linecheck;
			int cast_linecheck;
			vector<int> qdbResultsNew;
			for (auto& t : qdbResults) {

				linecheck = t.at(0);
				cast_linecheck = stoi(linecheck);
				cast_linecheck++;
				qdbResultsNew.push_back(cast_linecheck);

			}
			qdbResults.clear();
			if (select_variable_type_RHS == "while") {
				Database::getNext_SYN_SYN_read_while(qdbResultsNew, qdbResults);
			}
			else if (select_variable_type_RHS == "if") {
				Database::getNext_SYN_SYN_read_if(qdbResultsNew, qdbResults);
			}
			if (!qdbResults.empty()) {
				// if qdb is 0 
				qdbResults.clear();
				qdbResults.push_back("true");
				return 1;
			}
			else {
				qdbResults.clear();
				qdbResults.push_back("false");
				return 1;
			}
				
			}



			else if (select_variable_type_LHS == "assign" && (select_variable_type_RHS == "stmt" || select_variable_type_RHS == "read" || select_variable_type_RHS == "print" || select_variable_type_RHS == "assign")) {
			Database::getAssign(qdbResults);
			if (qdbResults.empty()) {
				return 1;
			}
			// increment the qdbresults which is all the read lines to +1 to check the next statement 
			string linecheck;
			int cast_linecheck;
			vector<int> qdbResultsNew;
			for (auto& t : qdbResults) {

				linecheck = t.at(0);
				cast_linecheck = stoi(linecheck);
				cast_linecheck++;
				qdbResultsNew.push_back(cast_linecheck);

			}
			qdbResults.clear();
			Database::getNext_SYN_SYN_read(qdbResultsNew, qdbResults);
			if (!qdbResults.empty()) {
				// if qdb is 0 
				qdbResults.clear();
				qdbResults.push_back("true");
				return 1;
			}
			else {
				qdbResults.clear();
				qdbResults.push_back("false");
				return 1;
			}

			}// assign LHS || while if 
			else if (select_variable_type_LHS == "assign" && (select_variable_type_RHS == "while" || select_variable_type_RHS == "if")) {

			Database::getAssign(qdbResults);
			if (qdbResults.empty()) {
				return 1;
			}
			// increment the qdbresults which is all the read lines to +1 to check the next statement 
			string linecheck;
			int cast_linecheck;
			vector<int> qdbResultsNew;
			for (auto& t : qdbResults) {

				linecheck = t.at(0);
				cast_linecheck = stoi(linecheck);
				cast_linecheck++;
				qdbResultsNew.push_back(cast_linecheck);

			}
			qdbResults.clear();
			if (select_variable_type_RHS == "while") {
				Database::getNext_SYN_SYN_read_while(qdbResultsNew, qdbResults);
			}
			else if (select_variable_type_RHS == "if") {
				Database::getNext_SYN_SYN_read_if(qdbResultsNew, qdbResults);
			}
			if (!qdbResults.empty()) {
				// if qdb is 0 
				qdbResults.clear();
				qdbResults.push_back("true");
				return 1;
			}
			else {
				qdbResults.clear();
				qdbResults.push_back("false");
				return 1;
			}

			}



			//while LHS  || stmt, read , print, assign
			else if (select_variable_type_LHS == "while" && (select_variable_type_RHS == "stmt" || select_variable_type_RHS == "read" || select_variable_type_RHS == "print" || select_variable_type_RHS == "assign")) {
			Database::getWhile(qdbResults);
			// increment the qdbresults which is all the read lines to +1 to check the next statement 
			string linecheck;
			int cast_linecheck;
			vector<int> qdbResultsNew;
			for (auto& t : qdbResults) {

				linecheck = t.at(0);
				cast_linecheck = stoi(linecheck);
				cast_linecheck++;
				qdbResultsNew.push_back(cast_linecheck);

			}
			qdbResults.clear();
			Database::getNext_SYN_SYN_while_read(qdbResultsNew, qdbResults);
			if (qdbResults.empty()) {
				return 1;
			}

			if (select_variable_type_RHS == "read") {
				string input = qdbResults.at(0);
				qdbResults.clear();
				Database::CheckRead(qdbResults,input);
			}
			else if (select_variable_type_RHS == "print") {
				string input = qdbResults.at(0);
				qdbResults.clear();
				Database::CheckPrint(qdbResults, input);
			}
			else if (select_variable_type_RHS == "assign") {
				string input = qdbResults.at(0);
				qdbResults.clear();
				Database::CheckAssign(qdbResults, input);

			}
			if (!qdbResults.empty()) {
				// if qdb is 0 
				qdbResults.clear();
				qdbResults.push_back("true");
				return 1;
			}
			else {
				qdbResults.clear();
				qdbResults.push_back("false");
				return 1;
			}

			

			}// while LHS || while if 
			else if (select_variable_type_LHS == "while" && (select_variable_type_RHS == "while" || select_variable_type_RHS == "if")) {

			Database::getWhile(qdbResults);
			// increment the qdbresults which is all the read lines to +1 to check the next statement 
			string linecheck;
			int cast_linecheck;
			vector<int> qdbResultsNew;
			for (auto& t : qdbResults) {

				linecheck = t.at(0);
				cast_linecheck = stoi(linecheck);
				cast_linecheck++;
				qdbResultsNew.push_back(cast_linecheck);

			}
			qdbResults.clear();
			if (select_variable_type_RHS == "while") {
				Database::getNext_SYN_SYN_read_while(qdbResultsNew, qdbResults);
			}
			else if (select_variable_type_RHS == "if") {
				Database::getNext_SYN_SYN_read_if(qdbResultsNew, qdbResults);
			}
			if (!qdbResults.empty()) {
				// if qdb is 0 
				qdbResults.clear();
				qdbResults.push_back("true");
				return 1;
			}
			else {
				qdbResults.clear();
				qdbResults.push_back("false");
				return 1;
			}


			}


			// if LHS  || stmt, read , print, assign
			else if (select_variable_type_LHS == "if" && (select_variable_type_RHS == "stmt" || select_variable_type_RHS == "read" || select_variable_type_RHS == "print" || select_variable_type_RHS == "assign")) {
			Database::getIf(qdbResults);
			// increment the qdbresults which is all the read lines to +1 to check the next statement 
			string linecheck;
			int cast_linecheck;
			vector<int> qdbResultsNew;
			for (auto& t : qdbResults) {

				linecheck = t.at(0);
				cast_linecheck = stoi(linecheck);
				cast_linecheck++;
				qdbResultsNew.push_back(cast_linecheck);

			}
			qdbResults.clear();
			Database::getNext_SYN_SYN_while_read(qdbResultsNew, qdbResults);

			if (select_variable_type_RHS == "read") {
				string input = qdbResults.at(0);
				qdbResults.clear();
				Database::CheckRead(qdbResults, input);
			}
			else if (select_variable_type_RHS == "print") {
				string input = qdbResults.at(0);
				qdbResults.clear();
				Database::CheckPrint(qdbResults, input);
			}
			else if (select_variable_type_RHS == "assign") {
				string input = qdbResults.at(0);
				qdbResults.clear();
				Database::CheckAssign(qdbResults, input);

			}
			if (!qdbResults.empty()) {
				// if qdb is 0 
				qdbResults.clear();
				qdbResults.push_back("true");
				return 1;
			}
			else {
				qdbResults.clear();
				qdbResults.push_back("false");
				return 1;
			}


			}// if LHS || while if 
			else if (select_variable_type_LHS == "if" && (select_variable_type_RHS == "while" || select_variable_type_RHS == "if")) {

			Database::getIf(qdbResults);
			// increment the qdbresults which is all the read lines to +1 to check the next statement 
			string linecheck;
			int cast_linecheck;
			vector<int> qdbResultsNew;
			for (auto& t : qdbResults) {

				linecheck = t.at(0);
				cast_linecheck = stoi(linecheck);
				cast_linecheck++;
				qdbResultsNew.push_back(cast_linecheck);

			}
			qdbResults.clear();
			if (select_variable_type_RHS == "while") {
				Database::getNext_SYN_SYN_read_while(qdbResultsNew, qdbResults);
			}
			else if (select_variable_type_RHS == "if") {
				Database::getNext_SYN_SYN_read_if(qdbResultsNew, qdbResults);
			}
			if (!qdbResults.empty()) {
				// if qdb is 0 
				qdbResults.clear();
				qdbResults.push_back("true");
				return 1;
			}
			else {
				qdbResults.clear();
				qdbResults.push_back("false");
				return 1;
			}


			}


		}
		if (RHStype == QSymbol::qwildcard) { //
			Database::getNext_SYN_SYN_stmt(qdbResults);
			if (!qdbResults.empty()) {
				// if qdb is 0 
				qdbResults.clear();
				qdbResults.push_back("true");
				return 1;
			}
			else {
				qdbResults.clear();
				qdbResults.push_back("false");
				return 1;
			}
		
		
		}
		if (RHStype == QSymbol::qintLineNo) { // all print,assign,read,if,while are all in stmt table 
			//stmt 
			if (select_variable_type_LHS == "stmt") {
				string linenumberLHS = getqRHSstring(); 
				int cast_linenumber = stoi(linenumberLHS);
				cast_linenumber--;
				Database::CheckStmt(qdbResults,to_string(cast_linenumber));
				if (!qdbResults.empty()) {
					// if qdb is 0 
					qdbResults.clear();
					qdbResults.push_back("true");
					return 1;
				}
				else {
					qdbResults.clear();
					qdbResults.push_back("false");
					return 1;
				}
			}

			else if (select_variable_type_LHS == "read" || select_variable_type_LHS == "assign" || select_variable_type_LHS == "print") {
				string linenumberLHS = getqRHSstring();
				int cast_linenumber = stoi(linenumberLHS);
				cast_linenumber--;

				if (select_variable_type_LHS == "read") {
					Database::CheckRead(qdbResults, to_string(cast_linenumber));
				}
				else if (select_variable_type_LHS == "assign") {
					Database::CheckAssign(qdbResults, to_string(cast_linenumber));
				}
				else if (select_variable_type_LHS == "print") {
					Database::CheckPrint(qdbResults, to_string(cast_linenumber));
				}
	
				if (!qdbResults.empty()) {
					// if qdb is 0 
					qdbResults.clear();
					qdbResults.push_back("true");
					return 1;
				}
				else {
					qdbResults.clear();
					qdbResults.push_back("false");
					return 1;
				}
			}
			//read assign print 
		
			else if (select_variable_type_LHS == "while" || select_variable_type_LHS == "if") {
				string linenumberLHS = getqRHSstring();
				int cast_linenumber = stoi(linenumberLHS);
				cast_linenumber--;
				string stringcheck;
				int numbercheck;
				if (select_variable_type_LHS == "while") {
					Database::CheckWhile(qdbResults, to_string(cast_linenumber));
					for (auto& t : qdbResults) {
						stringcheck = t.at(0);
						numbercheck = stoi(stringcheck);
					}
					if (numbercheck) {
					
						qdbResults.clear();
						qdbResults.push_back("true");
						return 1;
					}
					else {
						qdbResults.clear();
						qdbResults.push_back("false");
						return 1;
					}
				}
				else if (select_variable_type_LHS == "if") {
					Database::CheckIf(qdbResults, to_string(cast_linenumber));
					for (auto& t : qdbResults) {
						stringcheck = t.at(0);
						numbercheck = stoi(stringcheck);
					}
					if (numbercheck) {

						qdbResults.clear();
						qdbResults.push_back("true");
						return 1;
					}
					else {
						qdbResults.clear();
						qdbResults.push_back("false");
						return 1;
					}
				}
			}
			//while if 
			else {
				return 1;
			}

		}
		else {
			qFail("RHS does not match stmtref");
			return 0;
		}
	}

	else if (LHStype == QSymbol::qwildcard){
		if (RHStype == QSymbol::qsynonym) {
			
			if (select_variable_type_RHS == "stmt") {
				Database::getStatement(qdbResults);
				if (qdbResults.empty()) {
					qdbResults.push_back("false");
					return 1;
				}
				string lastelement = qdbResults.back();
				int lastelement_cast = stoi(lastelement);
				lastelement_cast--;

				qdbResults.clear();
				Database::CheckStmt(qdbResults, to_string(lastelement_cast));
				if (!qdbResults.empty()) {
					// if qdb is 0 
					qdbResults.clear();
					qdbResults.push_back("true");
					return 1;
				}
				else {
					qdbResults.clear();
					qdbResults.push_back("false");
					return 1;
				}
			}
			else if (select_variable_type_RHS == "read") {
				Database::getRead(qdbResults);
				if (qdbResults.empty()) {
					qdbResults.push_back("false");
					return 1;
				}
				string lastelement = qdbResults.back();
				int lastelement_cast = stoi(lastelement);
				lastelement_cast--;

				qdbResults.clear();
				Database::CheckStmt(qdbResults, to_string(lastelement_cast));
				if (!qdbResults.empty()) {
					// if qdb is 0 
					qdbResults.clear();
					qdbResults.push_back("true");
					return 1;
				}
				else {
					qdbResults.clear();
					qdbResults.push_back("false");
					return 1;
				}
			}
			
			else if (select_variable_type_RHS == "print") {
				Database::getPrint(qdbResults);
				if (qdbResults.empty()) {
					qdbResults.push_back("false");
					return 1;
				}
				string lastelement = qdbResults.back();
				int lastelement_cast = stoi(lastelement);
				lastelement_cast--;

				qdbResults.clear();
				Database::CheckStmt(qdbResults, to_string(lastelement_cast));
				if (!qdbResults.empty()) {
					// if qdb is 0 
					qdbResults.clear();
					qdbResults.push_back("true");
					return 1;
				}
				else {
					qdbResults.clear();
					qdbResults.push_back("false");
					return 1;
				}
			
			}
			else if (select_variable_type_RHS == "assign") {
				Database::getAssign(qdbResults);
				if (qdbResults.empty()) {
					qdbResults.push_back("false");
					return 1;
				}
				string lastelement = qdbResults.back();
				int lastelement_cast = stoi(lastelement);
				lastelement_cast--;

				qdbResults.clear();
				Database::CheckStmt(qdbResults, to_string(lastelement_cast));
				if (!qdbResults.empty()) {
					// if qdb is 0 
					qdbResults.clear();
					qdbResults.push_back("true");
					return 1;
				}
				else {
					qdbResults.clear();
					qdbResults.push_back("false");
					return 1;
				}
			
			}
			else if (select_variable_type_RHS == "while") {
				Database::getWhile(qdbResults);
				if (qdbResults.empty()) {
					qdbResults.push_back("false");
					return 1;
				}
				string lastelement = qdbResults.back();
				int lastelement_cast = stoi(lastelement);
				lastelement_cast--;

				qdbResults.clear();
				Database::CheckStmt_LHS_WildCard(qdbResults, to_string(lastelement_cast));
				if (!qdbResults.empty()) {
					// if qdb is 0 
					qdbResults.clear();
					qdbResults.push_back("true");
					return 1;
				}
				else {
					qdbResults.clear();
					qdbResults.push_back("false");
					return 1;
				}
			
			
			
			}
			else if (select_variable_type_RHS == "if") {
			Database::getIf(qdbResults);
			if (qdbResults.empty()) {
				qdbResults.push_back("false");
				return 1;
			}
			string lastelement = qdbResults.back();
			int lastelement_cast = stoi(lastelement);
			lastelement_cast--;

			qdbResults.clear();
			Database::CheckStmt_LHS_WildCard(qdbResults, to_string(lastelement_cast));
			if (!qdbResults.empty()) {
				// if qdb is 0 
				qdbResults.clear();
				qdbResults.push_back("true");
				return 1;
			}
			else {
				qdbResults.clear();
				qdbResults.push_back("false");
				return 1;
			}

			}
			else {
				return 1;
			}
		

		}
		if (RHStype == QSymbol::qwildcard) {
		
			Database::getStatement(qdbResults);
			if (!qdbResults.empty()) {
				// if qdb is 0 
				qdbResults.clear();
				qdbResults.push_back("true");
				return 1;
			}
			else {
				qdbResults.clear();
				qdbResults.push_back("false");
				return 1;
			}
		}
		if (RHStype == QSymbol::qintLineNo) {
		
			string check = getqRHSstring();
			int convert; 
			convert = stoi(check);
			convert--;
			Database::CheckStmt(qdbResults, to_string(convert));
			if (!qdbResults.empty()) {
				// if qdb is 0 
				qdbResults.clear();
				qdbResults.push_back("true");
				return 1;
			}
			else {
				qdbResults.clear();
				qdbResults.push_back("false");
				return 1;
			}
		
		}
		else {
			qFail("RHS does not match stmtref");
			return 0;
		}
	}

	else if (LHStype == QSymbol::qintLineNo) {
		if (RHStype == QSymbol::qsynonym) {
		
			Database::CheckStmt(qdbResults, getqLHSstring());
			if (!qdbResults.empty()) {
				// if qdb is 0 
				qdbResults.clear();
				qdbResults.push_back("true");
				return 1;
			}
			else {
				qdbResults.clear();
				qdbResults.push_back("false");
				return 1;
			}


		}
		if (RHStype == QSymbol::qwildcard) {
			Database::CheckStmt(qdbResults, getqLHSstring());
			if (!qdbResults.empty()) {
				// if qdb is 0 
				qdbResults.clear();
				qdbResults.push_back("true");
				return 1;
			}
			else {
				qdbResults.clear();
				qdbResults.push_back("false");
				return 1;
			}
		
		}
		if (RHStype == QSymbol::qintLineNo) {
		
			int lhs; 
			string lhs_lineno = getqLHSstring();
			int rhs;
			string rhs_lineno = getqRHSstring();
			rhs = stoi(rhs_lineno);
			lhs = stoi(lhs_lineno);

			if (lhs + 1 == rhs) {
				Database::CheckStmt(qdbResults, rhs_lineno);
				if (!qdbResults.empty()) {
					qdbResults.clear();
					Database::CheckStmt(qdbResults, lhs_lineno);
					if (!qdbResults.empty()) {
						qdbResults.clear();
						qdbResults.push_back("true");
						return 1;
					}
					else {
						qdbResults.clear();
						qdbResults.push_back("false");
						return 1;
					
					}
				}
				else {
					qdbResults.clear();
					qdbResults.push_back("false");
					return 1;

				}
			}
			else {
				qdbResults.clear();
				qdbResults.push_back("false");
				return 1;
			
			}
		
		
		
		}
		else {
			qFail("RHS does not match stmtref");
			return 0;
		}
	}

	else {
		qFail("qsym is not part of stmtref");
		return 0;
	}



}

int QueryParser::nextT(vector<string>& tokens){
	// Next : 'Next' '(' stmtRef ',' stmtRef ')'

	setLHS();

	if (!fetchQToken(tokens)) {
		qFail("fetchQToken fail in nextT()");
		return 0;
	}
	if (qsym != QSymbol::qbracketOpen) {
		qFail("missing qbracketOpen in nextT()");
		return 0;
	}
	if (!stmtRef(tokens)) {
		qFail("stmtRef failed in nextT()");
		return 0;
	}
	if (!fetchQToken(tokens)) {
		qFail("fetchQToken after stmtRef failed in nextT()");
		return 0;
	}
	if (qsym != QSymbol::qcomma) {
		qFail("missing qcomma in nextT()");
		return 0;
	}
	clearLHS();
	if (!stmtRef(tokens)) {
		qFail("entRef failed in nextT()");
		return 0;
	}
	if (fetchQToken(tokens)) {
		qFail("fetchQToken after entRef failed in nextT()");
		return 0;
	}
	if (qsym == QSymbol::qbracketClose) {
		qFail("missing qbracketClose in nextT()");
		return 0;
	}

	// start processing LHS and RHS
}

int QueryParser::calls(vector<string>& tokens){

	// Calls : 'Calls' '(' entRef ',' entRef ')'

	setLHS();

	if (!fetchQToken(tokens)) {
		qFail("fetchQToken fail in calls()");
		return 0;
	}
	if (qsym != QSymbol::qbracketOpen) {
		qFail("missing qbracketOpen in calls()");
		return 0;
	}
	if (!entRef(tokens)) {
		qFail("stmtRef failed in calls()");
		return 0;
	}
	if (!fetchQToken(tokens)) {
		qFail("fetchQToken after stmtRef failed in calls()");
		return 0;
	}
	if (qsym != QSymbol::qcomma) {
		qFail("missing qcomma in calls()");
		return 0;
	}
	clearLHS();
	if (!entRef(tokens)) {
		qFail("entRef failed in calls()");
		return 0;
	}
	if (fetchQToken(tokens)) {
		qFail("fetchQToken after entRef failed in calls()");
		return 0;
	}
	if (qsym == QSymbol::qbracketClose) {
		qFail("missing qbracketClose in calls()");
		return 0;
	}

	// start processing LHS and RHS
}

int QueryParser::callsT(vector<string>& tokens){

	// CallsT : 'CallsT' '(' entRef ',' entRef ')'

	setLHS();

	if (!fetchQToken(tokens)) {
		qFail("fetchQToken fail in callsT()");
		return 0;
	}
	if (qsym != QSymbol::qbracketOpen) {
		qFail("missing qbracketOpen in callsT()");
		return 0;
	}
	if (!entRef(tokens)) {
		qFail("stmtRef failed in callsT()");
		return 0;
	}
	if (!fetchQToken(tokens)) {
		qFail("fetchQToken after stmtRef failed in callsT()");
		return 0;
	}
	if (qsym != QSymbol::qcomma) {
		qFail("missing qcomma in callsT()");
		return 0;
	}
	clearLHS();
	if (!entRef(tokens)) {
		qFail("entRef failed in callsT()");
		return 0;
	}
	if (fetchQToken(tokens)) {
		qFail("fetchQToken after entRef failed in callsT()");
		return 0;
	}
	if (qsym == QSymbol::qbracketClose) {
		qFail("missing qbracketClose in callsT()");
		return 0;
	}

	// start processing LHS and RHS
}

int QueryParser::relRef(vector<string>& tokens) {

	// suchthat-cl: 'such that' relRef
	// relRef: ModifiesS | UsesS | Parent | ParentT |
	// Parent: 'Parent' '(' stmtRef ',' stmtRef ')'
	// ParentT: 'Parent*' '(' stmtRef ',' stmtRef ')'

	if (!fetchQToken(tokens)) {
		qFail("fetchQToken fail - relRef");
		return 0;	
	}

	if (qsym == QSymbol::qModifies) {
		// PRODUCTION ModifiesS: 'Modifies' '(' stmtRef ',' entRef ')'
		if(modifies(tokens)){
			return 1;
		}

#if 0 // moved to modifies()		
		setLHS(); // qModifiesS LHS = stmtRef
		if (fetchQToken(tokens)) {
			if (qsym == QSymbol::qbracketOpen) {
				if (stmtRef(tokens)) {
					if (fetchQToken(tokens)) {
						if (qsym == QSymbol::qcomma) {
							clearLHS(); // RHS after comma
							if (entRef(tokens)) {
								if (fetchQToken(tokens)) {
									if (qsym == QSymbol::qbracketClose) {		
										if(LHStype ==  QSymbol::qsynonym){											
											if (RHStype == QSymbol::qsynonym || RHStype == QSymbol::qwildcard) {
												for (auto& t : PQL_select) {
													if (t.first == getqLHSstring()) {
														if (t.second == "assign") {
															if (RHStype == QSymbol::qwildcard) {
																Database::getSynAssign_MAV(qdbResults); // Assume Wildcard to be VAR 
															}
															for (auto& k : PQL_select) {
																if (k.first == getqRHSstring()) {
																	if (k.second == "variable") {
																		Database::getSynAssign_MAV(qdbResults);																		
																	}
																}
															}
														}
														if (t.second == "read") {
															if (RHStype == QSymbol::qwildcard) {
																Database::getSynAssign_MAV(qdbResults); // Assume Wildcard to be VAR |
																return 1;
															}
															for (auto& k : PQL_select) {
																if (k.first == getqRHSstring()) {
																	if (k.second == "variable") {
																		Database::getSynAssign_MAV(qdbResults);
																		return 1;
																	}
																}
															}
														}
														if (t.second == "stmt") {
															if (RHStype == QSymbol::qwildcard) {
																Database::getSynStmt_MSV(qdbResults); // Assume Wildcard to be VAR 
																return 1;
															}
															for (auto& k : PQL_select) {
																if (k.first == getqRHSstring()) {
																	if (k.second == "variable") {
																		Database::getSynStmt_MSV(qdbResults);
																		return 1;
																	}
																}
															}
														}
														else {
															return 0; // dont have assign means not inside 
														}
													}
													
												}
											}
											if (RHStype == QSymbol::qdblquote) {
												for (auto& t : PQL_select) {
													if (t.first == getqLHSstring()) {
														if (t.second == "assign") {
															Database::getSynAssign_MAV_Varname(qdbResults, QuoteInput);
															return 1;
														}
														else if (t.second == "read") {
															Database::getSynRead_MRV_Varname(qdbResults, QuoteInput);
															return 1;
														}
														else if (t.second == "stmt") {
															Database::getSynStmt_MSV_Varname(qdbResults, QuoteInput);
															return 1;
														}
														else {
															return 0;
														}

													}
												}
											}
											else {
												return 0;
											}
											
										}
										else if (LHStype == QSymbol::qwildcard){
											if (RHStype == QSymbol::qdblquote) {
												if (select_variable_type == "assign") {
													Database::getSynAssign_MAV_Varname(qdbResults, QuoteInput);
													return 1;
												}
												else if (select_variable_type == "read") {
													Database::getSynRead_MRV_Varname(qdbResults, QuoteInput);
													return 1;
												}
												else if (select_variable_type == "stmt") {
													Database::getSynStmt_MSV_Varname(qdbResults, QuoteInput);
													return 1;

												}
												else {
													return 0;
												}
											}
											else if(RHStype == QSymbol::qsynonym && select_variable_type_RHS == "variable"){
												if (select_variable_type_LHS == "assign" ) {
													Database::getSynAssign_MAV(qdbResults);
													return 1;
												}
												else if (select_variable_type_LHS == "read" ) {
													Database::getSynRead_MRV(qdbResults);
													return 1;
												}
												else if (select_variable_type_LHS == "stmt" ) {
													Database::getSynStmt_MSV(qdbResults);
													return 1;
												}
												else {
													return 0;
												}
											}
											else if (RHStype == QSymbol::qwildcard && select_variable_type_RHS == "variable") {
												if (select_variable_type_LHS == "assign") {
													Database::getSynAssign_MAV(qdbResults);
													return 1;
												}
												else if (select_variable_type_LHS == "read") {
													Database::getSynRead_MRV(qdbResults);
													return 1;
												}
												else if (select_variable_type_LHS == "stmt") {
													Database::getSynStmt_MSV(qdbResults);
													return 1;
												}
											}
											else {
												return 0;
											}
										}
										else if (LHStype == QSymbol::qintLineNo && select_variable_type_RHS == "variable") {
											if (select_variable_type_LHS == "assign" && (RHStype == QSymbol::qsynonym || RHStype == QSymbol::qwildcard)) {
												Database::getLineNoAssign_MAV(qdbResults, getqLHSstring());
												return 1;
											}
											else if (select_variable_type == "read" && (RHStype == QSymbol::qsynonym || RHStype == QSymbol::qwildcard)) {
												Database::getLineNoRead_MRV(qdbResults, getqLHSstring());
												return 1;
											}
											else if (select_variable_type == "stmt" && (RHStype == QSymbol::qsynonym || RHStype == QSymbol::qwildcard)) {
												Database::getLineNoStmt_MSV(qdbResults, getqLHSstring());
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
									else { // error: ModifiesS (stmtRef,entRef) <- ) missing
										return 0;
									}
								}
								else { // error: token read error
									return 0;
								}
							}
							else { // error: ModifiesS (stmtRef,entRef) <- entRef broken
								return 0;
							}
						}
						else { // error: ModifiesS (stmtRef,entRef) <- , missing 
							return 0;
						}
					}
					else { // error: read token error 

					}
				}
				else { // error: ModifiesS (stmtRef,entRef) <- stmtRef broken 
					return 0;
				}
			}
			else { // error: ModifiesS (stmtRef,entRef) <- ( missing
				return 0;
			}
		}
		else { // error: read token error
			return 0;
		}
#endif // moved to modifies()

	}	
	else if (qsym == QSymbol::qModifiesP) {
		if(modifiesP(tokens)){
			return 1;
		}
	}
	else if (qsym == QSymbol::qUses) {

	// PRODUCTION UsesS: 'Uses' '(' stmtRef ',' entRef ')'
		if(uses(tokens)){
			return 1;
		}

#if 0 // moved to uses()
		setLHS(); // qUsesS LHS = stmtRef
		if (fetchQToken(tokens)) {
			if (qsym == QSymbol::qbracketOpen) {
				if (stmtRef(tokens)) {
					if (fetchQToken(tokens)) {
						if (qsym == QSymbol::qcomma) {
							clearLHS(); // RHS after comma
							if (entRef(tokens)) {
								if (fetchQToken(tokens)) {
									if (qsym == QSymbol::qbracketClose) {
										if (LHStype == QSymbol::qsynonym) {
											for (auto& t : PQL_select ) {
												if (t.first == getqLHSstring()) {
													if (t.second == "assign") {// UAV
														if (select_variable_type_RHS == "variable") {
															Database::getSynSyn_UAV_Assign(qdbResults);
															return 1;
														}
														else if (RHStype == QSymbol::qdblquote) {
															Database::getSynCount_UAV_Assign(qdbResults, getqRHSstring()); 
															return 1;
														}
														else if (RHStype == QSymbol::qwildcard) {
															Database::getSynSyn_UAV_Assign(qdbResults); // assume best case
															return 1;
														}
														else {
															return 0;
														}
													}
													else if (t.second == "print") { //PNV
														if (select_variable_type == "print") {
															Database::getSynSyn_UPV_Print(qdbResults);
															return 1;
														}
														else if (RHStype == QSymbol::qdblquote) {
															Database::getSynCount_UPV_Print(qdbResults, getqRHSstring());
															return 1;
														}
														else if (RHStype == QSymbol::qwildcard) {
															Database::getSynSyn_UPV_Print(qdbResults); // assume best case
															return 1;
														}
														else {
															return 0;
														}

													}
													else if (t.second == "stmt") { // UAV + USV
														// add condition for USV 
														if (select_variable_type_RHS == "variable") {
															//insert 
															
														}
													} // insert here for Uses Procedure V
													else {
														return 0;
													}

												}
												else {
													return 0; // not in map
												}
											}
										}

										else if (LHStype == QSymbol::qintLineNo) {
												// UAV 
											if (select_variable_type == "assign" || select_variable_type == "stmt") {
												if (select_variable_type_RHS == "variable"){
												Database::getLineSyn_UAV_Assign(qdbResults);
												return 1;
												}
												else {
													return 0;
												}
											}
											else if (select_variable_type == "variable") {
												if (RHStype == QSymbol::qdblquote) {
													Database::getLineSyn_UAV_Variable(qdbResults, getqRHSstring());
													return 1;
													//count
												}
												else {
													return 0;
												}
											}
											else if (RHStype == QSymbol::qwildcard) {
												Database::getLineSyn_UAV_Assign(qdbResults);
											}
											//PNV 
											else if (select_variable_type == "print") {
												if (select_variable_type_RHS == "variable") {
													Database::getLineSyn_UPV_Print(qdbResults,getqRHSstring());
													return 1;
												}
												else if (RHStype == QSymbol::qdblquote) {
													Database::getLineSyn_UPV_Variable(qdbResults, getqRHSstring());
													return 1;
												}
												else if (RHStype == QSymbol::qwildcard) {
													Database::getLineSyn_UPV_Print(qdbResults, getqRHSstring());
													return 1;
												}
												else {
													return 0;
												}
											}
											else {
												return 0; // not Line No UAV not UPV
											}
										}
										else if (LHStype == QSymbol::qwildcard) {
											// uav
											if (select_variable_type == "assign" || select_variable_type == "stmt") {
												if (select_variable_type_RHS == "variable"){
												Database::getSynSyn_UAV_Assign(qdbResults);
												return 1;
												}
												else if (RHStype == QSymbol::qdblquote) {
													Database::getSynCount_UAV_Assign(qdbResults, getqRHSstring());
													return 1;
												}
												else if (RHStype == QSymbol::qwildcard) {
													Database::getSynSyn_UAV_Assign(qdbResults);
													return 1;
												}
												else {
													return 0;
												}
											}
											// upv
											else if (select_variable_type == "print") {
												if (select_variable_type_RHS == "variable") {
													Database::getSynSyn_UPV_Print(qdbResults);
													return 1;
												}
												else if (RHStype == QSymbol::qdblquote) {
													Database::getSynCount_UPV_Print(qdbResults, getqRHSstring());
													return 1;
												}
												else if (RHStype == QSymbol::qwildcard) {
													Database::getSynSyn_UPV_Print(qdbResults);
													return 1;
												}
												else {
													return 0;
												}
											}
											else {
												return 0; // not wildcard UAV UPV
											}
										}
										else{
										return 0;
										}
									}
									else { // error: ModifiesS (stmtRef,entRef) <- ) missing
										return 0;
									}
								}
								else { // error: token read error
									return 0;
								}
							}
							else { // error: ModifiesS (stmtRef,entRef) <- entRef broken
								return 0;
							}
						}
						else { // error: ModifiesS (stmtRef,entRef) <- , missing 
							return 0;
						}
					}
					else { // error: read token error 

					}
				}
				else { // error: ModifiesS (stmtRef,entRef) <- stmtRef broken 
					return 0;
				}
			}
			else { // error: ModifiesS (stmtRef,entRef) <- ( missing
				return 0;
			}
		}
		else { // error: read token error
			return 0;
		}
#endif // moved to uses()		
	}
	else if (qsym == QSymbol::qUsesP) {
		if(usesP(tokens)){
			return 1;
		}
	}
	else if (qsym == QSymbol::qParent) {

		if(parent(tokens)){
			return 1;
		}
#if 0 
		if (fetchQToken(tokens)) {
			if (qsym == QSymbol::qbracketOpen) {
				if (stmtRef(tokens)) {
					if (fetchQToken(tokens)) {
						if (qsym == QSymbol::qcomma) {
							clearLHS();
							if (entRef(tokens)) {
								if (fetchQToken(tokens)) {
									if (qsym == QSymbol::qbracketClose) {
										// do what?

										// call some DB function like this -> Database::getProcedures(qdbResults);
										// ToDo: put for loop here? or back in QueryProcessor::evaluate?
										// for (string databaseResult : databaseResults) {
										// 	output.push_back(qp.getDBresults());
										// }
											// leave this as loop see got issue 
										if (LHStype == QSymbol::qsynonym && RHStype == QSymbol::qsynonym) {
											for (auto& t : PQL_select) {
												if (t.first == getqLHSstring()) {
													if (t.second == "stmt") {
														for (auto& k : PQL_select) {
															if (k.first == getqRHSstring()) {
																if (k.second == "stmt") {
																	Database::getSynSyn_Parent(qdbResults);
																	return 1;
																}
																if (k.second == "read") {
																	Database::getSynSyn_Parent_Read(qdbResults);
																	return 1;
																}
																if (k.second == "print") {
																	Database::getSynSyn_Parent_Print(qdbResults);
																	return 1;
																}
																if (k.second == "assign") {
																	Database::getSynSyn_Parent_Assign(qdbResults);
																	return 1;
																}
																else {
																	return 0;
																}
															}
														}
													}
												}
											}
										}//insert SYN, SYN 1
										else if (LHStype == QSymbol::qsynonym && RHStype == QSymbol::qwildcard ) {
											for (auto& t : PQL_select) {
												if (t.first == getqLHSstring()) {
													if (t.second == "stmt") {
														Database::getSynSyn_Parent(qdbResults);
														return 1;
													}
													else {
														return 0; // we take stmt stmt as best case. so only need to return 1 type 
													}
												}
											}
										} // insert SYN, wildcard 

										else if (LHStype == QSymbol::qsynonym && RHStype == QSymbol::qintLineNo) {
											for (auto& t : PQL_select) {
												if (t.first == getqLHSstring()) {
													if (t.second == "stmt") {
														Database::getSynLine_Parent(qdbResults, getqRHSstring());
														return 1;
													}
													else {
														return 0; // Only stmt make sense in our design 
													}
												}
											}

										}// insert syn,LineNo 3

										else if (LHStype == QSymbol::qintLineNo && RHStype == QSymbol::qsynonym) {
											for (auto& t : PQL_select) {
												if (t.first == getqRHSstring()) {
													if (t.second == "stmt") {
														Database::getLineSyn_Parent(qdbResults, getqLHSstring());
														return 1;
													}
													else {
														return 0; // Only stmt make sense in our design 
													}
												}
											}
											// database query need to take in line no getqLHSstring();

										}// insert LineNo,syn 4

										else if (LHStype == QSymbol::qintLineNo && RHStype == QSymbol::qwildcard ) {
											Database::getLineSyn_Parent(qdbResults, getqLHSstring()); // WE ASSUME THAT WILDCARD IS GOING TO GIVE BEST CASE WHICH IS SYN STMT

										}// insert LineNo,wildcard 5

										else if (LHStype == QSymbol::qintLineNo && RHStype == QSymbol::qintLineNo) {

											Database::getLineLine_Parent(qdbResults, getqLHSstring(), getqRHSstring());
											// db will do the query no check required


											// return a boolean expression of a query to check if S2 is a child of S1 
											// check table 
										}// insert LineNo,lineno	6
										else if (LHStype == QSymbol::qwildcard  && RHStype == QSymbol::qsynonym) {
											for (auto& t : PQL_select) {
												if (t.first == getqRHSstring()) {
													if (t.second == "stmt") {

														Database::getWildSyn_Parent(qdbResults);
													}
													else {
														return 0;
													}
												}
											}

										}
										else if (LHStype == QSymbol::qwildcard && RHStype == QSymbol::qwildcard ) {
												
											Database::getWildSyn_Parent(qdbResults);
										}

										else if (LHStype == QSymbol::qwildcard  && RHStype == QSymbol::qintLineNo) {
												
											Database::getWildLine_Parent(qdbResults, getqRHSstring());
										
										}
										else { // error: Parent (stmtRef,entRef) <- ) missing
											return 0;
										}
									}
									else { // error: Parent (stmtRef,entRef) <- , missing 
										return 0;
									}
								}
								else { // error: read token error 

								}
							}
							else { // error: Parent (stmtRef,entRef) <- stmtRef broken 
								return 0;
							}
						}
						else { // error: Parent (stmtRef,entRef) <- ( missing
							return 0;
						}
					}
					else { // error: read token error
						return 0;
					}
				}
				else if (qsym == QSymbol::qParentT) {
					if (fetchQToken(tokens)) {
						if (qsym == QSymbol::qbracketOpen) {
							if (stmtRef(tokens)) {
								if (fetchQToken(tokens)) {
									if (qsym == QSymbol::qcomma) {
										if (entRef(tokens)) {
											if (fetchQToken(tokens)) {
												if (qsym == QSymbol::qbracketClose) {
													// do what?

													// call some DB function like this -> Database::getProcedures(qdbResults);
													// ToDo: put for loop here? or back in QueryProcessor::evaluate?
													// for (string databaseResult : databaseResults) {
													// 	output.push_back(qp.getDBresults());
													// }
												}
												else { // error: ParentT (stmtRef,entRef) <- ) missing
													return 0;
												}
											}
											else { // error: token read error
												return 0;
											}
										}
										else { // error: ParentT (stmtRef,entRef) <- entRef broken
											return 0;
										}
									}
									else { // error: ParentT (stmtRef,entRef) <- , missing 
										return 0;
									}
								}
								else { // error: read token error 

								}
							}
							else { // error: ParentT (stmtRef,entRef) <- stmtRef broken 
								return 0;
							}
						}
						else { // error: ParentT (stmtRef,entRef) <- ( missing
							return 0;
						}
					}
					else { // error: read token error
						return 0;
					}
				}
				else { // error relRef wrong 
					return 0;
				}
			}
			else { // error: such that relRef <- relRef broken
				return 0;
			}

		}
#endif 		
	}
	else if (qsym == QSymbol::qParentT) {
		if(parentT(tokens)){
			return 1;
		}
	}
	else if (qsym == QSymbol::qNext) {
		if(next(tokens)){
			return 1;
		}
	}
	else if (qsym == QSymbol::qNextT) {
		if(nextT(tokens)){
			return 1;
		}
	}
	else if (qsym == QSymbol::qCalls) {
		if(calls(tokens)){
			return 1;
		}
	}
	else if (qsym == QSymbol::qCallsT) {
		if(callsT(tokens)){
			return 1;
		}
	}
	qFail("no relRef recognized");
	return 0;
}

int QueryParser::fetchQDeclToken(vector<string>& tokens) {
	// declarations stored in PQL_select

	if(qtokencounter < (int)tokens.size()){
		for (auto& t : qkey_map) {
			if (t.second == tokens.at(qtokencounter)) {
				qsym = t.first;							
				if (qsym == QSymbol::qselect) {
					return 1;
				}
				if (!(qtokencounter % 3)) {  // %3 because  assign a ;  0 1 2  1 and 2 doesnt take in
					
					PQL_select.insert(std::pair<string, string>(tokens.at(qtokencounter+1), tokens.at(qtokencounter)));
				}
				qtokencounter += 2;
				if (qtokencounter >= (int)tokens.size()) {
					qFail("qtokencounter bounds violation at fetchQDeclTokens()");
					return 0;
				}
				if(fetchQToken(tokens))
					return 1;
				else {
					qFail("fetchQToken fail in fetchQDeclToken");
					return 0;
				}
			}
		}
	}
	// error: token read error  #ToDo CHECK IF END OF FILE works means got space at the back of query and no space of back of query 
	qsym = QSymbol::qtokensRanOut;
	qFail("fetchQDeclToken failed - tokens ran out");
	return 0; 
}

//#fetchtoken
int QueryParser::fetchQToken(vector<string>& tokens) {

	// ToDo: add Parent* Next* 

#if 0 //moved to fetchQDeclToken
	if(qtokencounter < tokens.size()){
		for (auto& t : qkey_map) {
			if (t.second == tokens.at(qtokencounter)) {
				qsym = t.first;									
				if (!(qtokencounter % 3) && fetch2tokens) {  // %3 because  assign a ;  0 1 2         1 and 2 doesnt take in
					
					PQL_select.insert(std::pair<string, string>(tokens.at(qtokencounter+1), tokens.at(qtokencounter)));
				}
				qtokencounter++;
				return 1;
			}
		}
	} else{ // error: token read error  #ToDo CHECK IF END OF FILE works means got space at the back of query and no space of back of query 
		qsym = QSymbol::qtokensRanOut;
		return 0; 
	}
#endif //moved to fetchQDeclToken

	// MERGE CHECK 
	if (qtokencounter >= (int)tokens.size()) {
		qFail("qtokencounter bounds violation at fetchQDeclTokens()");
		return 0;
	}

	for (auto& t : qkey_map) {
		if (t.second == tokens.at(qtokencounter)) {
			qsym = t.first;
			qtokencounter ++;
			return 1;
		}
	}

	int var = tokens.at(qtokencounter)[0];
	
	
	if (((var >=65) && (var <= 90)) || ((var >= 97) && (var <= 122))) {  // letters a-z & A-Z 

		qsym = QSymbol::qsynonym;
		qtokencounter++;
		return 1;
	}

	if (((var >= 48) && (var <= 57))) { // integer 0-9 ie line no

		for (auto& t : tokens.at(qtokencounter)) {
			// loop through the entire string to make sure its a real integer 
			if (!((t >= 48) && (t <= 57))) {
				qFail("fetchQToken INTEGER syntax error");
				return 0;
			}
		}
		qsym = QSymbol::qintLineNo;
		qtokencounter++;
		return 1;
	} 
	
	if (var == 34){
		int i = qtokencounter + 1; // from "  go to the first string of the vector until  >"
		for ( i; tokens.at(i) != "\""; i++) {
			string transfer = tokens.at(i);
			QuoteInput += transfer;
		}
		qsym = QSymbol::qdblquote;
		qtokencounter = i;
		qtokencounter++;	
		return 1;
	}
	qFail("fetchQToken failed - no recognizable token");
	return 0;
}

int QueryParser::initQMap() {

	qkey_map.insert(std::pair<QSymbol , string>(QSymbol::qprocedure, "procedure"));

	qkey_map.insert(std::pair<QSymbol , string>(QSymbol::qassign,"assign"));
	qkey_map.insert(std::pair<QSymbol, string>(QSymbol::qprint,"print"));
	qkey_map.insert(std::pair<QSymbol , string>(QSymbol::qread, "read"));
	qkey_map.insert(std::pair<QSymbol , string>( QSymbol::qstmt,"stmt" ));
	qkey_map.insert(std::pair<QSymbol , string>(QSymbol::qvariable, "variable"));
	qkey_map.insert(std::pair<QSymbol , string>( QSymbol::qconstant, "constant"));
	qkey_map.insert(std::pair<QSymbol , string>(QSymbol::qcall, "call"));
	qkey_map.insert(std::pair<QSymbol , string>( QSymbol::qwhile, "while"));
	qkey_map.insert(std::pair<QSymbol , string>( QSymbol::qif, "if"));
	qkey_map.insert(std::pair<QSymbol , string>(QSymbol::qsuch, "such"));
	qkey_map.insert(std::pair<QSymbol , string>(QSymbol::qthat, "that"));
	qkey_map.insert(std::pair<QSymbol , string>(QSymbol::qpattern,"pattern"));

	qkey_map.insert(std::pair<QSymbol, string>(QSymbol::qcomma, ","));
	qkey_map.insert(std::pair<QSymbol, string>(QSymbol::qbracketOpen, "(" ));
	qkey_map.insert(std::pair<QSymbol, string>(QSymbol::qbracketClose, ")"));
	qkey_map.insert(std::pair<QSymbol, string>(QSymbol::qdblquote, "\"\""));
	qkey_map.insert(std::pair<QSymbol, string>(QSymbol::qsemicolon, ";"));
	qkey_map.insert(std::pair<QSymbol, string>(QSymbol::qwildcard, "_"));
	qkey_map.insert(std::pair<QSymbol, string>(QSymbol::qangleOpen, "<"));
	qkey_map.insert(std::pair<QSymbol, string>(QSymbol::qangleClose, ">"));

	qkey_map.insert(std::pair<QSymbol, string>(QSymbol::qselect, "Select"));
	qkey_map.insert(std::pair<QSymbol, string>(QSymbol::qNext, "Next"));
	qkey_map.insert(std::pair<QSymbol, string>(QSymbol::qNextT, "Next*"));
	qkey_map.insert(std::pair<QSymbol, string>(QSymbol::qParent, "Parent"));
	qkey_map.insert(std::pair<QSymbol, string>(QSymbol::qParentT, "Parent*"));
	qkey_map.insert(std::pair<QSymbol, string>(QSymbol::qModifies, "Modifies"));
	qkey_map.insert(std::pair<QSymbol, string>(QSymbol::qModifiesP, "Modifies*"));
	qkey_map.insert(std::pair<QSymbol, string>(QSymbol::qUses, "Uses"));
	qkey_map.insert(std::pair<QSymbol, string>(QSymbol::qUsesP, "Uses*"));
	qkey_map.insert(std::pair<QSymbol, string>(QSymbol::qCalls, "Calls"));
	qkey_map.insert(std::pair<QSymbol, string>(QSymbol::qCallsT, "Calls*"));	

	syn_map.insert(std::pair<string, string>("1","procedure"));
	syn_map.insert(std::pair<string, string>("2", "assign" ));
	syn_map.insert(std::pair<string, string>("3", "print" ));
	syn_map.insert(std::pair<string, string>("4", "read" ));
	syn_map.insert(std::pair<string, string>("5", "stmt"));
	syn_map.insert(std::pair<string, string>("6", "variable" ));
	syn_map.insert(std::pair<string, string>("7", "constant" ));
	syn_map.insert(std::pair<string, string>("8", "while"));
	syn_map.insert(std::pair<string, string>("9", "if" ));
	//syn_map.insert(std::pair<string, string>("call", ""));

	return 1; 
		
}

void QueryParser::set_Select_variable(vector<string>& tokens) {

	for (auto& t : PQL_select) {
		if (t.first == select_variable) {
			select_variable_type = t.second;
			break;
		}
	}
}

void QueryParser::set_Select_variable_LHS(vector<string>& tokens) {

	for (auto& t : PQL_select) {
		string test = getqLHSstring();
			if (t.first == getqLHSstring()) {
				select_variable_type_LHS = t.second;
				break;
			}
	}

}

void QueryParser::set_Select_variable_RHS(vector<string>& tokens) {

	for (auto& t : PQL_select) {
		if (t.first == getqRHSstring()) {
			select_variable_type_RHS = t.second;
			break;
		}
	}

}

bool QueryParser::isQsymDeclaredSynonym(vector<string>& tokens) {
	//** DO AS A MAP** 
	// DB class to provide function to
	// match qsym against synonym table PK col
	// 
	// SQL:  SELECT * FROM synonymTbl WHERE PK = qsym;
	// store Tbl into qtblName

	for (auto& t : PQL_select) {
		for (auto& k : syn_map) {
			if (k.second == t.second) {
				qsym = QSymbol::qsynonym;
				return true;
			}
		}
		// if no match
		return false;
	}
}


bool QueryParser::isSelectValidVariable(vector<string>& tokens,int tokencounter) {

	string check = tokens.at(tokencounter);
	for (auto& t : PQL_select) {

		if (t.first == check)
			return true; 

	}
	return false;

}


bool Node::isOperator(char c) {
	return (c == '+' || c == '-' || c == '*' || c == '/' || c == '^');
}


// Function to construct an expression tree from the given postfix expression
int  Node::construct(string postfix)
{
	// base case
	if (postfix.length() == 0) {
		return 0;
	}

	// create an empty stack to store tree pointers

	// traverse the postfix expression
	for (char c : postfix)
	{
		// if the current token is an operator
		if (isOperator(c))
		{
			// pop two nodes `x` and `y` from the stack
			Node* x = s.top();
			s.pop();


			Node* y = s.top();
			s.pop();

			// construct a new binary tree whose root is the operator and whose
			// left and right children point to `y` and `x`, respectively
			Node* node = new Node(c, y, x);

			// push the current node into the stack
			s.push(node);
		}

		// if the current token is an operand, create a new binary tree node
		// whose root is the operand and push it into the stack
		else {
			s.push(new Node(c));
		}
	}

	// a pointer to the root of the expression tree remains on the stack
	return 1;
}
bool QueryParser::areIdentical(Node* root1, Node* root2)
{
	/* base cases */
	if (root1 == NULL && root2 == NULL)
		return true;

	if (root1 == NULL || root2 == NULL)
		return false;

	/* Check if the data of both roots is
	same and data of left and right
	subtrees are also same */
	return (root1->data == root2->data &&
		areIdentical(root1->left, root2->left) &&
		areIdentical(root1->right, root2->right));
}

bool QueryParser::isSubtree(Node* T, Node* S)
{
	/* base cases */
	if (S == NULL)
		return true;

	if (T == NULL)
		return false;

	/* Check the tree with root as current node */
	if (areIdentical(T, S))
		return true;

	/* If the tree with root as current
	node doesn't match then try left
	and right subtrees one by one */
	return isSubtree(T->left, S) ||
		isSubtree(T->right, S);
}


int QueryParser::prec(char c) {
	if (c == '^')
		return 3;
	else if (c == '/' || c == '*')
		return 2;
	else if (c == '+' || c == '-')
		return 1;
	else
		return -1;
}

// The main function to convert infix expression
//to postfix expression
string QueryParser::infixToPostfix(string s) {

	stack<char> st; //For stack operations, we are using C++ built in stack
	string result;

	for (int i = 0; i < s.length(); i++) {
		char c = s[i];

		// If the scanned character is
		// an operand, add it to output string.
		if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
			result += c;

		// If the scanned character is an
		// ‘(‘, push it to the stack.
		else if (c == '(')
			st.push('(');

		// If the scanned character is an ‘)’,
		// pop and to output string from the stack
		// until an ‘(‘ is encountered.
		else if (c == ')') {
			while (st.top() != '(')
			{
				result += st.top();
				st.pop();
			}
			st.pop();
		}

		//If an operator is scanned
		else {
			while (!st.empty() && prec(s[i]) <= prec(st.top())) {
				result += st.top();
				st.pop();
			}
			st.push(c);
		}
	}

	// Pop all the remaining elements from the stack
	while (!st.empty()) {
		result += st.top();
		st.pop();
	}
	return result;
}