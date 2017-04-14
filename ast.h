//  Declarations for an AST calculator
//  From "flex & bison", fb3-1, by John Levine
//  Adapted by Brian Malloy
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <cmath>
#include <vector>
#include <iomanip>

extern void yyerror(const char*);
extern void yyerror(const char*, const char);
extern int error;


class Ast {
public:
  Ast(int);
  virtual ~Ast() {}
  char getNodetype() const { return nodetype; }
  virtual Ast* getLeft() const { throw std::string("No Left"); }
  virtual Ast* getRight() const { throw std::string("No Right"); }
  virtual char getType() const { throw std::string("No Type");}
  virtual double getNumber() const { throw std::string("No Number"); }
  virtual double eval() const = 0;
  virtual void setType(char) {}
  virtual std::string getFuncName() const { throw std::string("Not a function"); }
  virtual std::string getKey() const{ throw std::string("No key"); }
  virtual std::string getCallName() const { throw std::string("No Call Made"); }
  int getScope() const { return scope; }
  void setScope(int sc) { scope = sc; }  
private:
  char nodetype;
  int scope;
};

void treeFree(Ast*); 

class PlusNode : public Ast {
public:
  PlusNode(char nodetype, Ast* l, Ast* r) :
    Ast(nodetype), left(l), right(r)
  {}
  virtual ~PlusNode() {}
 virtual Ast* getLeft() const  { return left;  }
  virtual Ast* getRight() const { return right; }
  virtual char getType() const { 
    return left->getType() == 'f' || right->getType() == 'f' ? 'f' : 'i';  
  }
  virtual double eval() const {
    return left->eval() + right->eval();
  }
private:
  Ast *left;
  Ast *right;
};

class MinusNode : public Ast {
public:
  MinusNode(char nodetype, Ast* l, Ast* r) :
    Ast(nodetype), left(l), right(r)
  {}
  virtual ~MinusNode() {}
  virtual Ast* getLeft() const  { return left;  }
  virtual Ast* getRight() const { return right; }
  virtual char getType() const { 
    return left->getType() == 'f' || right->getType() == 'f' ? 'f' : 'i';  
  }
  virtual double eval() const {
    return left->eval() - right->eval();
  }
private:
  Ast *left;
  Ast *right;
};

class MultNode : public Ast {
public:
  MultNode(char nodetype, Ast* l, Ast* r) :
    Ast(nodetype), left(l), right(r)
  {}
  virtual ~MultNode() {}
  virtual Ast* getLeft() const  { return left;  }
  virtual Ast* getRight() const { return right; }
  virtual char getType() const { 
    return left->getType() == 'f' || right->getType() == 'f' ? 'f' : 'i'; 
  }
  virtual double eval() const {
    return left->eval() * right->eval();
  }
private:
  Ast *left;
  Ast *right;
};

class DivNode : public Ast {
public:
  DivNode(char nodetype, Ast* l, Ast* r) :
    Ast(nodetype), left(l), right(r)
  {}
  virtual ~DivNode() {}
  virtual Ast* getLeft() const  { return left;  }
  virtual Ast* getRight() const { return right; }
  virtual char getType() const { 
    return left->getType() == 'f' || right->getType() == 'f' ? 'f' : 'i';  
  }
  virtual double eval() const {
    double leftVal = left->eval();
    double rightVal = right->eval();
      if(rightVal == 0){
        if(left->getType() == 'f' || right->getType() == 'f'){
          std::cout << "ZeroDivisionError: float division by zero" << std::endl;
        }
        else{
          std::cout << "ZeroDivisionError: integer division or modulo by zero" << std::endl;
        }
        return 0;    
      }
      if(left->getType() == 'f' || right->getType() == 'f'){
        return leftVal / rightVal;
      }
      else{ 
        return floor((leftVal / rightVal));
      }
   }
private:
  Ast *left;
  Ast *right;
};

class IntDivNode : public Ast {
public:
  IntDivNode(char nodetype, Ast* l, Ast* r) :
    Ast(nodetype), left(l), right(r)
  {}
  virtual ~IntDivNode() {}
  virtual Ast* getLeft() const  { return left;  }
  virtual Ast* getRight() const { return right; }
  virtual char getType() const { 
     return left->getType() == 'f' || right->getType() == 'f' ? 'f' : 'i'; 
  }
  virtual double eval() const {
    double leftVal = left->eval();
    double rightVal = right->eval();
    if(rightVal == 0){
      if(left->getType() == 'f' || right->getType() == 'f'){
        std::cout << "ZeroDivisionError: float divmod()" << std::endl;
      }
      else{
        std::cout << "ZeroDivisionError: integer division or modulo by zero" << std::endl;
      }
      return 0;    
      }
      else{
        return floor((leftVal / rightVal));
      }
  }
private:
  Ast *left;
  Ast *right;
};

