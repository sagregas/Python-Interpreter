#include "symTable.h"
#include "ast.h"

SymTable::~SymTable(){
  std::map<std::string, Ast*>::iterator it = mymap.begin();
  while (it != mymap.end()){
    treeFree(it->second);
    ++it;
  }
}

bool SymTable::check(std::string val){
  std::map<std::string,Ast*>::const_iterator it;
  it = mymap.find(val);
  if(it != mymap.end()) return true;
  return false;
}

void SymTable::insert(std::string key,Ast* node){  
  mymap[key] = node;
}

Ast* SymTable::getValue(std::string key) const {
  std::map<std::string,Ast*>::const_iterator it;
  it = mymap.find(key);
  return (it -> second);
}

char SymTable::getType(std::string key) const {
  std::map<std::string,Ast*>::const_iterator it;
  it = mymap.find(key);
  return (it -> second)->getType();
}
