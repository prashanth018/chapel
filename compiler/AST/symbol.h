#ifndef _SYMBOL_H_
#define _SYMBOL_H_

#include "link.h"
#include "type.h"

class Stmt;

enum varType {
  VAR_NORMAL,
  VAR_CONFIG,
  VAR_STATE
};

class Symbol : public ILink {
 public:
  char* name;
  Type* type;
  int level;

  Symbol(char* init_name, Type* init_type = dtUnknown);

  void print(FILE* outfile);
  virtual void printDef(FILE* outfile);
  void printDefList(FILE* outfile, char* separator);
  virtual void codegen(FILE* outfile);
  virtual void codegenDef(FILE* outfile);
  void codegenDefList(FILE* outfile, char* separator);
};


class NullSymbol : public Symbol {
 public:
  NullSymbol(void);

  bool isNull(void);
};


class UseBeforeDefSymbol : public Symbol {
 public:
  UseBeforeDefSymbol(char* init_name);
};


class VarSymbol : public Symbol {
 public:
  varType varClass;
  bool isConst;

  VarSymbol(char* init_name, varType init_varClass = VAR_NORMAL, 
	    bool init_isConst = false, Type* init_type = dtUnknown);
  
  void printWithType(FILE* outfile);
};


class NullVarSymbol : public VarSymbol {
 public:
  NullVarSymbol(void);

  bool isNull(void);
};


enum paramType {
  PARAM_IN = 0,
  PARAM_INOUT,
  PARAM_OUT,
  PARAM_CONST,

  NUM_PARAM_TYPES
};


class ParamSymbol : public Symbol {
 public:
  paramType usage;

  ParamSymbol(paramType init_usage, char* init_name, 
	      Type* init_type = dtUnknown);

  void printDef(FILE* outfile);
};


class TypeSymbol : public Symbol {
 public:
  TypeSymbol(char* init_name, Type* init_definition);
};


class ClassSymbol : public TypeSymbol {
 public:
  ClassSymbol(char* init_name, ClassType* init_class);

  ClassType* getType(void);
};


class NullClassSymbol : public ClassSymbol {
 public:
  NullClassSymbol(void);
  
  bool isNull(void);
};


class ReduceSymbol : public ClassSymbol {
 public:
  ReduceSymbol(char* init_name, ClassType* init_class);
};


class FnSymbol : public Symbol {
 public:
  bool exportMe;
  Symbol* formals;
  Stmt* body;

  FnSymbol(char* init_name, Symbol* init_formals, Type* init_retType,
	    Stmt* init_body, bool init_exportMe=false);

  void codegenDef(FILE* outfile);
};


class EnumSymbol : public Symbol {
 public:
  int val;

  EnumSymbol(char* init_name, int init_val);
};

#endif
