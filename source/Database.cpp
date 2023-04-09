#include "Database.h"
#include <iostream>
#include <string>
#include "QParser.h"
using namespace std;

sqlite3* Database::dbConnection;
vector<vector<string>> Database::dbResults;
char* Database::errorMessage;

// method to connect to the database and initialize tables in the database
void Database::initialize() {


	//					################## D R O P  T A B L E S ##################

	// open a database connection and store the pointer into dbConnection
	sqlite3_open("database.db", &dbConnection);

	// drop the existing procedure table (if any)
	string dropProcedureTableSQL = "DROP TABLE IF EXISTS procedures";
	sqlite3_exec(dbConnection, dropProcedureTableSQL.c_str(), NULL, 0, &errorMessage);

	// drop the existing variable table (if any) ||added
	string dropVariableTableSQL = "DROP TABLE IF EXISTS variable";
	sqlite3_exec(dbConnection, dropVariableTableSQL.c_str(), NULL, 0, &errorMessage);

	// drop the existing statement table (if any) ||added
	string dropStatementTableSQL = "DROP TABLE IF EXISTS stmt";
	sqlite3_exec(dbConnection, dropStatementTableSQL.c_str(), NULL, 0, &errorMessage);

	// drop the existing constant table (if any) ||added
	string dropConstantTableSQL = "DROP TABLE IF EXISTS constant";
	sqlite3_exec(dbConnection, dropConstantTableSQL.c_str(), NULL, 0, &errorMessage);

	// drop the existing read table (if any) ||added
	string dropReadTableSQL = "DROP TABLE IF EXISTS read";
	sqlite3_exec(dbConnection, dropReadTableSQL.c_str(), NULL, 0, &errorMessage);

	// drop the existing print table (if any) ||added
	string dropPrintTableSQL = "DROP TABLE IF EXISTS print";
	sqlite3_exec(dbConnection, dropPrintTableSQL.c_str(), NULL, 0, &errorMessage);

	// drop the existing assign table (if any) ||added
	string dropAssignTableSQL = "DROP TABLE IF EXISTS assign";
	sqlite3_exec(dbConnection, dropAssignTableSQL.c_str(), NULL, 0, &errorMessage);

	// drop the existing pattern table (if any) ||added
	string dropPatternTableSQL = "DROP TABLE IF EXISTS pattern";
	sqlite3_exec(dbConnection, dropAssignTableSQL.c_str(), NULL, 0, &errorMessage);


	//													################## C R E A T E  T A B L E S ##################

	// create a procedure table 
	string createProcedureTableSQL = "CREATE TABLE procedures ( procedureName VARCHAR(255), procedureID VARCHAR(255) PRIMARY KEY);";
	sqlite3_exec(dbConnection, createProcedureTableSQL.c_str(), NULL, 0, &errorMessage);

	// create a procBlock table  || added 
	string createProcBlockTableSQL = "CREATE TABLE ProcBlock (LineNo VARCHAR(255) PRIMARY KEY ,UPV VARCHAR(255) ,MPV VARCHAR(255) ,BlockID VARCHAR(255), FOREIGN KEY(BlockID) REFERENCES procedures(procedureID) );";
	sqlite3_exec(dbConnection, createProcBlockTableSQL.c_str(), NULL, 0, &errorMessage);

	// create a variable table  || added   
	string createVariableTableSQL = "CREATE TABLE variable ( variableName VARCHAR(255) PRIMARY KEY, USV VARCHAR(255), MSV VARCHAR(255));";
	sqlite3_exec(dbConnection, createVariableTableSQL.c_str(), NULL, 0, &errorMessage);

	// create a statement table  || added 
	string createStatementTableSQL = "CREATE TABLE stmt ( stmtLine VARCHAR(255) PRIMARY KEY, Parent VARCHAR(255) NOT NULL, While VARCHAR(255), If VARCHAR(255));";
	sqlite3_exec(dbConnection, createStatementTableSQL.c_str(), NULL, 0, &errorMessage);

	// create a constant table  || added 
	string createConstantTableSQL = "CREATE TABLE constant ( constantLine VARCHAR(255) PRIMARY KEY, constValue VARCHAR(255));"; // add 1 more col called constValue
	sqlite3_exec(dbConnection, createConstantTableSQL.c_str(), NULL, 0, &errorMessage);

	// create a read table  || added 
	string createReadTableSQL = "CREATE TABLE read ( readLine VARCHAR(255) PRIMARY KEY, MRV VARCHAR(255));"; // when null means nothing 
	sqlite3_exec(dbConnection, createReadTableSQL.c_str(), NULL, 0, &errorMessage);

	// create a print table  || added 
	string createPrintTableSQL = "CREATE TABLE print ( printLine VARCHAR(255) PRIMARY KEY, UPNV VARCHAR(255));"; // when null means nothing 
	sqlite3_exec(dbConnection, createPrintTableSQL.c_str(), NULL, 0, &errorMessage);

	// create a assign table  || added 
	string createAssignTableSQL = "CREATE TABLE assign ( assignLine VARCHAR(255) PRIMARY KEY, UAV VARCHAR(255), MAV VARCHAR(255), MSVR VARCHAR(255));";
	sqlite3_exec(dbConnection, createAssignTableSQL.c_str(), NULL, 0, &errorMessage);

	// create a pattern table  || added 
	string createPatternTableSQL = "CREATE TABLE pattern ( patternLine VARCHAR(255) PRIMARY KEY, pLHS VARCHAR(255), pRHS VARCHAR(255));";
	sqlite3_exec(dbConnection, createPatternTableSQL.c_str(), NULL, 0, &errorMessage);


	///////////////////////////////////////////////////////// PQL SIDE CREATE TABLE ///////////////////////////////////////////////









	// initialize the result vector
	dbResults = vector<vector<string>>();
}

