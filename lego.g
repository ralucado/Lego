#header
<<
#include <string>
#include <iostream>
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
/*  if (type == ID) {
    attr->kind = "id";
    attr->text = text;
  }
  else {*/
    attr->kind = text;
    attr->text = "";
//  }
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
#token ID "[a-zA-Z][a-zA-Z0-9]+"
#token END "ENDEF"
//...
#token SPACE "[\ \n]" << zzskip();>>

lego: grid ops defs <<#0=createASTlist(_sibling);>>;
//...
grid: GRID^ NUM NUM;

ops: (fact | moviment | bucle | height)* <<#0=createASTlist(_sibling);>>;

fact: ID ASIG^ (place | pp);

place: PLACE^ comp AT! comp;

pp: (comp | ID ) ((PUSH^ | POP^) pp | );

moviment: MOVE^ ID dir NUM;

bucle: WHILE^ LP! cond RP! LB! ops RB!;

cond: (fits | (atom reop atom)) (AND^ cond| );

atom: height | NUM;

comp: (LP!| ) NUM COMA! NUM (RP!| )<<#0=createASTlist(_sibling);>>;

height: HEIGHT^ LP! ID RP!;

fits: FITS^ LP! ID COMA! comp COMA! NUM RP!;

defs: ;

dir: NORTH | EAST | WEST | SOUTH;

reop: LT^ | GT^;