#include <iostream>
#include <vector>
#include <string.h>
#include <string>
#include <fstream>
#include <unordered_map>

using namespace std;

void AST_NODE_EXP(struct AST_Node* node);
void AST_NODE_STATEMENT(struct AST_Node* node);
void AST_NODE_FUNCTION(struct AST_Node* node);
void AST_NODE_PROGRAM(struct AST_Node* node);


enum tokenType {
    INT, //0
    IDENTIFIER, // 1 
    OPEN_PAREN, // 2
    CLOSE_PAREN, // 3
    OPEN_BRACE, // 4
    CLOSE_BRACE, // 5
    RETURN, // 6
    SEMICOLON, // 7
    CONSTANT // 8
};


unordered_map<string, tokenType> keywords = {
    {"int", INT},
    {"return", RETURN}
};



class token{
    public:
        token(tokenType t, string lex, string lit) : tok(t), lexeme(lex), literal(lit)  { };
        tokenType getTokenType() { return tok; }
        string getLexeme() { return lexeme; }
        string getLiteral() { return literal; }
    private:
        string literal;
        string lexeme; 
        tokenType tok;
};


vector<token*> tokens;


string source = "";
int start = 0;
int current = 0;
int line = 0;


bool isDigit(char c){
    return c >= '0' && c <= '9'; 
}

bool isAlpha(char c){
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           c == '_'; 
}

bool isAlphaNumeric(char c){
    return isAlpha(c) || isDigit(c);
}


bool isAtEnd(){ 
    return current >= source.size();
}

char peek(){
    if(isAtEnd()) return '\0';
    return source[current];    
}

char peekNext(){
    if(current + 1 >= source.size()) return '\0';
    return source[current + 1];
}

char advance(){ 
    return source[current++];
}


void addToken(tokenType type, string lexeme){
    string literal = source.substr(start, current - start);
    tokens.push_back(new token(type, lexeme, literal));
}


void addToken(tokenType type){
    addToken(type, "");
}

void number(){
    while(isDigit(peek())) advance();
    addToken(CONSTANT, source.substr(start, current - start));
}

void identifier(){
    while(isAlphaNumeric(peek())) advance();
    //do not use substr(start, current) in c++
    std::string text = source.substr(start, current - start);
    //cout << text << endl;
    tokenType type = (keywords.count(text)) ? type = keywords[text] : type = IDENTIFIER;
    addToken(type); 

}

void scanToken(){
    char c = advance();
    switch(c){
        case '(': addToken(OPEN_PAREN); break;
        case ')': addToken(CLOSE_PAREN); break;
        case '{': addToken(OPEN_BRACE); break;
        case '}': addToken(CLOSE_BRACE); break;
        case ';': addToken(SEMICOLON); break;
        case ' ':
        case '\t':
            break;
        case '\r':
        case '\n':
            line++;
            break;
        default:
            if(isDigit(c)){
                number();
            }
            else if(isAlpha(c)){
                identifier();
            }
            else{
                printf("Line %d. Unexpected error", line);
            }
            break;
    }
}

struct exp{
    string lit;
};

struct statement{
    string st;
    struct exp ex;
};

struct function {
    string returnType;
    string id;
    struct statement stmt;
};

struct program{
    struct function func;
};


enum grammerType{
    PROGRAM, 
    FUNCTION,
    STATEMENT,
    EXP
};



struct AST_Node{
    grammerType type;
    struct program prog;
    struct function func;
    struct statement stmt;
    struct exp ex;



    struct AST_Node* progr;
    struct AST_Node* funct;
    struct AST_Node* statement;
    struct AST_Node* expression;
};


struct AST_Node* root;
int currentToken = 0;
string result = "";

//Grammer
// <program> ::= <function>
// <function> ::= "int" <id> "(" ")" "{" <statement> "}"
// <statement> ::= "return" <exp> ";"
// <exp> ::= <int>


struct AST_Node* parseExp(){
    struct exp e;
    struct AST_Node* ast_node = new AST_Node;
    ast_node->type = EXP;
    if(tokens[currentToken]->getTokenType() != CONSTANT) exit(1);
    ast_node->ex.lit = tokens[currentToken++]->getLiteral();
    return ast_node;
}

