/*
 ( ͡° ͜ʖ ͡°) °º¤ø,¸¸,ø¤º°`°º¤ø,¸,ø¤°º¤ø,¸¸,ø¤º°`°º¤ø,¸,ø¤º°`
 
 Currently this header file is under BSD 3-clause license.
 You can contact me if you want me to reconsider this license.
 
 ( ͡° ͜ʖ ͡°) °º¤ø,¸¸,ø¤º°`°º¤ø,¸,ø¤°º¤ø,¸¸,ø¤º°`°º¤ø,¸,ø¤º°`
 
 Copyright (c) 2017 (13 oct), Kevin De Keyser
 All rights reserved.
 
 ( ͡° ͜ʖ ͡°) °º¤ø,¸¸,ø¤º°`°º¤ø,¸,ø¤°º¤ø,¸¸,ø¤º°`°º¤ø,¸,ø¤º°`

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.
 * The name of its contributors may not be used to endorse or promote products
 derived from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#ifndef treeprint_h
#define treeprint_h

#include <cassert>
#include <codecvt>
#include <iostream>
#include <locale>
#include <vector>
#include <string>
#include "wchar.h"

struct AST {
    std::string expression;
    std::vector<AST*> children;
};


namespace utf8 {
#include "utf8.h"
    std::wstring toWideString(const std::string str) {
        std::wstring str2 = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(str.c_str());
        return str2;
    }
    
    std::size_t distance(std::string const &s) {
        return utf8::distance(s.begin(), s.end());
    }
}

//Recursive function that returns the width of the sub characters.
std::size_t printPostOrder(const AST & root, std::vector<std::wstring> & result, int depth, std::size_t pos, std::size_t MIN_CHILD_SPACING, bool UNICODE_BRACKETS) {
    assert(MIN_CHILD_SPACING % 2 == 1 && "MIN_CHILD_SPACING must be odd in order to achieve best symmetric results. You can comment-out this assertion if this doesn't matter to you.");
    
    //Make sure position is ok.
    while(depth >= result.size()) result.push_back(L"");
    for(std::size_t i = result[depth].size(); i < pos; ++i) result[depth] += L" ";
    assert(result[depth].size() == pos);
    
    //Compute the children and their sizes
    std::size_t childSpace = 0;
    for(std::size_t i = 0; i < root.children.size(); ++i) {
        std::size_t childWidth = printPostOrder(*root.children[i], result, depth + 2, pos + childSpace, MIN_CHILD_SPACING, UNICODE_BRACKETS);
        //slight correction
        if(childWidth % 2 == 0) childWidth--;
        //Add the spaces or lines until the child is reached.
        std::size_t childPos = pos + childSpace + childWidth/2;
        if(i == 0) {
            for(std::size_t i = result[depth+1].size(); i < childPos; ++i) result[depth+1] += L" ";
        } else {
            for(std::size_t i = result[depth+1].size(); i < childPos; ++i) result[depth+1] += UNICODE_BRACKETS?L"─":L"-";
        }
        
        //Add specific downward slope
        if(root.children.size() == 1) result[depth+1] += UNICODE_BRACKETS?L"│":L"|";
        else if(i == 0) result[depth+1] += UNICODE_BRACKETS?L"╭":L"+";
        else if(i+1 == root.children.size()) result[depth+1] += UNICODE_BRACKETS?L"╮":L"+";
            else result[depth+1] += UNICODE_BRACKETS?L"┬":L"+";
        
        childSpace += childWidth;
        if(i+1 != root.children.size()) childSpace += MIN_CHILD_SPACING + 1;
    }
    
    std::size_t parentPosition = 0;
    //Add additional space to the parent node, according to the size of the children.
    if(childSpace >= utf8::distance(root.expression)) {
        parentPosition = pos + (childSpace - utf8::distance(root.expression)) / 2 - (utf8::distance(root.expression)%2==0);
        for(std::size_t i = pos; i < parentPosition; ++i) result[depth].push_back(' ');
    }
    //In case the width of the parent is larger than the width of the children, add additional space to the children.
    else if(root.children.size() > 0) {
        //insert space into the children at position pos.
        
        std::size_t additionalSpace = (utf8::distance(root.expression) - childSpace) / 2;
        parentPosition = pos - (utf8::distance(root.expression)%2==0);
        for(int i = depth + 1; i < result.size(); ++i) {
            while(pos+1 >= result[i].size()) result[i].push_back(' ');
            if(pos < result[i].size()) {
                std::wstring before = result[i].substr(0,pos);
                for(std::size_t i = 0; i < additionalSpace; ++i) before.push_back(' ');
                before += result[i].substr(pos);
                result[i] = before;
            }
        }
    }
    
    result[depth] += utf8::toWideString(root.expression);
    
    //Last but not least add uplink towards the chain.
    if(root.children.size() > 0) {
        //Set parentPostition from front position to middle position
        parentPosition = parentPosition + utf8::distance(root.expression)/2;
        if(UNICODE_BRACKETS) {
            if     (result[depth+1].substr(parentPosition,1) == L"─") result[depth+1].replace(parentPosition,1,L"┴");
            else if(result[depth+1].substr(parentPosition,1) == L"┬") result[depth+1].replace(parentPosition,1,L"┼");
            else if(result[depth+1].substr(parentPosition,1) == L"╭") result[depth+1].replace(parentPosition,1,L"├");
            else if(result[depth+1].substr(parentPosition,1) == L"╮") result[depth+1].replace(parentPosition,1,L"┤");
        }
        else {
            if     (result[depth+1].substr(parentPosition,1) == L"-") result[depth+1].replace(parentPosition,1,L"+");
        }
    }
    if(childSpace < utf8::distance(root.expression)) return utf8::distance(root.expression); //root.expression.size();
    else return childSpace;
}


void PrintAST(const AST & root, int MIN_CHILD_SPACING, bool UNICODE_BRACKETS) {
    // A concrete UTF-8 standard has to be set in order for wcout depending on the implementation!
    // std::wcout.imbue(std::locale("en_US.UTF-8"));

    std::vector<std::wstring> representation;
    printPostOrder(root, representation, 0, 0, 3, UNICODE_BRACKETS);
    for(std::size_t i = 0; i < representation.size(); ++i) {
        std::wcout << representation[i] << std::endl;
    }
    std::wcout << std::flush;
}

#endif /* treeprint_h */
