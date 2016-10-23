#header
<<
#include <string>
#include <iostream>
#include <vector>
#include <map>
using namespace std;

// struct to store information about tokens
typedef struct {
  string kind;
  string text;
} Attrib;



// function to fill token information (predeclaration)
void zzcr_attr(Attrib *attr, int type, char *text);

// fields for AST nodes
#define AST_FIELDS string kind; string text;
#include "ast.h"

// macro to create a new AST node (and function predeclaration)
#define zzcr_ast(as,attr,ttype,textt) as=createASTnode(attr,ttype,textt)
AST* createASTnode(Attrib* attr,int ttype, char *textt);
>>

<<
#include <cstdlib>
#include <cmath>

//global structures
AST *root;


// function to fill token information
void zzcr_attr(Attrib *attr, int type, char *text) {
 /* if (type == ID) {
    attr->kind = "id";
    attr->text = text;
  }
  else {*/
    attr->kind = text;
    attr->text = "";
//}
}

// function to create a new AST node
AST* createASTnode(Attrib* attr, int type, char* text) {
  AST* as = new AST;
  as->kind = attr->kind;
  as->text = attr->text;
  as->right = NULL;
  as->down = NULL;
  return as;
}


/// create a new "list" AST node with one element
AST* createASTlist(AST *child) {
 AST *as=new AST;
 as->kind="list";
 as->right=NULL;
 as->down=child;
 return as;
}

/// get nth child of a tree. Count starts at 0.
/// if no such child, returns NULL
AST* child(AST *a,int n) {
AST *c=a->down;
for (int i=0; c!=NULL && i<n; i++) c=c->right;
return c;
}



/// print AST, recursively, with indentation
void ASTPrintIndent(AST *a,string s)
{
  if (a==NULL) return;

  cout<<a->kind;
  if (a->text!="") cout<<"("<<a->text<<")";
  cout<<endl;

  AST *i = a->down;
  while (i!=NULL && i->right!=NULL) {
    cout<<s+"  \\__";
    ASTPrintIndent(i,s+"  |"+string(i->kind.size()+i->text.size(),' '));
    i=i->right;
  }

  if (i!=NULL) {
      cout<<s+"  \\__";
      ASTPrintIndent(i,s+"   "+string(i->kind.size()+i->text.size(),' '));
      i=i->right;
  }
}

///structs per el programa
typedef struct {
    int x, y; // punt superior esquerra
    int h, w; // dimensions
} tblock;

typedef struct {
    int n, m;
    vector< vector <int> > height;
    map <string, tblock> blocks;
} Graella;

//predeclaracio de coses
void doOperations(AST *a);
int retHeight(AST* a);

Graella g;

//aqui posem el punter a la definicio de cada funcio
//del programa per tal de executarla quan faci falta
map <string, AST*> functions;

void createGrid(AST *a){
 g.n = atoi(child(a, 0)->kind.c_str());
 g.m = atoi(child(a, 1)->kind.c_str());
 g.height = vector< vector<int> >(g.m, vector<int>(g.n, 0));
}

void defineFunctions(AST *a){
    int i = 0;
    AST *actual = child(a, i);
    while(actual != NULL){

        functions.insert(pair<string, AST*>( child(actual,0)->kind, child(actual,1)));
        ++i;
        actual = child(a,i);
    }
    map<string,AST*>::iterator it;
    for (it = functions.begin(); it != functions.end(); ++it){
        cout << it->first << "->" << it->second->kind << endl;
    }

}


int contCond = 0;
bool fits(AST* a){
    ++contCond;
    bool b = contCond != 2;
    cout << "Does " << a->kind << "\'s son fit? " << b << endl;
    return b;
}

int ievaluaCond(AST* a){
    if(a->kind == "AND"){
        return ievaluaCond(child(a,0)) && ievaluaCond(child(a,1));
    }
    else if (a->kind == "<"){
        return ievaluaCond(child(a,0)) < ievaluaCond(child(a,1));
    }
    else if (a->kind == ">"){
        return ievaluaCond(child(a,0)) > ievaluaCond(child(a,1));
    }
    else if (a->kind == "HEIGHT") return retHeight(a);
    else return atoi(a->kind.c_str());
}

bool evaluaCond(AST* a){
    cout << "avere com faig aixo sense morirme" << endl;
    if(a->kind == "AND"){
        return bool(ievaluaCond(child(a,0))) && bool(ievaluaCond(child(a,1)));
    }
    else if (a->kind == "FITS") return fits(child(a,0));
    else if (a->kind == "<"){
        return ievaluaCond(child(a,0)) < ievaluaCond(child(a,1));
    }
    else if (a->kind == ">"){
        return ievaluaCond(child(a,0)) > ievaluaCond(child(a,1));
    }
    else{
        cerr << "LA CONDICIO HA DE CONTENIR ALMENYS UN OPERADOR BINARI" << endl;
        exit(1);
    }
}

