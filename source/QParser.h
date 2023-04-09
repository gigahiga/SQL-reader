#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <stack>
using namespace std;


class Node
{
    //stack<Node*> s;
public:
    stack<Node*> s;
    char data;
    Node* left;
    Node* right;
    Node(char data) {
        this->data = data;
        this->left = this->right = nullptr;
    }
    Node(char data, Node* left, Node* right)
    {
        this->data = data;
        this->left = left;
        this->right = right;
    }
    
    bool isOperator(char c);
    int construct(string postfix);
    //bool areIdentical(Node* root1, Node* root2);
    //bool isSubtree(Node* T, Node* S);

    Node();

    // destructor
    ~Node();


};


enum class QClause{

    cdefaultStart,      // nothing
    csuchthat,          // purely 1 such that
    cpattern,           // purely 1 pattern
    csuchthatpattern,   // such that then pattern
    csuchthatsuchthat,  // dbl stack suchthat
    cpatternsuchthat,   // pattern then such that
    cpatternpattern     // dbl stack pattern    
};

enum class QSymbol {

// PQL is case sensitive 
// 'stmt' | 'read' | 'print' | 'call' | 
// 'while' | 'if' | 'assign' | 'variable' | 
// 'constant' | 'procedure'  
// 
//--- declarations -----------
    qprocedure,     // "procedure"    
    qassign,        // "assign" 
    qprint,         // "print"
    qread,          // "read"
    qstmt,          // "stmt"
    qvariable,      // 'variable'
    qconstant,      // 'constant' 
    qcall,          // 'call'
    qwhile,         // 'while'
    qif,            // 'if'    

//---     
    qcomma,         // , multi-synonym 
    qbracketOpen,   // (
    qbracketClose,  // )
    qdblquote,      // "
    qsemicolon,     // ; 
    qselect,        // select    
    qsuch,          // such 
    qthat,          // that
    qpattern,       // pattern
    qwildcard,      // _ underscore
    qwildcardBrackOpen,     // (_
    qwildcardBrackClose,    // _)
    qangleOpen,     // <
    qangleClose,    // >
// ---     
    qintLineNo,     // integer - line no
    qsynonym,       // synonym

// ---
// The "S" suffix means that the relationship applies to 
// all statements except calls. Therefore, for iteration 2, 
// you do not need to consider modifies/uses by 
// procedure/procedure call.

    qModifies,      // ModifiesS     
    qModifiesP,     // Modifies*     
    qUses,          // UsesS 
    qUsesP,         // UsesP
    qParent,        // Parent
    

// If a relationship has "T" as the suffix, you may assume 
// that it refers to the star variant of the relationship without a "T". 
// For example, ParentT refers to the Parent* relationship.   

    qParentT,       // ParentT
    qNext,          // Next ITR3
    qNextT,         // NextT ITR3
    qCalls,         // Calls
    qCallsT,       // Calls*  
//---
    qdefaultStart,  // marker
    qtokensRanOut   // end of query stmt
};

class QueryParser {

    QSymbol         qsym;
    QClause         qsearchCl; 
    QSymbol         qDeclType;
    string          qtblName;
    bool            qside; // 1 = left; 0 = right
    QSymbol         qLHS, qRHS;
    string          qLHSstring, qRHSstring;
    int             qtokencounter;
    vector<string>  qdbResults;
    map<QSymbol,string> qkey_map;               // init the map
    map<string, string> PQL_select;             // FORMAT IN MAP IS     "a"   "assign" 
    map<string, QSymbol> declMap;               // xfer from PQL_Select to declMap
    map<string, string> syn_map;                // init the synonym map 
    bool            fetch2tokens;               // when hit select i want it to stop running PQL_select map inserts 
    string          select_variable;            // this is for what comes AFTER select for our query 
    string          select_variable_type;       // for select 
    string          select_variable_type_LHS;   // for the exact type on LHS
    string          select_variable_type_RHS;   // for the exact type on RHS 
    QSymbol         LHStype;                    // 
    QSymbol         RHStype;
    bool            Select_Result_Insert;
    string          QuoteInput;
    vector <string> pRHS;
    vector <string> pLHS; 
    Node            Tree;
    Node            SubTree;
    

public:
    // default constructor
    QueryParser();