// method to close the database connection
void Database::close() {
	sqlite3_close(dbConnection);
}

// method to insert a procedure into the database
void Database::insertProcedure(string procedureName,int procID) {
	string insertProcedureSQL = "INSERT INTO procedures ('procedureName','procedureID') VALUES ('" + procedureName + "','" + to_string(procID) + "');";
	sqlite3_exec(dbConnection, insertProcedureSQL.c_str(), NULL, 0, &errorMessage);
}

//method to insert variables into the database || 
void Database::insertVariable(string variableName) {
	string insertVariableSQL = "INSERT INTO variable ('variableName') VALUES ('" + variableName + "');";
	sqlite3_exec(dbConnection, insertVariableSQL.c_str(), NULL, 0, &errorMessage);
}

//method to insert variables into the database || IF DONT HAVE 
void Database::insertVariableUSVNULL(string variableName, int USV) {
	string insertVariableUSVNULLSQL = "INSERT INTO variable ('variableName','USV') VALUES ('" + variableName + "','" + to_string(USV) + "');";
	sqlite3_exec(dbConnection, insertVariableUSVNULLSQL.c_str(), NULL, 0, &errorMessage);
}


//method to insert variables into the database || IF HAVE 
void Database::insertVariableUSV( string currentVariable , int LineNo) {
	string insertVariableUSVSQL = "UPDATE variable SET USV = '" + to_string(LineNo) + "' WHERE variableName = '" + currentVariable + "';";
	sqlite3_exec(dbConnection, insertVariableUSVSQL.c_str(), NULL, 0, &errorMessage);
}
//method to insert variables into the database || IF HAVE
void Database::insertVariableMSV(string currentVariable, int LineNo) {
	string insertVariableMSVSQL = "UPDATE variable SET MSV = '" + to_string(LineNo) + "' WHERE variableName = '" + currentVariable + "';";
	sqlite3_exec(dbConnection, insertVariableMSVSQL.c_str(), NULL, 0, &errorMessage);
}

//method to insert variables into the database || IF DONT HAVE 
void Database::insertVariableMSVNULL(string variableName, int MSV) {
	string insertVariableMSVNULLSQL = "INSERT INTO variable ('variableName','MSV') VALUES ('" + variableName + "','" + to_string(MSV) + "');";
	sqlite3_exec(dbConnection, insertVariableMSVNULLSQL.c_str(), NULL, 0, &errorMessage);
}
//method to insert statement into the database || add
void Database::insertStatement(string statementName, int LineNo, int ParentNo) {
	string insertStatementSQL = "INSERT INTO stmt ('stmtLine','Parent','While','If') VALUES ('" + to_string(LineNo) + "','" + to_string(ParentNo) + "','" + to_string(0) + "','" + to_string(0) + "');";
	sqlite3_exec(dbConnection, insertStatementSQL.c_str(), NULL, 0, &errorMessage);
}
void Database::insertStatementWhile(string statementName, int LineNo, int ParentNo, int flag) {
	string insertStatementSQL = "INSERT INTO stmt ('stmtLine','Parent','While','If') VALUES ('" + to_string(LineNo) + "','" + to_string(ParentNo) + "','" + to_string(flag) + "','" + to_string(0) + "');";
	sqlite3_exec(dbConnection, insertStatementSQL.c_str(), NULL, 0, &errorMessage);
}

