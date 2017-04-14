#include <iostream>
#include <map>
#include <string>

class Ast;

class SymTable {
public:
  bool check(std::string);
  void insert(std::string, Ast*);
  Ast* getValue(std::string) const;
  char getType(std::string) const;
  ~SymTable();
  SymTable(const SymTable&);
  SymTable& operator=(const SymTable&);
  SymTable() {}
private:
   std::map<std::string,Ast*> mymap;
};
