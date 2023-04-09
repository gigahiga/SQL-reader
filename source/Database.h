#pragma once

#include <string>
#include <vector>
#include "sqlite3.h"

using namespace std;

// The Database has to be a static class due to various constraints.
// It is advisable to just add the insert / get functions based on the given examples.
class Database {
public:
	// method to connect to the database and initialize tables in the database
	static void initialize();

	// method to close the database connection
	static void close();

	// method to insert a procedure into the database
	static void insertProcedure(string procedureName,int procID);

	//overloaded variable 
	static void insertVariable(string variableName);

	// method to insert variables into the database || VARIABLE HAVE  
	static void insertVariableUSV(string currentVariable, int LineNo);

	// method to insert variables into the database || VARIABLE DONT HAVE 
	static void insertVariableUSVNULL(string currentVariable, int LineNo);

	// method to insert variables into the database || VARIABLE HAVE 
	static void insertVariableMSV(string currentVariable, int MSV);

	// method to insert variables into the database || VARIABLE DONT HAVE  
	static void insertVariableMSVNULL(string currentVariable, int MSV);

	// method to insert variables into the database || added 24th jan
	static void insertStatement(string statementName, int LineNo, int ParentNo);

	// method to insert variables into the database || added 24th jan
	static void insertStatementWhile(string statementName, int LineNo, int ParentNo,int flag);

	static void insertStatementIf(string statementName, int LineNo, int ParentNo, int flag);

	// method to insert constant into the database || added 24th jan
	static void insertConstant( int LineNo,string constantName );

	// method to insert read into the database || added 24th jan
	static void insertRead(string readName, int LineNo);

	// method to insert print into the database || added 24th jan
	static void insertPrint(string printName, int LineNo);

	// method to insert print into the database || added 24th jan
	static void insertAssign(int LineNo, string assignLHS, string assignRHS);
	// method to insert a repeat MSV even tho its in the table of variable 
	static void insertAssignMSVR( int LineNo,string assignRHS);
	

	// method to add to block table UPV RHS 
	static void insertBlockUPV(int LineNo, string UPV, int blockID);

	// method to add to block table MPV LHS 
	static void insertBlockMPV(int LineNo, string MPV, int blockID);

	// method to add to pattern table LHS RHS 
	static void insertPattern(int LineNo, string pLHS, string pRHS);




	// method to get all the procedures from the database
	static void getProcedures(vector<string>& results);

	static void getVariable(vector<string>& results);

	static void getStatement(vector<string>& results);

	static void getConstant(vector<string>& results);

	static void getRead(vector<string>& results);

	static void getPrint(vector<string>& results);

	static void getAssign(vector<string>& results);

	static void getPattern(vector<string>& results);


	static void getWhile(vector<string>& results);

	static void getIf(vector<string>& results);

	static void getPattern_Assign_LHS(vector<string>& results,string LHS);
	

	static bool varCheck(vector<string>& results,string varChecker);

	static bool blockUPVCheck(vector<string>& results, string UPVChecker, int blockID);


	/// checkers for read,print,assign

	static void CheckPrint(vector<string>& results,string input);
	static void CheckAssign(vector<string>& results, string input);
	static void CheckRead(vector<string>& results, string input);
	static void CheckStmt(vector<string>& results, string input);
	static void CheckWhile(vector<string>& results, string input);
	static void CheckIf(vector<string>& results, string input);
	static void CheckStmt_LHS_WildCard(vector<string>& results, string input);
	
	//												######## PARENT GETTERS ############## 


	//############## SYN##############
	static void getSynSyn_Parent(vector<string>& results); // This is for assuming for wildcards and syn syn case

	static void getSynSyn_Parent_Read(vector<string>& results);

	static void getSynSyn_Parent_Print(vector<string>& results);

	static void getSynSyn_Parent_Assign(vector<string>& results);

	static void getSynLine_Parent(vector<string>& results, string input);

	//######################## LINE####################

	static void getLineSyn_Parent(vector<string>& results, string input);
	// getLineWild is the same as LineSyn as we assume wild to be Syn + stmt as best case 
	static void getLineLine_Parent(vector<string>& results, string LHSinput, string RHSinput);


	//###################### Wild ###############

	static void getWildSyn_Parent(vector<string>& results);
	// Get wild wild is the same as wild syn as we assume best case 
	static void getWildLine_Parent(vector<string>& results,string input);


													//############ MODIFIES GETTERS #############

									//######### SYN ASSIGN ############
	//#########  MAV ##########
	static void getSynAssign_MAV(vector<string>& results);
	// #####MAV Syn Assign Mav Insert the varname name 
	static void getSynAssign_MAV_Varname(vector<string>& results,string input);
	//####### MAV LINE NUMBER LHS 
	static void getLineNoAssign_MAV(vector<string>& results, string input);

	//########## MRV ###########
	static void getSynRead_MRV(vector<string>& results);
	// #####MAV Syn Read MRV Insert the varname name 
	static void getSynRead_MRV_Varname(vector<string>& results, string input);
	//####### MRV LINE NUMBER LHS 
	static void getLineNoRead_MRV(vector<string>& results, string input);
	
	// ######### MSV #########
	static void getSynStmt_MSV(vector<string>& results);
	// #####MAV Syn Read MRV Insert the varname name 
	static void getSynStmt_MSV_Varname(vector<string>& results, string input);
	//####### MSV LINE NUMBER LHS 
	static void getLineNoStmt_MSV(vector<string>& results, string input);


												//############ USES GETTERS #############
	//# UAV
	static void getSynSyn_UAV_Assign(vector<string>& results);
	// used for wildcard also
	static void getSynCount_UAV_Assign(vector<string>& results, string input);
	// count for assign
	static void getLineSyn_UAV_Assign(vector<string>& results);

	static void getLineSyn_UAV_Variable(vector<string>& results, string input); // Count 

	// #UPV
	static void getSynSyn_UPV_Print(vector<string>& results);

	static void getSynCount_UPV_Print(vector<string>& results,string input);

	static void getLineSyn_UPV_Print(vector<string>& results,string input);

	static void getLineSyn_UPV_Variable(vector<string>& results,string input );


	//#MAV 

	static void getSynSyn_MAV_Stmt(vector<string>& results); // returns all characters on the right hand side 


											//############ Patterns GETTERS #############

	static void QueryPatternTree_With_LHS(vector<string>& results, string LHS);

	

	static void QueryPatternTree_Only_RHS(vector<string>& results);


	//######################## NEXT GETTERS #####################

	static void getNext_SYN_SYN_stmt(vector<string>& results);

	static void getNext_SYN_SYN_stmt_loop(vector<string>& results);

	static void getNext_SYN_SYN_read(vector<int>& checker, vector<string>& results);

	static void getNext_SYN_SYN_read_while(vector<int>& checker, vector<string>& results);

	static void getNext_SYN_SYN_read_if(vector<int>& checker, vector<string>& results);


	static void getNext_SYN_SYN_while_read(vector<int>& checker, vector<string>& results);

	static void getNext_SYN_SYN_if_read(vector<int>& checker, vector<string>& results);


private:
	// the connection pointer to the database
	static sqlite3* dbConnection;
	// a vector containing the results from the database
	static vector<vector<string>> dbResults;
	// the error message from the database
	static char* errorMessage;

	static bool nullresult;
	// callback method to put one row of results from the database into the dbResults vector
	// This method is called each time a row of results is returned from the database
	static int callback(void* NotUsed, int argc, char** argv, char** azColName);

	static int callback2(void* NotUsed, int argc, char** argv, char** azColName);
};