    // destructor
    ~QueryParser();

    int queryParse(vector<string>& tokens);

    int fetchQToken(vector<string>& tokens);
    int fetchQDeclToken(vector<string>& tokens);
#if 0
    int 
    (vector<string>& tokens);
    int statementList(vector<string>& tokens);  // if we need to add the exact string into the db need to change
                                                // ** ITR2_remove arg int nextToken - not used  

    int relExpr(vector<string>& tokens);     // ** ITR2_add
    int relFactor(vector<string>& tokens);   // ** ITR2_add
    int expr(vector<string>& tokens);        // ** ITR2_add
    int exprPrime(vector<string>& tokens);   // ** ITR2_add
    int term(vector<string>& tokens);        // ** ITR2_add
    int termPrime(vector<string>& tokens);   // ** ITR2_add
    int factor(vector<string>& tokens);      // ** ITR2_add
#endif
    int initQMap(void);

    int declaration(vector<string>& tokens);

    int resultCl(vector<string>& tokens);

    int noSuchThatQuery(vector<string>& token);


    bool areIdentical(Node* root1, Node* root2);
    bool isSubtree(Node* T, Node* S);


    int isTwoSyn(vector<string>& token);

    int isSymInPQLSelect(vector<string>& tokens);

    bool isSelectValidVariable(vector<string>& tokens, int counter);

    bool isQsymDeclaredSynonym(vector<string>& tokens);

    string infixToPostfix(string input);

    int prec(char input);

    void set_Select_variable_LHS(vector<string>& tokens);
    
    void set_Select_variable_RHS(vector<string>& tokens);

    void set_Select_variable(vector<string>& tokens);

    int isSuchThat(vector<string>& tokens);
    int suchThat(vector<string>& tokens);
    int pattern(vector<string>& tokens);

    int relRef(vector<string>& tokens);
    
    int stmtRef(vector<string>& tokens);

    int entRef(vector<string>& tokens);

    //add modifies/parents/...etc
    int modifies(vector<string>& tokens);
    int modifiesLsynRsynW(vector<string>& tokens);
    int modifiesLsynRdquote(vector<string>& tokens);
    int modifiesP(vector<string>& tokens);

    int uses(vector<string>& tokens);
    int usesP(vector<string>& tokens);
    int usesLsyn(vector<string>& tokens);
    int usesLlineno(vector<string>& tokens);
    int usesLwildcard(vector<string>& tokens);

    int parent(vector<string>& tokens);
    int parentT(vector<string>& tokens);

    int next(vector<string>& tokens);
    int nextT(vector<string>& tokens);

    int calls(vector<string>& tokens);
    int callsT(vector<string>& tokens);

    vector<string> getDBresults(){ return qdbResults;}

    void setLHS(void) { qside = 1; } // 1 = left; 0 = right
    void clearLHS(void) { qside = 0; }
    bool getLHS(void) { return qside; }

    void storeqLHS(void){ qLHS = qsym;}
    void clearqLHS(void){ qLHS = QSymbol::qdefaultStart;}
    QSymbol getqLHS(void){return qLHS;}

    void storeqRHS(void){ qRHS = qsym;}
    void clearqRHS(void){ qRHS = QSymbol::qdefaultStart;}
    QSymbol getqRHS(void){return qRHS;}

    void storeqLHSstring(string tok){ qLHSstring = tok;}
    void storeqRHSstring(string tok){ qRHSstring = tok;}

    string getqLHSstring(){ return qLHSstring;}
    string getqRHSstring(){ return qRHSstring;}

    void qFail(string msg) { cout << msg << endl;}
};