void Database::insertStatementIf(string statementName, int LineNo, int ParentNo, int flag) {
	string insertStatementSQL = "INSERT INTO stmt ('stmtLine','Parent','If','While') VALUES ('" + to_string(LineNo) + "','" + to_string(ParentNo) + "','" + to_string(flag) + "','" + to_string(0) + "');";
	sqlite3_exec(dbConnection, insertStatementSQL.c_str(), NULL, 0, &errorMessage);
}

//method to insert constant into the database || add
void Database::insertConstant( int LineNo, string constantName) {
	string insertConstantSQL = "INSERT INTO constant ('constantLine','constValue') VALUES ('" + to_string(LineNo) + "','" + constantName + "');";
	sqlite3_exec(dbConnection, insertConstantSQL.c_str(), NULL, 0, &errorMessage);
}


//method to insert read into the database || add
void Database::insertRead(string readName, int LineNo) {
	string insertReadSQL = "INSERT INTO read ('readLine','MRV') VALUES ('" + to_string(LineNo) + "','" + readName + "');";
	sqlite3_exec(dbConnection, insertReadSQL.c_str(), NULL, 0, &errorMessage);
}

//method to insert print into the database || add
void Database::insertPrint(string printName, int LineNo) {
	string insertPrintSQL = "INSERT INTO print ('printLine','UPNV') VALUES ('" + to_string(LineNo) + "','" + printName + "');";
	sqlite3_exec(dbConnection, insertPrintSQL.c_str(), NULL, 0, &errorMessage);
}

//method to insert assign into the database || MAV IS LEFT HAND SIDE                UAV IS RIGHT HAND SIDE 
void Database::insertAssign(int LineNo, string assignLHS, string assignRHS) {
	string insertAssignSQL = "INSERT INTO assign ('assignLine','UAV','MAV') VALUES ('" + to_string(LineNo) + "','" + assignRHS + "','" + assignLHS + "');";
	sqlite3_exec(dbConnection, insertAssignSQL.c_str(), NULL, 0, &errorMessage);
}
void Database::insertAssignMSVR(int LineNo ,string assignRHS) {
	string insertAssignSQL = "UPDATE assign SET MSVR = '" + assignRHS +"' WHERE assign.assignLine = '" + to_string(LineNo) + "';";
	sqlite3_exec(dbConnection, insertAssignSQL.c_str(), NULL, 0, &errorMessage);
}

//method to insert to block table  || RHS of the '=' statement  which is also only equals will be counted as modify  
void Database::insertBlockUPV(int LineNo,string UPV, int BlockID ) {
	string insertBlockUPVSQL = "UPDATE ProcBlock SET UPV = '" + UPV + "' WHERE LineNo = '" + to_string(LineNo) + "';";
	sqlite3_exec(dbConnection, insertBlockUPVSQL.c_str(), NULL, 0, &errorMessage);
}

void Database::insertBlockMPV(int LineNo, string MPV, int BlockID) {
	string insertBlockMPVSQL = "INSERT INTO ProcBlock ('LineNo','MPV','BlockID') VALUES ('" + to_string(LineNo) + "','" + MPV + "','" + to_string(BlockID) + "');";
	sqlite3_exec(dbConnection, insertBlockMPVSQL.c_str(), NULL, 0, &errorMessage);
}


//method to insert pattern into the database || pLHS IS LEFT HAND SIDE                pRHS IS RIGHT HAND SIDE 
void Database::insertPattern(int LineNo, string patternLHS, string patternRHS) {
	string insertPatternSQL = "INSERT INTO pattern ('patternLine','pLHS','pRHS') VALUES ('" + to_string(LineNo) + "','" + patternLHS + "','" + patternRHS + "');";
	sqlite3_exec(dbConnection, insertPatternSQL.c_str(), NULL, 0, &errorMessage);
}

// method to get all the procedures from the database || add 
void Database::getProcedures(vector<string>& results) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getProceduresSQL = "SELECT procedureName FROM procedures;";
	sqlite3_exec(dbConnection, getProceduresSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		string result;
		result = dbRow.at(0);
		results.push_back(result);
	}
}

// method to get all the variable from the database || add 
void Database::getVariable(vector<string>& results) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getVariableSQL = "SELECT variableName FROM variable;";
	sqlite3_exec(dbConnection, getVariableSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		string result;
		result = dbRow.at(0);
		results.push_back(result);
	}
}

// method to get all the statement from the database || add 
void Database::getStatement(vector<string>& results) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getStatementSQL = "select stmtline from stmt order by cast(stmtline as int)";
	sqlite3_exec(dbConnection, getStatementSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		string result;
		result = dbRow.at(0);
		results.push_back(result);
	}
}


