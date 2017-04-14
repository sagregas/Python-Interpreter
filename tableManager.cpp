#include "tableManager.h"
#include "symTable.h"
#include "ast.h"

TableManager& TableManager::getInstance() {
  static TableManager instance;
  return instance;
}

TableManager::~TableManager() {
  std::vector<SymTable*>::iterator it = managerVec.begin();
  while(it != managerVec.end()){
    delete *it;
    managerVec.erase(it);
  }
}



TableManager::TableManager() : currScope(0) {
  managerVec.push_back(new SymTable());
}

int TableManager::getCurrScope() const {
  return currScope;
}


Ast* TableManager::getTableValue(std::string val, int scope){
  for(int i = scope; i>=0; --i){
    if(managerVec[i]->check(val)){
      return managerVec[i]->getValue(val);
    }
  }
 return new ErrorNode(0,val);
}


bool TableManager::check(std::string key, int scope) {
  for( int i = scope; i>=0; --i){
    if(managerVec[i]->check(key)){
      return true;
    }
  }
  return false;
}

char TableManager::getType(std::string key) const {
  for (int i = currScope; i>=0; --i){
    if(managerVec[i]->check(key)){
      return managerVec[i]->getType(key);
    }
  }
  return '\0';
}

void TableManager::insertSymbol(std::string key,int scope, Ast* node) {
  (managerVec[scope])->insert(key,node);
} 

void TableManager::incrScope(){
 ++currScope;
 managerVec.push_back(new SymTable());
} 

void TableManager::decrScope(){
  --currScope;
}

void TableManager::destroySymbolTable(int i){
  delete managerVec[i];
  managerVec[i] = new SymTable();
}
