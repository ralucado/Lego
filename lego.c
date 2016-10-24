/*
 * A n t l r  T r a n s l a t i o n  H e a d e r
 *
 * Terence Parr, Will Cohen, and Hank Dietz: 1989-2001
 * Purdue University Electrical Engineering
 * With AHPCRC, University of Minnesota
 * ANTLR Version 1.33MR33
 *
 *   antlr -gt lego.g
 *
 */

#define ANTLR_VERSION	13333
#include "pcctscfg.h"
#include "pccts_stdio.h"

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
#define GENAST

#include "ast.h"

#define zzSET_SIZE 4
#include "antlr.h"
#include "tokens.h"
#include "dlgdef.h"
#include "mode.h"

/* MR23 In order to remove calls to PURIFY use the antlr -nopurify option */

#ifndef PCCTS_PURIFY
#define PCCTS_PURIFY(r,s) memset((char *) &(r),'\0',(s));
#endif

#include "ast.c"
zzASTgvars

ANTLR_INFO

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
  map <string, vector<tblock> > apilats;
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
  for (it = functions.begin(); it != functions.end(); ++it){
    cout << it->first << "->" << it->second->kind << endl;
  }
  
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
  bool found = false;
  int i = bloc.y, j=bloc.x;
  while (not found && i < bloc.y+bloc.h){
    found = flat(i,j,tamY,tamX,nivell);
    if(found) return true;
    ++j;
    if(j % (bloc.x+bloc.w) == 0){
      j = bloc.x;
      ++i;
    }
  }
  return false;
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
  //cout << "guarra" << endl;
  if(flat(x,y,h,w,0)){
    tblock bloc;
    bloc.x = x; bloc.y = y; bloc.h = h; bloc.w = w;
    g.blocks.insert(pair<string,tblock>(nom,bloc));
    //cout << "aqui ve el sida " << endl;
    for (int i = bloc.y; i < bloc.y+h; ++i){
      for (int j = bloc.x; j < bloc.x+w; ++j){
        ++g.height[i][j];
        //cout << g.height[i][j] << " " ;
        
            }
      //cout << endl;
    }
  }
}

void asignacio(AST* a){
  cout << "el token asig es de " << a->kind << endl;
  string nom = child(a,0)->kind;
  if(child(a,1)->kind == "PLACE"){
    AST *tamany = child( child(a,1) ,0);
    AST *lloc = child( child(a,1) ,1);
    placeBlock(nom, atoi(child(tamany, 0)->kind.c_str()), atoi(child(tamany,1)->kind.c_str()), atoi(child(lloc,0)->kind.c_str()), atoi(child(lloc,1)->kind.c_str()));
  }
  else if(child(a,1)->kind == "PUSH"){
    // cout << "hola que tal " << endl;
    
    }
  else{//POP
    
    }
  

}

void move(AST* a){
  cout << "el token move es de " << a->kind << endl;
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
  cout << "el token bakel es de " << a->kind << endl;
  while (evaluaCond(child(a,0))){
    doOperations(child(a,1));
  }
}

int retHeight(AST* a){
  cout << "volem saber la alçada de " << child(a,0)->kind <</* " i count es "<< count << */endl;
  string nom = child(a, 0)->kind;
  return heightByName(nom);
}

void execFunc(AST* a){
  cout << "el token execfunc es de " << a->kind << endl;
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
    else if (actual->kind == "HEIGHT") cout << "ret height es " << retHeight(actual) << endl;
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

void
#ifdef __USE_PROTOS
lego(AST**_root)
#else
lego(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  grid(zzSTR); zzlink(_root, &_sibling, &_tail);
  ops(zzSTR); zzlink(_root, &_sibling, &_tail);
  defs(zzSTR); zzlink(_root, &_sibling, &_tail);
  (*_root)=createASTlist(_sibling);
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd1, 0x1);
  }
}

void
#ifdef __USE_PROTOS
ops(AST**_root)
#else
ops(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    for (;;) {
      if ( !((setwd1[LA(1)]&0x2))) break;
      if ( (LA(1)==ID) ) {
        fact(zzSTR); zzlink(_root, &_sibling, &_tail);
      }
      else {
        if ( (LA(1)==MOVE) ) {
          moviment(zzSTR); zzlink(_root, &_sibling, &_tail);
        }
        else {
          if ( (LA(1)==WHILE) ) {
            bucle(zzSTR); zzlink(_root, &_sibling, &_tail);
          }
          else {
            if ( (LA(1)==HEIGHT) ) {
              height(zzSTR); zzlink(_root, &_sibling, &_tail);
            }
            else break; /* MR6 code for exiting loop "for sure" */
          }
        }
      }
      zzLOOP(zztasp2);
    }
    zzEXIT(zztasp2);
    }
  }
  (*_root)=createASTlist(_sibling);
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd1, 0x4);
  }
}

