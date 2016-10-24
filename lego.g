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
    map <string, vector<string> > apilats;
} Graella;

//predeclaracio de coses
void doOperations(AST *a);
int retHeight(AST* a);
int heightByName(string nom);
bool flat(int x, int y, int h, int w, int ground);

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
    /*for (it = functions.begin(); it != functions.end(); ++it){
        cout << it->first << "->" << it->second->kind << endl;
    }*/

}
int fitsX, fitsY;
bool Bfits(int tamX, int tamY, int x, int y, int h, int w, int nivell){
    bool found = false;
    int i = y, j=x;
    while (not found && i < y+h){
        found = flat(i,j,tamY,tamX,nivell);
        if(found){
            fitsX = j;
            fitsY = i;
            return true;
        }
        ++j;
        if(j % (x+w) == 0){
            j = x;
            ++i;
        }
    }
    return false;
}

bool fits(AST* a){
    //cout << "maria fits" << endl;
    string nom = child(a,0)->kind;
    //cout << nom << endl;
    int tamX = atoi(child(child(a,1), 0)->kind.c_str());
    int tamY = atoi(child(child(a,1), 1)->kind.c_str());
    int nivell = atoi(child(a, 2)->kind.c_str());
    //cout << " alpaca!" << endl;
    map<string,tblock>::iterator it;
    it = g.blocks.find(nom);
    tblock bloc = it->second;
    return Bfits(tamX, tamY, bloc.x, bloc.y, bloc.h, bloc.w, nivell);
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
    //cout << "avere com faig aixo sense morirme" << endl;
    if(a->kind == "AND"){
        return bool(ievaluaCond(child(a,0))) && bool(ievaluaCond(child(a,1)));
    }
    else if (a->kind == "FITS") return fits(a);
    else if (a->kind == "<"){
        return ievaluaCond(child(a,0)) < ievaluaCond(child(a,1));
    }
    else if (a->kind == ">"){
        return ievaluaCond(child(a,0)) > ievaluaCond(child(a,1));
    }
    else{
        cerr << "LA CONDICIO HA DE CONTENIR ALMENYS UN OPERADOR BINARI" << endl;
       // exit(1);
    }
}

bool flat(int x, int y, int h, int w, int ground){
    int firstHeight = ground;
    if(x >= g.n or y>=g.m or x<1 or y<1 or x+w >= g.n or y+h >= g.m) return false;
    if (ground == -1) firstHeight = g.height[x][y];
    for (int i = y; i < y+h; ++i){
        for (int j = x; j < x+w; ++j){
            if(g.height[i][j] != firstHeight) return false;
        }
    }
    return true;
}

void level(int desnivell, int x, int y, int h, int w){
    for (int i = y; i < y+h; ++i){
        for (int j = x; j < x+w; ++j){
            g.height[i][j] += desnivell;
        }
    }
}

int heightByName(string nom){
    map<string,tblock>::iterator it;
    it = g.blocks.find(nom);
    tblock bloc = it->second;
    int maxH = g.height[bloc.x][bloc.y];
    //cout << "maxH de " << nom << " es " << maxH << endl;
    for (int i = bloc.y; i < bloc.y+ bloc.h; ++i){
        for (int j = bloc.x; j < bloc.x+ bloc.w; ++j){
            if (g.height[i][j] > maxH ){
                maxH = g.height[i][j];
                //cout << "nou  maxH = " << maxH << endl;
            }
        }
    }
    return maxH;
}

void placeBlock(string nom, int h, int w, int x, int y){
    if(flat(x,y,h,w,0)){
        tblock bloc;
        bloc.x = x; bloc.y = y; bloc.h = h; bloc.w = w;
        g.blocks.insert(pair<string,tblock>(nom,bloc));
        for (int i = bloc.y; i < bloc.y+h; ++i){
            for (int j = bloc.x; j < bloc.x+w; ++j){
                ++g.height[i][j];
                //cout << g.height[i][j] << " " ;

            }
            //cout << endl;
        }
    }
}