void Database::getWhile(vector<string>& results) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getStatementSQL = "select stmtline from stmt where stmt.while >0 order by cast(stmtline as int);";
	sqlite3_exec(dbConnection, getStatementSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		string result;
		result = dbRow.at(0);
		results.push_back(result);
	}
}


void Database::getIf(vector<string>& results) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getStatementSQL = "select stmtline from stmt where stmt.If >0 order by cast(stmtline as int);";
	sqlite3_exec(dbConnection, getStatementSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		string result;
		result = dbRow.at(0);
		results.push_back(result);
	}
}



// method to get all the constant from the database || add 
void Database::getConstant(vector<string>& results) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getConstantSQL = "SELECRT DISTINCT (constValue) FROM constant;";
	sqlite3_exec(dbConnection, getConstantSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		string result;
		result = dbRow.at(0);
		results.push_back(result);
	}
}



// method to get all the read from the database || add 
void Database::getRead(vector<string>& results) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getReadSQL = "select readLine from read order by cast(readLine as int)";
	sqlite3_exec(dbConnection, getReadSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		string result;
		result = dbRow.at(0);
		results.push_back(result);

	}
}



void Database::CheckStmt(vector<string>& results, string input) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getReadSQL = "select stmtLine from stmt WHERE stmtLine = '" + input + "';";
	sqlite3_exec(dbConnection, getReadSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		string result;
		result = dbRow.at(0);
		results.push_back(result);

	}
}


void Database::CheckStmt_LHS_WildCard(vector<string>& results, string input) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getReadSQL = "select stmtLine from stmt WHERE stmtLine = '" + input + "';";
	sqlite3_exec(dbConnection, getReadSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		string result;
		result = dbRow.at(0);
		results.push_back(result);

	}
}



void Database::CheckWhile(vector<string>& results, string input) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getReadSQL = "select While from stmt WHERE stmtLine = '" + input + "';";
	sqlite3_exec(dbConnection, getReadSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		string result;
		result = dbRow.at(0);
		results.push_back(result);

	}
}

void Database::CheckIf(vector<string>& results, string input) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getReadSQL = "select If from stmt WHERE stmtLine = '" + input + "';";
	sqlite3_exec(dbConnection, getReadSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		string result;
		result = dbRow.at(0);
		results.push_back(result);

	}
}


void Database::CheckRead(vector<string>& results,string input) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getReadSQL = "select readLine from read WHERE readline = '" + input + "';";
	sqlite3_exec(dbConnection, getReadSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		string result;
		result = dbRow.at(0);
		results.push_back(result);

	}
}

void Database::CheckPrint(vector<string>& results,string input) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getReadSQL = "select printLine from print WHERE printLine = '" + input + "';";
	sqlite3_exec(dbConnection, getReadSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		string result;
		result = dbRow.at(0);
		results.push_back(result);

	}
}

void Database::CheckAssign(vector<string>& results, string input) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getReadSQL = "select assignLine from assign WHERE assignLine = '" + input + "';";
	sqlite3_exec(dbConnection, getReadSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		string result;
		result = dbRow.at(0);
		results.push_back(result);

	}
}
// method to get all the print from the database || add 
void Database::getPrint(vector<string>& results) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getPrintSQL = "select printLine from print order by cast(printLine as int)";
	sqlite3_exec(dbConnection, getPrintSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		string result;
		result = dbRow.at(0);
		results.push_back(result);
	}
}

// method to get all the assign from the database || add 
void Database::getAssign(vector<string>& results) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getAssignSQL = "select assignLine from assign order by cast(assignLine as int)";
	sqlite3_exec(dbConnection, getAssignSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		string result;
		result = dbRow.at(0);
		results.push_back(result);
	}
}

// ### PATTERN SQL QUERY #####
void Database::getPattern(vector<string>& results) { // this is for pattern line only 
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getAssignSQL = "SELECT PatternLine FROM pattern;";
	sqlite3_exec(dbConnection, getAssignSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		string result;
		result = dbRow.at(0);
		results.push_back(result);
	}
}

void Database::getPattern_Assign_LHS(vector<string>& results , string LHS) { // this is for pattern line only 
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getAssignSQL = "SELECT assignLine FROM assign WHERE UAV = '" + LHS + "';";
	sqlite3_exec(dbConnection, getAssignSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		string result;
		result = dbRow.at(0);
		results.push_back(result);
	}
}