struct AST_Node* parseStatement(){
    struct statement st;
    struct AST_Node* ast_node = new AST_Node;
    ast_node->type = STATEMENT;
    if(tokens[currentToken++]->getTokenType() != RETURN) {
        cout << "Wrong keyword" << endl;
        exit(1);
    }
    ast_node->expression = parseExp();
    if(tokens[currentToken++]->getTokenType() != SEMICOLON){
        cout << "missing colon" << endl;
        exit(1);
    }
    return ast_node;
}

struct AST_Node* parseFunction(){
    struct function temp;
    struct AST_Node* ast_node = new AST_Node;
    ast_node->type = FUNCTION;
    if(tokens[currentToken]->getTokenType() != INT){
        cout << "not an int" << endl;
        exit(1);
    }
    ast_node->func.returnType = "int";
    currentToken++;
    if(tokens[currentToken]->getTokenType() != IDENTIFIER){
        cout << "not an id" << endl;
        exit(1);
    }
    ast_node->func.id = tokens[currentToken]->getLiteral();
    currentToken++;
    if(tokens[currentToken++]->getTokenType() != OPEN_PAREN) {
        cout << "Missing open paren" << endl;
        exit(1);
    }
    if(tokens[currentToken++]->getTokenType() != CLOSE_PAREN) {
        cout << "Missing closing paren" << endl;
        exit(1);
    }
    if(tokens[currentToken++]->getTokenType() != OPEN_BRACE) {
        cout << "Missing open brace" << endl;
        exit(1);
    }
    ast_node->statement = parseStatement();

    //temp.stmt = parseStatement();
    if(tokens[currentToken++]->getTokenType() != CLOSE_BRACE) {
        cout << "Missing closing brace" << endl;
        exit(1);
    }
    return ast_node;
}

struct AST_Node* parseProgram(){
    struct AST_Node* ast_node = new AST_Node;
    ast_node->type = PROGRAM;
    ast_node->funct = parseFunction();
    return ast_node;
}

void parse(){
    root = parseProgram();
}



void codeGen(struct AST_Node* node){ 
    switch(node->type){
        case PROGRAM:
            AST_NODE_PROGRAM(node);
            break;
        case FUNCTION:
            AST_NODE_FUNCTION(node);
            break;
        case STATEMENT:
            AST_NODE_STATEMENT(node);
            break;
        case EXP:
            AST_NODE_EXP(node);
            break;
    }
}

void AST_NODE_EXP(struct AST_Node* node){
    result += "  mov eax, " + node->ex.lit + "\n";
}

void AST_NODE_STATEMENT(struct AST_Node* node){
    result += "  push ebp\n";
    result += "  mov ebp, esp\n";
    codeGen(node->expression);  
    result += "  pop ebp\n";
    result += "  ret\n";
}

void AST_NODE_FUNCTION(struct AST_Node* node){
    result += "  call " + node->func.id + "\n";
    result += "  mov ebx, eax\n";
    result += "  mov eax, 1\n";
    result += "  int 0x80\n\n";
    result += node->func.id + ":\n";
    codeGen(node->statement);
}

void AST_NODE_PROGRAM(struct AST_Node* node){
    codeGen(node->funct);
}


void scan(){
    while(!isAtEnd()){
        start = current;
        scanToken();
    }
}

void writeFile(){
    ofstream output("Example1.asm");
    //if file gives error, return couldnt open the file
    if(!output.is_open()){
        cout << "Error opening file.\n";
        return;
    }
    for(char c : result){
        output << c;
    }
    output << "\n";
    output.close();
}

void readFile(char* arg){
    char c;
    std::ifstream file(arg);
    if(!file.is_open()){
        std::cout << "Error opening file." << std::endl;
        return;
    } 

    while(file.get(c)){
        source += c;
    }

    result += "global _start\n\n";
    result += "_start:\n";

    scan();
    parse();
    codeGen(root);
    writeFile();
}


int main(int argc, char *argv[]){
    if(argc > 2) std::cout << "Use: g++ <filepath>" << std::endl;
    else if(argc == 2) readFile(argv[1]);
    return 0;
}