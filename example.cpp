#include "treeprint.h"

int main(int argc, const char * argv[]) {
    
    std::wcout.imbue(std::locale("en_US.UTF-8")); // A concrete UTF-8 standard has to be set in order for wcout to work!

    //Example usage:
    
    AST h = {"4", {}};
    AST g = {"2", {}};
    AST f = {"x", {}};
    AST e = {"Преступление", {}};
    AST d = {"ääöüöäüöäüöäüöäüöäüöäüöäöü", {}};
    AST c = {"ÉGÉìÉRÅ[ÉfÉBÉìÉOÇÕìÔÇµÇ≠Ç»Ç¢",{&g,&h}};
    AST b = {"Zebra1", {&d,&e,&f}};
    AST a = {"&", {&b,&c}};

    //either directly print it:
    PrintAST(a,3,true); //3 is min spacing in between the child nodes, true is for enabling Unicode characters in the tree.
    
    //or store the widestring:
    std::vector<std::wstring> representation;
    printPostOrder(a, representation, 0, 0, 3, true); //3 is min spacing in between the child nodes.
    
    
    return 0;
}
