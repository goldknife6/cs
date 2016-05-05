# c-short
 1. *program* → *declarationList* | **ε**
 2. *declarationList* → *declarationList* *declaration* | *declaration*
 3. *declaration* → *varDeclaration* | *funcDeclaration*
 4. *varDeclaration* → **var** *type-id* *varDeclId* [= *simpleExpression*] **;**
 7. *varDeclId* → **ID** | **ID** **[** NUMCONST **]**
 9. *funcDeclaration* → **def**  **ID** **(** *params* **)** *type-id* **{** *locVarDecList* *statementList* **}**
 10. *params* → *paramList*  | **ε**
 11. *paramList* → *paramList* **,** *paramTypeList* | *paramTypeList*
 12. *paramTypeList* → *type-id* *paramId*
 13. *paramId* → **ID** | **ID** **[ ]**
 14. *statement* → *expressionStmt* |  *compoundStmt*  | *ifStmt* | *whileStmt* | *forStmt* | *returnStmt* | *breakStmt*
 15. *compoundStmt*→ **{** *locVarDecList* *statementList* **}**
 16. *locVarDecList → locVarDecList LocVarDec* | *LocVarDec* 
 17. *LocVarDec* → **var** *type-id* *varDeclId* [= *simpleExpression*] **;**
 20. *statementList → statementList statement* | **ε** 
 21. *expressionStmt* → *exprList* **;** | **;** 
 22. *ifStmt* → **if** **(** *exprList* **)** *statement* **[** **else** *statement* **]**
 23. *whileStmt* → **while** **(** *exprList* **)** *statement*
 24. *forStmt* → **for** **(** *exprList* **;** *exprList* **;** *exprList* **)** *statement*
 24. *returnStmt* → **return** **;** | **return** *expressionList* **;**
 25. *breakStmt* → **break** **;**
 25. *exprList* → *exprList* **,** *expr* | *expr*
 26. *expr* → *assignExpr* | *simpleExpr*
 26. *assignExpr* → **$** *mutable* **=** *expr* 
 27. *simpleExpr → simpleExpr* **|** *andExpr* | *andExpr*
 28. *andExpr → andExpr* **&** *unaryRelExpr* | *unaryRelExpr*
 29. *unaryRelExpr →* **!** *relExpr* | *relExpr*
 30. *relExpr → sumExpr relop sumExpr* | *sumExpr*
 31. *relop* → **<=** | **<** | **>** | **>=** | **==** | **! =**
 32. *sumExpr → sumExpr sumop term* | *term*
 33. *sumop* → **+** | **−**
 34. *term → term mulop unaryExpr* | *unaryExpr*
 35. *mulop* → **'*'** | **/** | **%**
 36. *unaryExpr* → *unaryop unaryExpr* | *factor*
 37. *unaryop* → **−** 
 38. *factor* → *immutable* | *mutables*
 39. *mutable* → **ID** | **ID** **[** *expr* **]**
 40. *immutable* → **(** *expression* **)** | *call* | NUMCONST | CHARCONST | STRINGCONST | **true** | **false**
 41. *call* → **[** *args* **]** **ID** 
 42. *args → argList* | **ε** 
 43. *argList → argList*  **','**  *expr* | *expr*