void asignacio(AST* a){
    cout << "el token asig es de " << a->kind << endl;
}

void move(AST* a){
    cout << "el token move es de " << a->kind << endl;
}

void bucle(AST* a){
    //contCond = 0;
    cout << "el token bakel es de " << a->kind << endl;
    while (evaluaCond(child(a,0))){
        doOperations(child(a,1));
    }
}

int h1 = 1, h2 = 2;
bool count = false;
int retHeight(AST* a){
    cout << "volem saber la alçada de " << child(a,0)->kind <</* " i count es "<< count << */endl;
/*
    count = !count;
    if (count) return h1;
    return h2;*/
    return 0;
}

void execFunc(AST* a){
    cout << "el token execfunc es de " << a->kind << endl;
    map <string, AST*>::iterator it;
    it = functions.find(a->kind);
    if(it != functions.end()) doOperations(functions[a->kind]);
    else{
        cerr << "THE FUNCTION " << a->kind << " IS NOT DEFINED" << endl;
        exit(1);
    }
}



void doOperations(AST *a){
//asig
//move
//while
//height
//def
int i = 0;
AST* actual = child(a,i);
    while(actual != NULL){
        if (actual->kind == "=") asignacio(actual);
        else if (actual->kind == "MOVE") move(actual);
        else if (actual->kind == "WHILE") bucle(actual);
        else if (actual->kind == "HEIGHT") cout << retHeight(actual) << endl;
        else execFunc(actual);
        ++i;
        actual = child(a,i);
    }
}

///execute INSTR
void executeInstructions(AST *a){
    if(a != NULL){
        if(child(a, 0) != NULL) createGrid(child(a, 0));
        else{
            cerr << "THE PROGRAM MUST BEGIN BY A GRID DECLARATION" << endl;
            exit(1);
        }
        if(child(a, 2) != NULL) defineFunctions(child(a, 2));
        if(child(a, 1) != NULL) doOperations(child(a, 1));
    }
}

/// print AST
void ASTPrint(AST *a)
{
  while (a!=NULL) {
    cout<<" ";
    ASTPrintIndent(a,"");
    a=a->right;
  }
}

int main() {
  root = NULL;
  ANTLR(lego(&root), stdin);
  ASTPrint(root);
  executeInstructions(root);
}
>>

#lexclass START


#token NUM "[0-9]+"
#token GRID "Grid"
#token LP "\("
#token RP "\)"
#token LB "\["
#token RB "\]"
#token COMA "\,"
#token NORTH "NORTH"
#token EAST "EAST"
#token WEST "WEST"
#token SOUTH "SOUTH"
#token PLACE "PLACE"
#token AT "AT"
#token MOVE "MOVE"
#token PUSH "PUSH"
#token POP "POP"
#token WHILE "WHILE"
#token FITS "FITS"
#token HEIGHT "HEIGHT"
#token DEF "DEF"
#token GT "\>"
#token LT "\<"
#token AND "AND"
#token ASIG "\="
#token END "ENDEF"
#token ID "[a-zA-Z][a-zA-Z0-9]+"
//...
#token SPACE "[reop\ \n\b]" << zzskip();>>

lego: grid ops defs <<#0=createASTlist(_sibling);>>;
//...
ops: (fact | moviment | bucle | height)* <<#0=createASTlist(_sibling);>>;

bucle: WHILE^ LP! cond RP! LB! ops RB!;
///bucle: WHILE^ LP cond RP LB ops RB;

grid: GRID^ NUM NUM;

fact: ID ((ASIG^ (place | pp)) | );
place: PLACE^ comp AT! comp;
pp: (comp | ID ) ((PUSH^ | POP^) pp | );


moviment: MOVE^ ID dir NUM;

cond: (fits | comparacio) (AND^ cond| );
////cond: (fits | (atom reop atom)) (AND^ cosa)*;

comparacio: atom (LT^ | GT^) atom;

atom: height | NUM;

comp: (LP!| ) NUM COMA! NUM (RP!| )<<#0=createASTlist(_sibling);>>;

height: HEIGHT^ LP! ID RP!;

fits: FITS^ LP! ID COMA! comp COMA! NUM RP!;

defs: (def)*<<#0=createASTlist(_sibling);>>;

def: DEF^ ID ops END!;

dir: NORTH | EAST | WEST | SOUTH;

reop: LT^ | GT^;

////fact: ID ((ASIG^ (asig)) | );
/////asig: asigop | place;
////asigop: (ID | comp) (PUSH^ | POP^) asigopre;
////asigopre: (ID | comp) (((PUSH^ | POP^) asigopre) | );
