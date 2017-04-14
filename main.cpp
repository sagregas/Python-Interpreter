#include <iostream>
#include "ast.h"
#include "parse.tab.hpp"
#include "symTable.h"
#include "tableManager.h"

extern FILE * yyin;
extern int keywords;
extern int identifiers;
extern int numbers;
extern int yydebug;
extern int yylineno;

int error = 0;

int yylex(void);
extern int yyparse();


void openFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
      std::cerr << "Could not open file \"" 
                << filename 
                << "\"" << std::endl;
      exit(1);
    }
    yyin = file;
}

int main(int argc, char * argv[]) {
  if (argc > 1) { // user-supplied filename
    openFile(argv[1]);
  }
  yydebug = 0;
  int parseErrors = yyparse();
  if ( parseErrors ) {
    std::cerr << "Abnormal termination" << std::endl;
    return 1;
  }
  return 0;
}
