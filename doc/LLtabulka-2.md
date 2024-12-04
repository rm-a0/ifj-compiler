||
| - |
|.|
|e|
|E|
|+|
|-|
|?|
|void|
|u8|
|i32|
|f64|
|[]u8|
|const|
|ifj|
|=|
|"@import"|
|pub|
|fn|
|(|
|)|
|,|
|:|
|var|
|;|
|{|
|}|
|write|
|readstr|
|readi32|
|readf64|
|i2f|
|f2i|
|string|
|lenght|
|concat|
|substring|
|strcmp|
|ord|
|chr|
|ifj|
|||
|else|
|while|
|return|
|==|
|!=|
|<=|
|>=|
|>|
|<|
|idetntifier|
||
|<program>|



|<top\_level\_decl\_list>|
| - |
|<top\_level\_decl>|
|<data\_type>|
|<type>|
|<type\_prefix>|
|<fn\_decl>|
|<param\_list>|
|<param\_list1>|
|<param>|
|<const\_decl>|
|<var\_decl>|
|<type\_assign>|
|<expression>|
|<expression1>|
|<term>|
|<term1>|
|<factor>|
|<factor\_suffix>|
|<number>|
|<block>|
|<statement\_list>|
|<statement\_list1>|
|<statement>|
|<assignment>|
|<assignment1>|
|<arg\_list>|
|<argument>|
|<argument1>|
|<built\_in\_fn>|
|<build\_in\_string>|
|<if\_statement>|
|<else\_statement>|
|<while\_statement>|
|<bin\_expression>|
|<element\_bind>|
|<return\_statement>|
|<return\_statement1>|
|<binary\_operator>|



|First()|
| - |
|.|
|e|
|E|
|+|
|-|
|?|
|void|
|u8|
|i32|
|f64|
|[]u8|
|const|
|ifj|
|=|
|"@import"|
|pub|
|fn|
|(|
|)|
|,|
|:|
|var|
|;|
|{|
|}|
|write|
|readstr|
|readi32|
|readf64|
|i2f|
|f2i|
|string|
|lenght|
|concat|
|substring|
|strcmp|
|ord|
|chr|
|ifj|
|||
|else|
|while|
|return|
|==|
|!=|
|<=|
|>=|
|>|
|<|
|identifier|
||
|**"pub", "const", "var", ε**|



|**"pub", "const", "var", ε**|
| - |
|**"pub", "const", "var"**|
|**"?", "void", "u8", "i32", "f64", "[]u8"**|
|**"void", "u8", "i32", "f64", "[]u8"**|
|**"?", ε**|
|"pub"|
|**identifier, ε**|
|**",", ε**|
|identifier|
|"const"|
|"var"|
|":", ε|
|**integer, float, identifier, string\_literal, "("**|
|"+", "-", ε|
|**integer, float, identifier, string\_literal, "("**|
|"\*", "/", ε|
|integer, float, identifier, string\_literal, "("|
|**"(", ε**|
|**integer, float**|
|**"{"**|
|**"const", "var", identifier, "if", "while", "return"**|
|**"const", "var", identifier, "if", "while", "return", ε**|
|**"const", "var", identifier, "if", "while", "return"**|
|**identifier**|
|**integer, float, identifier, string\_literal, "(", "ifj"**|
|**integer, float, identifier, string\_literal, "(", ε**|
|**integer, float, identifier, string\_literal, "("**|
|**",", ε**|
|**"ifj"**|
|write, "readstr", "readi32", "readf64", "i2f", "f2i", "string", "length", "concat", "substring", "strcmp", "ord", "chr"|
|**"if"**|
|**"else" | ε**|
|**"while"**|
|**"==", "!=", "<=", ">=", ">", "<", ε**|
|**"|", ε**|
|**"return"**|
|**integer, float, identifier, string\_literal, "(", ε**|
|**"==", "!=", "<=", ">=", ">", "<"**|



|Follow()|Predict()|
| - | - |
|$||
|$||
|$||
|$||
|$||
|$||
|$||
|$||
|$||
|$||
|$||
|$||
|$||
|$||
|$||
|$||
|$||
|$||
|$||
|$||
|$||
|$||
|$||
|$||
|$||
|$||
|$||
|$||
|$||
|$||
|$||
|$||
|$||
|$||
|$||
|$||
|$||
|$||
|$||
|$||
|$||
|$||
|$||
|$||
|$||
|$||
|$||
|$||
|$||
|$||
|||
|$||