bool Database::varCheck(vector<string>& results, string varChecker) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan
	
		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string variableCheck = "SELECT variableName FROM variable;" ;
	sqlite3_exec(dbConnection, variableCheck.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		string result;
		result = dbRow.at(0);
		if (result == varChecker) {

			return true;
		}
		
	}
	return false;
}


bool Database::blockUPVCheck(vector<string>& results, string UPVChecker, int BlockID) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string variableCheck = "SELECT BlockID FROM ProcBlock WHERE UPV = '" + UPVChecker + "';";
	sqlite3_exec(dbConnection, variableCheck.c_str(), callback, 0, &errorMessage);

	for (vector<string> dbRow : dbResults) {
	
		
			if (dbRow.at(0) == UPVChecker) {
				return true;
			}
	

	}
	return false;
}



// ########################### P Q L   S I D E   Q U E R Y   G E T T E R S  #############



// ################################## SYN  #######################

void Database::getSynSyn_Parent(vector<string>& results) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getPrintSQL = "SELECT DISTINCT(stmt.Parent) FROM stmt WHERE stmt.Parent <> '0';";
	sqlite3_exec(dbConnection, getPrintSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		string result;
		result = dbRow.at(0);
		results.push_back(result);
	}
}

void Database::getSynSyn_Parent_Read(vector<string>& results) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getPrintSQL = "SELECT rLine FROM(SELECT stmt.stmtLine AS sLine, stmt.Parent AS sPrnt, read.readLine AS rLine FROM stmt LEFT JOIN read ON sLine = rLine) WHERE sPrnt <> '0' AND  rLine <> 'NULL';";
	sqlite3_exec(dbConnection, getPrintSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		string result;
		result = dbRow.at(0);
		results.push_back(result);
	}
}


void Database::getSynSyn_Parent_Print(vector<string>& results) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getPrintSQL = "SELECT pLine FROM( SELECT stmt.stmtLine AS sLine, stmt.Parent AS sPrnt, print.printLine AS pLine FROM stmt LEFT JOIN print ON sLine = pLine) WHERE sPrnt <> '0' AND  pLine <> 'NULL';";
	sqlite3_exec(dbConnection, getPrintSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		string result;
		result = dbRow.at(0);
		results.push_back(result);
	}
}

void Database::getSynSyn_Parent_Assign(vector<string>& results) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getPrintSQL = "SELECT aLine FROM( SELECT stmt.stmtLine AS sLine, stmt.Parent AS sPrnt, assign.assignLine AS aLine FROM stmt LEFT JOIN assign ON sLine = aLine) WHERE sPrnt <> '0' AND aLine <> 'NULL';";
	sqlite3_exec(dbConnection, getPrintSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		string result;
		result = dbRow.at(0);
		results.push_back(result);
	}
}


void Database::getSynLine_Parent(vector<string>& results, string input) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getPrintSQL = "SELECT stmt.Parent FROM stmt WHERE stmtLine = '" + input + "';";
	sqlite3_exec(dbConnection, getPrintSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		string result;
		result = dbRow.at(0);
		results.push_back(result);
	}
}

//########################## LINE  ##########################

void Database::getLineSyn_Parent(vector<string>& results, string input) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getPrintSQL = "SELECT stmt.stmtLine FROM stmt WHERE stmt.Parent = '" + input + "';";
	sqlite3_exec(dbConnection, getPrintSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		string result;
		result = dbRow.at(0);
		results.push_back(result);
	}
}


void Database::getLineLine_Parent(vector<string>& results, string LHSinput,string RHSinput ) { // LHS RHS 
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getPrintSQL = "Select case when stmt.Parent = '0' then 'False' when stmt.Parent ='" + LHSinput + "' and stmt.stmtLine ='" + RHSinput + "' then 'True' else 'False' End as 'bool' from stmt where stmt.stmtline ='" + RHSinput + "'";
	sqlite3_exec(dbConnection, getPrintSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		string result;
		result = dbRow.at(0);
		results.push_back(result);
	}
}


// ############ WILDCARD#####################


// used for Wild+Syn and Wild+Wild 
void Database::getWildSyn_Parent(vector<string>& results) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getPrintSQL = "SELECT DISTINCT(stmt.Parent) FROM stmt WHERE stmt.Parent <> '0';";
	sqlite3_exec(dbConnection, getPrintSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		string result;
		result = dbRow.at(0);
		results.push_back(result);
	}
}

void Database::getWildLine_Parent(vector<string>& results,string input) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getPrintSQL = "SELECT stmt.Parent FROM stmt WHERE stmt.stmtLine = '" + input + "';";
	sqlite3_exec(dbConnection, getPrintSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		string result;
		result = dbRow.at(0);
		results.push_back(result);
	}
}