void
#ifdef __USE_PROTOS
bucle(AST**_root)
#else
bucle(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  zzmatch(WHILE); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(LP);  zzCONSUME;
  cond(zzSTR); zzlink(_root, &_sibling, &_tail);
  zzmatch(RP);  zzCONSUME;
  zzmatch(LB);  zzCONSUME;
  ops(zzSTR); zzlink(_root, &_sibling, &_tail);
  zzmatch(RB);  zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd1, 0x8);
  }
}

void
#ifdef __USE_PROTOS
grid(AST**_root)
#else
grid(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  zzmatch(GRID); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(NUM); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(NUM); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd1, 0x10);
  }
}

void
#ifdef __USE_PROTOS
fact(AST**_root)
#else
fact(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  zzmatch(ID); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    if ( (LA(1)==ASIG) ) {
      {
        zzBLOCK(zztasp3);
        zzMake0;
        {
        zzmatch(ASIG); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
        {
          zzBLOCK(zztasp4);
          zzMake0;
          {
          if ( (LA(1)==PLACE) ) {
            place(zzSTR); zzlink(_root, &_sibling, &_tail);
          }
          else {
            if ( (setwd1[LA(1)]&0x20) ) {
              pp(zzSTR); zzlink(_root, &_sibling, &_tail);
            }
            else {zzFAIL(1,zzerr1,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
          }
          zzEXIT(zztasp4);
          }
        }
        zzEXIT(zztasp3);
        }
      }
    }
    else {
      if ( (setwd1[LA(1)]&0x40) ) {
      }
      else {zzFAIL(1,zzerr2,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
    }
    zzEXIT(zztasp2);
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd1, 0x80);
  }
}

void
#ifdef __USE_PROTOS
place(AST**_root)
#else
place(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  zzmatch(PLACE); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
  comp(zzSTR); zzlink(_root, &_sibling, &_tail);
  zzmatch(AT);  zzCONSUME;
  comp(zzSTR); zzlink(_root, &_sibling, &_tail);
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd2, 0x1);
  }
}

void
#ifdef __USE_PROTOS
pp(AST**_root)
#else
pp(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    if ( (setwd2[LA(1)]&0x2) ) {
      comp(zzSTR); zzlink(_root, &_sibling, &_tail);
    }
    else {
      if ( (LA(1)==ID) ) {
        zzmatch(ID); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
      }
      else {zzFAIL(1,zzerr3,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
    }
    zzEXIT(zztasp2);
    }
  }
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    if ( (setwd2[LA(1)]&0x4) ) {
      {
        zzBLOCK(zztasp3);
        zzMake0;
        {
        if ( (LA(1)==PUSH) ) {
          zzmatch(PUSH); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
        }
        else {
          if ( (LA(1)==POP) ) {
            zzmatch(POP); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
          }
          else {zzFAIL(1,zzerr4,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
        }
        zzEXIT(zztasp3);
        }
      }
      pp(zzSTR); zzlink(_root, &_sibling, &_tail);
    }
    else {
      if ( (setwd2[LA(1)]&0x8) ) {
      }
      else {zzFAIL(1,zzerr5,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
    }
    zzEXIT(zztasp2);
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd2, 0x10);
  }
}

void
#ifdef __USE_PROTOS
moviment(AST**_root)
#else
moviment(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  zzmatch(MOVE); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(ID); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  dir(zzSTR); zzlink(_root, &_sibling, &_tail);
  zzmatch(NUM); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd2, 0x20);
  }
}

void
#ifdef __USE_PROTOS
cond(AST**_root)
#else
cond(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    if ( (LA(1)==FITS) ) {
      fits(zzSTR); zzlink(_root, &_sibling, &_tail);
    }
    else {
      if ( (setwd2[LA(1)]&0x40) ) {
        comparacio(zzSTR); zzlink(_root, &_sibling, &_tail);
      }
      else {zzFAIL(1,zzerr6,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
    }
    zzEXIT(zztasp2);
    }
  }
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    if ( (LA(1)==AND) ) {
      zzmatch(AND); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
      cond(zzSTR); zzlink(_root, &_sibling, &_tail);
    }
    else {
      if ( (LA(1)==RP) ) {
      }
      else {zzFAIL(1,zzerr7,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
    }
    zzEXIT(zztasp2);
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd2, 0x80);
  }
}

