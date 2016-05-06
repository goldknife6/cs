# c-short
 1. *program* → *declist* | **ε**
 2. *declist* → *declist* *dec* | *dec*
 3. *dec* → *vardec* | *funcdec*
 4. *vardec* → **var** *type-id* *vardecid* [= *simpleexpr*] **;**
 7. *vardecid* → **ID** | **ID** **[** NUMCONST **]**
 9. *funcdec* → **def**  **ID** **(** *paramlist*| **ε** **)** *type-id* **{** *locvardecList* *stmtlist* **}**
 11. *paramlist* → *paramlist* **,** *paramt* | *paramt*
 12. *paramt* → *type-id* **ID** 
 14. *stmt* → *exprstmt* |  *compoundstmt*  | *ifstmt* | *whilestmt* | *forstmt* | *returnstmt* | *breakstmt*
 15. *compoundstmt*→ **{** *locvardecList* *stmtlist* **}**
 16. *locvardecList → locvardecList locvardec* | *locvardec* 
 17. *locvardec* → **var** *type-id* *vardecid* [= *simpleexpr*] **;**
 20. *stmtlist → stmtlist stmt* | **ε** 
 21. *exprstmt* → *exprlist* **;** | **;** 
 22. *ifstmt* → **if** **(** *exprlist* **)** *stmt* **[** **else** *stmt* **]**
 23. *whilestmt* → **while** **(** *exprlist* **)** *stmt*
 24. *forstmt* → **for** **(** *exprlist* **;** *exprlist* **;** *exprlist* **)** *stmt*
 24. *returnstmt* → **return** **;** | **return** *exprlist* **;**
 25. *breakstmt* → **break** **;**
 25. *exprlist* → *exprlist* **,** *expr* | *expr*
 26. *expr* → *assignexpr* | *simpleexpr*
 26. *assignexpr* → **$** *mutable* **=** *expr* 
 27. *simpleexpr → simpleexpr* **|** *andexpr* | *andexpr*
 28. *andexpr → andexpr* **&** *unaryRelExpr* | *unaryRelExpr*
 29. *unaryRelExpr →* **!** *relExpr* | *relExpr*
 30. *relExpr → sumExpr relop sumExpr* | *sumExpr*
 31. *relop* → **<=** | **<** | **>** | **>=** | **==** | **!=**
 32. *sumExpr → sumExpr sumop term* | *term*
 33. *sumop* → **+** | **−**
 34. *term → term mulop unaryexpr* | *unaryexpr*
 35. *mulop* → **'*'** | **/** | **%**
 36. *unaryexpr* → * **−** unaryexpr* | *factor*
 38. *factor* → *immutable* | *mutables*
 39. *mutable* → **ID**
 40. *immutable* → **(** *expr* | **ε** **)** | *call* | NUMCONST | CHARCONST | STRINGCONST | **true** | **false**
 41. *call* → **[** *arglist* **]** **ID** 
 43. *arglist → arglist*  **','**  *expr* | *expr*