//##################################################    Modifies        ###########################################
// Used for Syn Syn // Syn Wildcard 
// Used for Wildcard and Syn /Wildcard Wildcard
void Database::getSynAssign_MAV(vector<string>& results) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getPrintSQL = "SELECT assignLine FROM assign WHERE MAV <> '0';";
	sqlite3_exec(dbConnection, getPrintSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		string result;
		result = dbRow.at(0);
		results.push_back(result);
	}
}

void Database::getSynRead_MRV(vector<string>& results) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getPrintSQL = "SELECT readLine FROM read WHERE MRV <> '0'; ";
	sqlite3_exec(dbConnection, getPrintSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		string result;
		result = dbRow.at(0);
		results.push_back(result);
	}
}

void Database::getSynStmt_MSV(vector<string>& results) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getPrintSQL = "SELECT stmtLine FROM stmt WHERE MSV <> '0';";
	sqlite3_exec(dbConnection, getPrintSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		string result;
		result = dbRow.at(0);
		results.push_back(result);
	}
}


void Database::getSynAssign_MAV_Varname(vector<string>& results,string input) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getPrintSQL = "SELECT assignLine FROM assign WHERE MAV = '" + input + "';";
	sqlite3_exec(dbConnection, getPrintSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		string result;
		result = dbRow.at(0);
		results.push_back(result);
	}
}

void Database::getSynRead_MRV_Varname(vector<string>& results, string input) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getPrintSQL = "SELECT readLine FROM read WHERE MRV = '" + input + "';";
	sqlite3_exec(dbConnection, getPrintSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		string result;
		result = dbRow.at(0);
		results.push_back(result);
	}
}

void Database::getSynStmt_MSV_Varname(vector<string>& results, string input) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getPrintSQL = "SELECT stmtLine FROM stmt WHERE MSV = '" + input + "';";
	sqlite3_exec(dbConnection, getPrintSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		string result;
		result = dbRow.at(0);
		results.push_back(result);
	}
}


void Database::getLineNoAssign_MAV(vector<string>& results, string input) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getPrintSQL = "SELECT assignLine FROM assign WHERE MAV <> '0' AND assignLine = '" + input + "';";
	sqlite3_exec(dbConnection, getPrintSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		string result;
		result = dbRow.at(0);
		results.push_back(result);
	}
}

void Database::getLineNoRead_MRV(vector<string>& results, string input) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getPrintSQL = "SELECT readLine FROM read WHERE MRV <> '0' AND readLine = '" + input + "';";
	sqlite3_exec(dbConnection, getPrintSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		string result;
		result = dbRow.at(0);
		results.push_back(result);
	}
}


void Database::getLineNoStmt_MSV(vector<string>& results, string input) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getPrintSQL = "SELECT stmtLine FROM stmt WHERE MSV <> '0' AND stmtLine = '" + input + "';";
	sqlite3_exec(dbConnection, getPrintSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		string result;
		result = dbRow.at(0);
		results.push_back(result);
	}
}



//							#################################  USES ######################################

// UAV  SYN SYN Stmt 



// UAV SYN SYN Assign

void Database::getSynSyn_UAV_Assign(vector<string>& results) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getPrintSQL = "SELECT assign.assignLine FROM assign WHERE assign.UAV <> '0';";
	sqlite3_exec(dbConnection, getPrintSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		string result;
		result = dbRow.at(0);
		results.push_back(result);
	}
}

// UAV SYN COUNT Assign 

void Database::getSynCount_UAV_Assign(vector<string>& results, string input) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getPrintSQL = "SELECT assignLine FROM assign WHERE assign.UAV = '" + input + "';";
	sqlite3_exec(dbConnection, getPrintSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		string result;
		result = dbRow.at(0);
		results.push_back(result);
	}
}



void Database::getSynSyn_UPV_Print(vector<string>& results) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getPrintSQL = "SELECT printLine FROM print;";
	sqlite3_exec(dbConnection, getPrintSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		string result;
		result = dbRow.at(0);
		results.push_back(result);
	}
}


void Database::getSynSyn_MAV_Stmt(vector<string>& results) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan
		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getPrintSQL = "SELECT assignLine FROM assign WHERE MSVR NOT NULL";
	sqlite3_exec(dbConnection, getPrintSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		string result;
		result = dbRow.at(0);
		results.push_back(result);
	}
}