void
#ifdef __USE_PROTOS
comparacio(AST**_root)
#else
comparacio(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  atom(zzSTR); zzlink(_root, &_sibling, &_tail);
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    if ( (LA(1)==LT) ) {
      zzmatch(LT); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
    }
    else {
      if ( (LA(1)==GT) ) {
        zzmatch(GT); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
      }
      else {zzFAIL(1,zzerr8,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
    }
    zzEXIT(zztasp2);
    }
  }
  atom(zzSTR); zzlink(_root, &_sibling, &_tail);
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd3, 0x1);
  }
}

void
#ifdef __USE_PROTOS
atom(AST**_root)
#else
atom(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==HEIGHT) ) {
    height(zzSTR); zzlink(_root, &_sibling, &_tail);
  }
  else {
    if ( (LA(1)==NUM) ) {
      zzmatch(NUM); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
    }
    else {zzFAIL(1,zzerr9,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd3, 0x2);
  }
}

void
#ifdef __USE_PROTOS
comp(AST**_root)
#else
comp(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    if ( (LA(1)==LP) ) {
      zzmatch(LP);  zzCONSUME;
    }
    else {
      if ( (LA(1)==NUM) ) {
      }
      else {zzFAIL(1,zzerr10,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
    }
    zzEXIT(zztasp2);
    }
  }
  zzmatch(NUM); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(COMA);  zzCONSUME;
  zzmatch(NUM); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    if ( (LA(1)==RP) ) {
      zzmatch(RP);  zzCONSUME;
    }
    else {
      if ( (setwd3[LA(1)]&0x4) ) {
      }
      else {zzFAIL(1,zzerr11,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
    }
    zzEXIT(zztasp2);
    }
  }
  (*_root)=createASTlist(_sibling);
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd3, 0x8);
  }
}

void
#ifdef __USE_PROTOS
height(AST**_root)
#else
height(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  zzmatch(HEIGHT); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(LP);  zzCONSUME;
  zzmatch(ID); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(RP);  zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd3, 0x10);
  }
}

void
#ifdef __USE_PROTOS
fits(AST**_root)
#else
fits(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  zzmatch(FITS); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(LP);  zzCONSUME;
  zzmatch(ID); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(COMA);  zzCONSUME;
  comp(zzSTR); zzlink(_root, &_sibling, &_tail);
  zzmatch(COMA);  zzCONSUME;
  zzmatch(NUM); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(RP);  zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd3, 0x20);
  }
}

void
#ifdef __USE_PROTOS
defs(AST**_root)
#else
defs(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    while ( (LA(1)==DEF) ) {
      def(zzSTR); zzlink(_root, &_sibling, &_tail);
      zzLOOP(zztasp2);
    }
    zzEXIT(zztasp2);
    }
  }
  (*_root)=createASTlist(_sibling);
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd3, 0x40);
  }
}

void
#ifdef __USE_PROTOS
def(AST**_root)
#else
def(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  zzmatch(DEF); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(ID); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  ops(zzSTR); zzlink(_root, &_sibling, &_tail);
  zzmatch(END);  zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd3, 0x80);
  }
}

void
#ifdef __USE_PROTOS
dir(AST**_root)
#else
dir(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==NORTH) ) {
    zzmatch(NORTH); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  }
  else {
    if ( (LA(1)==EAST) ) {
      zzmatch(EAST); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
    }
    else {
      if ( (LA(1)==WEST) ) {
        zzmatch(WEST); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
      }
      else {
        if ( (LA(1)==SOUTH) ) {
          zzmatch(SOUTH); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
        }
        else {zzFAIL(1,zzerr12,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
      }
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd4, 0x1);
  }
}

void
#ifdef __USE_PROTOS
reop(AST**_root)
#else
reop(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==LT) ) {
    zzmatch(LT); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
  }
  else {
    if ( (LA(1)==GT) ) {
      zzmatch(GT); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
    }
    else {zzFAIL(1,zzerr13,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd4, 0x2);
  }
}