int firstIncLev(tblock bloc, string name){
    int nivell = 0;
    while(nivell < heightByName(name)){
        for(int i = bloc.y; i < bloc.y+bloc.h; ++i){
            for(int j = bloc.x; j < bloc.x+bloc.w; ++j){
                int acth = g.height[i][j];
                if(acth != nivell) return nivell;
            }
        }
        ++nivell;
    }
}

void push(AST* a){
    int h, w;
    bool id = false;
    if (child(a,0)->kind == "list"){
        w = atoi(child(child(a,0),0)->kind.c_str());
        h = atoi(child(child(a,0),1)->kind.c_str());

    }

    else{
        id = true;
        map<string,tblock>::iterator it;
        it = g.blocks.find(child(a,0)->kind);
        tblock bloc = it->second;
        w = bloc.w;
        h = bloc.h;
    }

    string destName = child(a,1)->kind;
    if (destName == "list"){
        cerr << "EL SEGON OPERAND NO POT SER UN BLOC SENSE NOM" << endl;
        return;
    }
    map<string,tblock>::iterator it;
    it = g.blocks.find(destName);
    tblock bloc = it->second;
    int destx = bloc.x; int desty = bloc.y; int desth = bloc.h; int destw = bloc.w;
    bool placed = false;
    int destniv = firstIncLev(bloc, destName);
    while(not placed && destniv <= desth){
        if(Bfits(w,h,destx,desty,desth,destw,destniv)){
            placeBlock("",h,w,fitsX,fitsY);
            placed = true;
        }
    }
}

void pop(AST* a){

}

void asignacio(AST* a){
     //cout<< "el token asig es de " << a->kind << endl;
    string nom = child(a,0)->kind;
    if(child(a,1)->kind == "PLACE"){
        AST *tamany = child( child(a,1) ,0);
        AST *lloc = child( child(a,1) ,1);
        placeBlock(nom, atoi(child(tamany, 0)->kind.c_str()), atoi(child(tamany,1)->kind.c_str()), atoi(child(lloc,0)->kind.c_str()), atoi(child(lloc,1)->kind.c_str()));
    }
    else if(child(a,1)->kind == "PUSH"){
       // //cout << "hola que tal " << endl;
        push(child(a,1));
    }
    else{//POP
        pop (child(a,1));
    }


}

void move(AST* a){
    //cout << "el token move es de " << a->kind << endl;
    string nom = child(a, 0)->kind;
    string dir  = child(a, 1)->kind;
    int distancia = atoi(child(a, 2)->kind.c_str());
    int dx,dy;
    if(dir == "NORTH"){
        dx = 0; dy = -distancia;
    }
    else if (dir == "SOUTH"){
        dx = 0; dy = distancia;
    }
    else if(dir == "EAST"){
        dx = distancia; dy = 0;
    }
    else{
        dx = -distancia; dy = 0;
    }
    map<string,tblock>::iterator it;
    it = g.blocks.find(nom);
    tblock bloc = it->second;
    if (it == g.blocks.end()){
        cerr << "EL BLOC ESPECIFICAT NO EXISTEIX" << endl;
        //exit(1);
    }
    else if(flat(bloc.x+dx, bloc.y+dy, bloc.h, bloc.w,0)){
        level(-heightByName(nom),bloc.x,bloc.y,bloc.h,bloc.w);
        level(heightByName(nom),bloc.x+dx,bloc.y+dy,bloc.h,bloc.w);
        it->second.x = bloc.x+dx;
        it->second.y = bloc.y+dy;
    }
}

void bucle(AST* a){
    //contCond = 0;
    //cout << "el token bakel es de " << a->kind << endl;
    while (evaluaCond(child(a,0))){
        doOperations(child(a,1));
    }
}

int retHeight(AST* a){
    //cout << "volem saber la alçada de " << child(a,0)->kind <</* " i count es "<< count << */endl;
    string nom = child(a, 0)->kind;
    return heightByName(nom);
}

void execFunc(AST* a){
    //cout << "el token execfunc es de " << a->kind << endl;
    map <string, AST*>::iterator it;
    it = functions.find(a->kind);
    if(it != functions.end()) doOperations(functions[a->kind]);
    else{
        cerr << "LA FUNCIO " << a->kind << " NO ESTA DEFINIDA" << endl;
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
#token SPACE "[\ \n\t]" << zzskip();>>

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