void Database::getSynCount_UPV_Print(vector<string>& results, string input) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getPrintSQL = "SELECT print.UPNV FROM print WHERE print.UPNV = '" + input + "';";
	sqlite3_exec(dbConnection, getPrintSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		string result;
		result = dbRow.at(0);
		results.push_back(result);
	}
}




void Database::getLineSyn_UAV_Assign(vector<string>& results) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getPrintSQL = "SELECT assign.assignLine FROM assign WHERE assign.UAV <> '0';";
	sqlite3_exec(dbConnection, getPrintSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		string result;
		result = dbRow.at(0);
		results.push_back(result);
	}
}


void Database::getLineSyn_UAV_Variable(vector<string>& results, string input) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getPrintSQL = "SELECT assign.UAV FROM assign WHERE assign.assignLine = '" + input + "'; ";
	sqlite3_exec(dbConnection, getPrintSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		string result;
		result = dbRow.at(0);
		results.push_back(result);
	}
}


//UPNV 


void Database::getLineSyn_UPV_Print(vector<string>& results, string input) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getPrintSQL = "SELECT print.UPNV FROM print WHERE printLine = '" + input + "'; ";
	sqlite3_exec(dbConnection, getPrintSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		string result;
		result = dbRow.at(0);
		results.push_back(result);
	}
}


void Database::getLineSyn_UPV_Variable(vector<string>& results, string input) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getPrintSQL = "SELECT UPNV FROM print WHERE UPVN = '" + input + "'; ";
	sqlite3_exec(dbConnection, getPrintSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		string result;
		result = dbRow.at(0);
		results.push_back(result);
	}
}

//  ############################################# // NEXT ################################
void Database::getNext_SYN_SYN_stmt(vector<string>& results) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getPrintSQL = "select stmtline from stmt order by cast(stmtline as int);";
	sqlite3_exec(dbConnection, getPrintSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	vector<vector<string>>::iterator ptr;
	string result;
	string prev = "-1";
	for (int i = 0; i < dbResults.size(); ++i)
	{
		for (int j = 0; j < dbResults[i].size(); ++j)
		{
			result = dbResults[i][j];
			j++;
			int r = stoi(result);
			int p = stoi(prev);
			p += 1;
			if (r ==p ) {
				results.push_back(result);

			}
			 prev = result;
			
		}
		
	}
}


void Database::getNext_SYN_SYN_stmt_loop(vector<string>& results) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getPrintSQL = "select stmtline,parent from stmt order by cast(stmtline as int);";
	sqlite3_exec(dbConnection, getPrintSQL.c_str(), callback, 0, &errorMessage);
	string prevresult = "-1";
	string nextParent;
	string nextstatement;
	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		nextstatement = dbRow.at(0);
		nextParent = dbRow.at(1);
		// casting 
		int prevstatement = stoi(prevresult);
		int parent = stoi(nextParent);
		int next = stoi(nextstatement);

			if ((next == prevstatement + 1) && parent != 0) {
				results.push_back("1");
				
			}

		prevresult = dbRow.at(0);
		
		
	}
}


// this is for read, print , assign for LHS of next
void Database::getNext_SYN_SYN_read(vector<int>& resultsNew,vector<string>& results) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getPrintSQL = "select stmtline from stmt order by cast(stmtline as int);";
	sqlite3_exec(dbConnection, getPrintSQL.c_str(), callback, 0, &errorMessage);
	
	int i = 0;
	// first loop 
	string compare;
	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		int test =resultsNew.at(i);
		int dbrowint = stoi(dbRow.at(0));
		if (test < dbrowint) {
			
			// move on 

			i++;
			if (i>=resultsNew.size()) {
				break;
			}

		}
		compare = to_string(resultsNew.at(i));
		if (compare == dbRow.at(0)){
			results.push_back("1");
		}
	

	
	}
}



void Database::getNext_SYN_SYN_read_while(vector<int>& resultsNew, vector<string>& results) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getPrintSQL = "select stmtline,while from stmt order by cast(stmtline as int);";
	sqlite3_exec(dbConnection, getPrintSQL.c_str(), callback, 0, &errorMessage);

	int i = 0;
	// first loop 
	string compare;
	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		int test = resultsNew.at(i);
		int dbrowint = stoi(dbRow.at(0));
		if (test < dbrowint) {

			// move on 

			i++;
			if (i >= resultsNew.size()) {
				break;
			}

		}
		compare = to_string(resultsNew.at(i));
		if (compare == dbRow.at(0) && (dbRow.at(1) == "1")) {
			results.push_back("1");
		}



	}
}