class ModNode : public Ast {
public:
  ModNode(char nodetype, Ast* l, Ast* r) :
    Ast(nodetype), left(l), right(r)
  {}
  virtual ~ModNode() {}
  virtual Ast* getLeft() const  { return left;  }
  virtual Ast* getRight() const { return right; }
  virtual char getType() const { 
    return left->getType() == 'f' || right->getType() == 'f' ? 'f' : 'i';  
  }
  virtual double eval() const {
    double leftVal = left->eval();
    double rightVal = right->eval();
    if(rightVal == 0){
      if(left->getType() == 'f' || right->getType() == 'f'){
        std::cout << "ZeroDivisionError: float modulo" << std::endl;
      }
      else{
        std::cout << "ZeroDivisionError: integer division or modulo by zero" << std::endl;
      }
      return 0;    
    }
    else{
      return fmod(fmod(leftVal,rightVal)+rightVal,rightVal);
    }
  } 
private:
  Ast *left;
  Ast *right;
};

class ExpNode : public Ast {
public:
  ExpNode(char nodetype, Ast* l, Ast* r) :
    Ast(nodetype), left(l), right(r)
  {}
  virtual ~ExpNode() {}
  virtual Ast* getLeft() const  { return left;  }
  virtual Ast* getRight() const { return right; }
  virtual char getType() const {
    if(right->eval() < 0) return 'f'; 
    else return left->getType() == 'f' || right->getType() == 'f' ? 'f' : 'i'; 
  }
  virtual double eval() const {
    return pow(left->eval(),right->eval());
  }
private:
  Ast *left;
  Ast *right;
};

class UMinusNode : public Ast {
public:
  UMinusNode(char nodetype, Ast* l, Ast* r) :
    Ast(nodetype), left(l), right(r)
  {}
  virtual ~UMinusNode() {}
  virtual Ast* getLeft() const  { return left;  }
  virtual Ast* getRight() const { return right; }
  virtual char getType() const { 
    return left->getType(); 
  }
  virtual double eval() const {
    return -left->eval();
  }
private:
  Ast *left;
  Ast *right;
};

class UPlusNode : public Ast {
public:
  UPlusNode(char nodetype, Ast* l, Ast* r) :
    Ast(nodetype), left(l), right(r)
  {}
  virtual ~UPlusNode() {}
  virtual Ast* getLeft() const  { return left;  }
  virtual Ast* getRight() const { return right; }
  virtual char getType() const { 
    return left->getType(); 
  }
  virtual double eval() const {
    return left->eval();
  }
private:
  Ast *left;
  Ast *right;
};

class AstNumber : public Ast {
public:
  AstNumber(char nodetype, double n, char type) : Ast(nodetype), number(n), numberType(type) {}
  virtual ~AstNumber() {}
  virtual double getNumber() const { return number; }
  virtual char getType() const { return numberType; }
  virtual double eval() const {
    return number;
  }
  
private:
  double number;
  char numberType;
};

class IdNode : public Ast {
public:
  IdNode(char nodetype, char typeInfo, std::string str) :
    Ast(nodetype), key(str), type(typeInfo)
  {}
  virtual char getType() const;
  virtual double eval() const;
  virtual void setType(char t){
    type = t;
  }
  virtual ~IdNode() {}
  virtual std::string getKey() const { return key; } 
private:
  std::string key;
  char type;  
};

class PrintNode : public Ast {
public:
  PrintNode(char nodetype, Ast* val) : Ast(nodetype) , value(val) { }
  virtual ~PrintNode() {}
  virtual double eval() const { 
    value->setScope(this->getScope());
    if(value->getType() == 'f' && fmod(value->eval(),1) == 0){
      std::cout << std::setprecision(12) << value->eval() << ".0" << std::endl;
    }
    else{ 
      std::cout << std::setprecision(12) << value->eval() << std::endl;
    }
    if(this->getScope()==0) ::treeFree(value);
    return 0; 
  }
  virtual char getType() const { return value -> getType(); }
private:
  Ast* value;
};


class AssNode : public Ast {
public:
  AssNode(char nodetype, Ast* left, Ast* right) : Ast(nodetype) , lhs(left), rhs(right) { }
  virtual ~AssNode() {}
  virtual double eval() const;
  virtual Ast* getLeft() const { return lhs; }
  virtual Ast* getRight() const { return rhs; }
private:
 Ast* lhs;
 Ast* rhs;
};


class FunctionNode : public Ast {
public:
  FunctionNode(char nodetype, std::string name, std::vector<Ast*>* f) : 
    Ast(nodetype), funcName(name), funcVec(f) { }
  virtual ~FunctionNode() {  
    for (std::vector<Ast*>::iterator it = funcVec->begin() ; it != funcVec->end(); ++it)
     {
       treeFree(*it);
     } 
     funcVec->clear();  
  }
  virtual double eval() const;
  virtual std::string getFuncName() const { return funcName; } 
private:
  std::string funcName;
  std::vector<Ast*>* funcVec;
};

class CallNode : public Ast {
public:
  CallNode(char nodetype, std::string name) : Ast(nodetype), key(name) {}
  virtual ~CallNode() {}
  virtual double eval() const;
  virtual std::string getCallName() const { return key; }
private:
  std::string key; 
};

class ErrorNode : public Ast {
public:
  ErrorNode(int error, std::string val) : Ast('E'), errorCode(error), key(val) {}
  virtual double eval() const {
    if(errorCode == 0) { std::cout << "NameError: name '" << key << "' is not defined" << std::endl;  }
    return 0;
  }
private:
  int errorCode;
  std::string key;
};
