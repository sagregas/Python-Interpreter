
# include "ast.h"
# include "symTable.h"  
#include "tableManager.h"


Ast::Ast(int n) : nodetype(n), scope(TableManager::getInstance().getCurrScope()) { }

double IdNode::eval() const {
  TableManager& tableManager = TableManager::getInstance();
  return (tableManager.getTableValue(key,this->getScope()))->eval();  
}

char IdNode::getType() const{
  TableManager& tableManager = TableManager::getInstance();
  if(!tableManager.check(key, this->getScope()))
   return '\0';
  return tableManager.getType(key);
}


double AssNode::eval() const {
  if(lhs->getNodetype() == 'I'){
    double d = rhs -> eval();
    lhs->setType(rhs -> getType());
    TableManager& tableManager = TableManager::getInstance();
    tableManager.insertSymbol(lhs->getKey(),lhs->getScope(), new AstNumber('K',d, rhs->getType()));
  }
  return 0;  
}


double FunctionNode::eval() const {
  std::vector<Ast*>::reverse_iterator it = funcVec->rbegin();
  while(it != funcVec->rend()){
    if((*it)->getNodetype() == 'J'){
      (*it)->setScope(this->getScope()+1);
    }
    (*it) -> eval();
     ++it;
  }
  TableManager::getInstance().destroySymbolTable(this->getScope()+1);
return 0;
}

double CallNode::eval() const {
  return (TableManager::getInstance().getTableValue(key, this->getScope()))->eval();
}


void treeFree(Ast *a) {
  switch(a->getNodetype()) {
   // two subtrees
  case '+':
  case '-':
  case '*':
  case '/':
  case '^':
  case '!':
  case '%':
  case 'V':
    treeFree(a->getRight());

   // one subtrees
  case 'M':
  case 'P':
    treeFree(a->getLeft());

   //no subtree
  case 'K':
  case 'I':
  case 'J':
  case '1':
  case '2':
  case 'E':
    delete a;
    break;

  default: std::cout << "internal error: bad node "
                << a->getNodetype() << std::endl;;
  }
}