void Database::getNext_SYN_SYN_read_if(vector<int>& resultsNew, vector<string>& results) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getPrintSQL = "select stmtline,if from stmt order by cast(stmtline as int);";
	sqlite3_exec(dbConnection, getPrintSQL.c_str(), callback, 0, &errorMessage);

	int i = 0;
	// first loop 
	string compare;
	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		int test = resultsNew.at(i);
		int dbrowint = stoi(dbRow.at(0));
		if (test < dbrowint) {

			// move on 

			i++;
			if (i >= resultsNew.size()) {
				break;
			}

		}
		compare = to_string(resultsNew.at(i));
		if (compare == dbRow.at(0) && (dbRow.at(1) == "1")) {
			results.push_back("1");
		}



	}
}
/// PATTERN 



void Database::getNext_SYN_SYN_while_read(vector<int>& resultsNew, vector<string>& results) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getPrintSQL = "select stmtline,while from stmt order by cast(stmtline as int);";
	sqlite3_exec(dbConnection, getPrintSQL.c_str(), callback, 0, &errorMessage);
	int i = 0;
	// first loop 
	string compare;
	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		int test = resultsNew.at(i);
		int dbrowint = stoi(dbRow.at(0));
		if (test < dbrowint) {

			// move on 

			i++;
			if (i >= resultsNew.size()) {
				break;
			}

		}
		compare = to_string(resultsNew.at(i));
		if (compare == dbRow.at(0) && (dbRow.at(1) == "0")) {
			results.push_back(dbRow.at(0));
		}



	}
}


void Database::getNext_SYN_SYN_if_read(vector<int>& resultsNew, vector<string>& results) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getPrintSQL = "select stmtline,if from stmt order by cast(stmtline as int);";
	sqlite3_exec(dbConnection, getPrintSQL.c_str(), callback, 0, &errorMessage);
	int i = 0;
	// first loop 
	string compare;
	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		int test = resultsNew.at(i);
		int dbrowint = stoi(dbRow.at(0));
		if (test < dbrowint) {

			// move on 

			i++;
			if (i >= resultsNew.size()) {
				break;
			}

		}
		compare = to_string(resultsNew.at(i));
		if (compare == dbRow.at(0) && (dbRow.at(1) == "0")) {
			results.push_back("1");
		}



	}
}

// HAVE LHS AND RHS 
void Database::QueryPatternTree_With_LHS(vector<string>& results, string LHS ) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getPrintSQL = "SELECT patternLine,pRHS FROM pattern WHERE pattern.pLHS = '" + LHS + "'; ";
	sqlite3_exec(dbConnection, getPrintSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		string result; // this will take the patterns
		
		// result is pattern  || input is LHS of the equation 
		result = dbRow.at(1);
		results.push_back(result);
		result = dbRow.at(0);
		results.push_back(result);
	}
}





void Database::QueryPatternTree_Only_RHS(vector<string>& results) {
	// clear the existing results
	dbResults.clear();
	// method to get all the variable from the database  || added 24th jan

		// retrieve the procedures from the procedure table
		// The callback method is only used when there are results to be returned.
	string getPrintSQL = "SELECT patternLine,pRHS FROM pattern; ";
	sqlite3_exec(dbConnection, getPrintSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		string result; // this will take the patterns

		// result is pattern  || input is LHS of the equation 
		result = dbRow.at(1);
		results.push_back(result);
		result = dbRow.at(0);
		results.push_back(result);
	}
}


// callback method to put one row of results from the database into the dbResults vector
// This method is called each time a row of results is returned from the database
int Database::callback(void* NotUsed, int argc, char** argv, char** azColName) {
	NotUsed = 0;
	vector<string> dbRow;

	// argc is the number of columns for this row of results
	// argv contains the values for the columns 
	// Each value is pushed into a vector.

	for (int i = 0; i < argc; i++) {
		if (argv[i] == NULL) {
			cout << "NULLCHECK" << endl;
			continue;

		}
		dbRow.push_back(argv[i]);
	}

	// The row is pushed to the vector for storing all rows of results 
	dbResults.push_back(dbRow);

	return 0;
}


int Database::callback2(void* NotUsed, int argc, char** argv, char** azColName) {
	NotUsed = 0;
	vector<string> dbRow;

	// argc is the number of columns for this row of results
	// argv contains the values for the columns 
	// Each value is pushed into a vector.

	for (int i = 0; i < argc; i++) {
		if (argv[i] == NULL) {
			cout << "NULLCHECK" << endl;
			continue;

		}
		dbRow.push_back(argv[i]);
	}

	// The row is pushed to the vector for storing all rows of results 
	dbResults.push_back(dbRow);

	return 0;
}