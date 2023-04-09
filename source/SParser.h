#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <map>
using namespace std;



// iteration1:SIMPLEparser
// A class to parse SIMPLE program
class SimpleParser {

    typedef enum {
        //  -------------  
        times,          // symbol *        
        plus,           // symbol +
        minus,          // symbol -
        equal,          // symbol =           
        lessThan,       // <
        greaterThan,    // >        
        semiColon,      // ;
        curlyOpen,      // {
        curlyClose,     // }        
        comma,          // :
        // -------------          
        divide,         // /  ** ITR2_add
        modulo,         // %  ** ITR2_add
        bracketOpen,    // (  ** ITR2_add
        bracketClose,   // )  ** ITR2_add
        logicAnd,       // &&
        logicOr,        // ||
        notEqual,       // !=
        greaterEqual,    // >=
        lessEqual,       // <=  // might want to consider !function
        doubleEqual,    // ==




        // -------------          
        letter,         // A-Z, a-z
        digit,          // 0-9
        const_value,    // integer
        var_name,       // name
        proc_name,      // name        
        name,           // NAME : LETTER (LETTER | DIGIT)*
#if 0
        // -------------  ** ITR2_remove
        factor,         // iteration1: var_name | const_value  ** ITR2_remove
        expr,           // expression: term exprPrime  **ITR2_update
        exprPrime,      // exprPrime: '+' term exprPrime | '-' term exprPrime | epsilon **ITR2_add
        relexpr,        // rel_expr: rel_factor ‘ > ’ rel_factor | rel_factor ‘ < ’ rel_factor
        term,           // term: factor termPrime **ITR2_update
        termPrime,      // termPrime: '*' factor termPrime | '/' factor termPrime |  '%' factor termPrime | epsilon
        rel_factor,     // rel_factor: var_name | const_value | expr
#endif
        // -------------   keywords        
        keyProcedure,   // "procedure"
        keyAssign,      // "assignment" 
        keyPrint,       // "print"
        keyRead,        // "read"
        stmt,           //  
        stmtLst,        //
        // -------------  ** ITR2_add
        keyWhile,      // "while" ** ITR2_change
        keyIf,         // "if"    ** ITR2_change
        keyThen,       // "then"  ** ITR2_add 
        keyElse,       // "else"  ** ITR2_add 
        // -------------
        defaultStart    // start
    } Symbol;

    Symbol  sym;
    string  errString;
    int     lineNo;
    int     tokencounter;
    int     start_expression;
    int     end_expression;
    map     <string, Symbol> key_map; // init the map
    map     <string, Symbol>  logic_map; // logical operator map 
    vector  <string> expression_result; // final result of infix to postix (will change to prefix)
    int     blockcounter;
    int     parentLine;
    int     procID = 1;
    string  patternRHS;
    int     pattern_before_equal;
    bool    real_expression;

public:
    // default constructor
    SimpleParser();

    // destructor
    ~SimpleParser();
    //ITR 1 
    int simpleparse(vector<string>& tokens);
    int lookAhead(vector<string>& tokens, Symbol look_sym);
    int procBlock(vector<string>& tokens);
    int fetchToken(vector<string>& tokens);
    int statementList(vector<string>& tokens, int nexttoken);  // if we need to add the exact string into the db need to change

    int relExpr(vector<string>& tokens);     // ** ITR2_add
    int expr(vector<string>& tokens);        // ** ITR2_add
    int relFactor(vector<string>& tokens);   // ** ITR2_add



    int exprPrime(vector<string>& tokens, int nexttoken);   // ** ITR2_add
    int term(vector<string>& tokens, int nexttoken);        // ** ITR2_add
    int termPrime(vector<string>& tokens, int nexttoken);   // ** ITR2_add
    int factor(vector<string>& tokens, int nexttoken);      // ** ITR2_add
    int prec(char expressionResult);
    int expression(vector<string>& tokens);




    int initMap();


    static void FailExit(string errString) {
        cout << errString << endl; // do we need to tell the auto tester or what kind of error message should we put 
    };
};