|$||
| - | - |
|"pub", "const", "var",$||
|**"{", ",", ")", "="**||
|"{", ",", ")", "="||
|"void", "u8", "i32", "f64", "[]u8"||
|"pub", "const", "var",$||
|")"||
|")"||
|",", ")"||
|"pub", "const", "var", $, identifier, "if", "while", "return", "}"||
|"pub", "const", "var", $, identifier, "if", "while", "return", "}"||
|"="||
|**";", "==", "!=", "<=", ">=", ">", "<", ")"**||
|**";", "==", "!=", "<=", ">=", ">", "<", ")"**||
|**"+", "-", ";", "==", "!=", "<=", ">=", ">", "<", ")"**||
|**"+", "-", ";", "==", "!=", "<=", ">=", ">", "<", ")"**||
|**"\*", "/", "+", "-", ";", "==", "!=", "<=", ">=", ">", "<", ")", ","**||
|**"\*", "/", "+", "-", ";", "==", "!=", "<=", ">=", ">", "<", ")", ","**||
|**"\*", "/", "+", "-", ";", "==", "!=", "<=", ">=", ">", "<", ")", ","**||
|"else", "pub", "const", "var", $, identifier, "if", "while", "return", "}"||
|"}"||
|**"}"**||
|**"const", "var", identifier, "if", "while", "return", "}"**||
|**"const", "var", identifier, "if", "while", "return", "}"**||
|**";"**||
|**")"**||
|` `**")"**||
|")"||
|**";"**||
|"("||
|**"const", "var", identifier, "if", "while", "return", "}"**||
|**"const", "var", identifier, "if", "while", "return", "}"**||
|**"const", "var", identifier, "if", "while", "return", "}"**||
|**")"**||
|**"{"**||
|**"const", "var", identifier, "if", "while", "return", "}"**||
|";"||
|**integer, float, identifier, string\_literal, "("**||

||$|pub|fn|void|u8|i32|f64|
| - | - | - | - | - | - | - | - |
|<program>|1|1||||||
|<top\_level\_decl\_list>|3|2||||||
|<top\_level\_decl>||4||||||
|<data-type>||||7|7|7|7|
|||||||||
|<type\_prefix>||||9|9|9|9|
|<fn\_decl>||10||||||
|<param\_list>||||||||
|<param\_list1>||||||||
|<param>||||||||
|<const\_decl>||||||||
|<var\_decl>||||||||
|<type\_assign>||||||||
|<expression>||||||||
|<expression1>||||||||
|<term>||||||||
|<term1>||||||||
|<factor>||||||||
|<factor\_suffix>||||||||
|<number>||||||||
|<block>||||||||
|<statement\_list>||||||||
|<statement\_list1>||||||||
|<statement>||||||||
|<assignment>||||||||
|<assignment1>||||||||
|<arg\_list>||||||||
|<argument>||||||||
|<argument1>||||||||
|<build\_in\_fn>||||||||
|||||||||
|<if\_statement>||||||||
|<else\_statement>||||||||
|<while\_statement>||||||||
|<bin\_expression>||||||||
|<element\_bind>||||||||
|<return\_statement>||||||||
|<return\_statement1>||||||||


|[]u8|?|(|)|,|:|;|const|
| - | - | - | - | - | - | - | - |
||||||||1|
||||||||2|
||||||||5|
|7|7|||||||
|||||||||
|9|8|||||||
|||||||||
||||12|||||
||||14|13||||
|||||||||
||||||||16|
|||||||||
||||||18|||
|||20||||||
||||23|23||23||
|||24||||||
||||27|27||27||
|||31||||||
|||32|33|33||33||
|||||||||
|||||||||
||||||||37|
||||||||38|
||||||||40|
|||||||||
|||47||||||
|||49|50|||||
|||51||||||
||||53|52||||
|||||||||
|||||||||
|||||||||
||||||||57|
|||||||||
||||60|||||
|||||||||
|||||||||
|||64||||65||


|var|=|+|-|\*|/|{|}|
| - | - | - | - | - | - | - | - |
|1||||||||
|2||||||||
|6||||||||
|||||||||
|||||||||
|||||||||
|||||||||
|||||||||
|||||||||
|||||||||
|||||||||
|17||||||||
||19|||||||
|||||||||
|||21|22|||||
|||||||||
|||27|27|25|26|||
|||||||||
|||33|33|33|33|||
|||||||||
|||||||36||
|37||||||||
|38|||||||39|
|41||||||||
|||||||||
|||||||||
|||||||||
|||||||||
|||||||||
|||||||||
|||||||||
|||||||||
|57|||||||57|
|||||||||
|||||||||
|||||||62||
|||||||||
|||||||||


|ifj|.|<built\_in\_string>|if|else|while|||return|
| - | - | - | - | - | - | - | - |
|||||||||
|||||||||
|||||||||
|||||||||
|||||||||
|||||||||
|||||||||
|||||||||
|||||||||
|||||||||
|||||||||
|||||||||
|||||||||
|||||||||
|||||||||
|||||||||
|||||||||
|||||||||
|||||||||
|||||||||
|||||||||
||||37||37||37|
||||38||38||38|
||||43||44||45|
|||||||||
|48||||||||
|||||||||
|||||||||
|||||||||
|54||||||||
|||||||||
||||55|||||
||||57|56|57||57|
||||||58|||
|||||||||
|||||||61||
||||||||63|
|||||||||


|<binary\_operator>|identifier|integer|float|string\_literal|
| - | - | - | - | - |
||||||
||||||
||||||
||||||
||||||
||||||
||||||
||11||||
||||||
||15||||
||||||
||||||
||||||
||20|20|20|20|
|23|||||
||24|24|24|24|
|27|||||
||29|28|28|30|
|33|||||
|||34|35||
||||||
||37||||
||38||||
||42||||
||46||||
||47|47|47|47|
||49|49|49|49|
||51|51|51|51|
||||||
||||||
||||||
||||||
||57||||
||||||
|59|||||
||||||
||||||
||64|64|64|64|

