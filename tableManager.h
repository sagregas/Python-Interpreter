#include <iostream>
#include <vector>
#include <string>

class SymTable;
class Ast;

class TableManager {
public:
  static TableManager& getInstance();
  void insertSymbol(std::string,int, Ast*);
  Ast* getTableValue(std::string, int) ;
  int getCurrScope() const;
  void incrScope();
  void decrScope();
  bool check(std::string, int);
  char getType(std::string) const ;
  void destroySymbolTable(int);
  std::vector<Ast*> getGlobalVector() const;
  int getSize() const;
private:
  TableManager();
  ~TableManager();
  TableManager(const TableManager&);
  TableManager& operator=(const TableManager&);
  std::vector<SymTable*> managerVec;
  int currScope;
};
