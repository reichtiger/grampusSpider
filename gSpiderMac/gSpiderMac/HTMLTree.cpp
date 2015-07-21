//#include "stdafx.h"

#include "HTMLTree.h"

HTMLTree::HTMLTree(void)
{
    pFirstChild = NULL;
    nChildBranchNum = false;
    depth = 0;
    dom_versioninfo =  NULL;
    debugMode = false;
}

HTMLTree::~HTMLTree(void)
{
    HTMLBranch* a = (HTMLBranch*)pFirstChild;
    HTMLBranch* b;
    while (a != NULL)
    {
        b = a->sibling_next;
        delete a;
        a = b;
    }
    if (dom_versioninfo != NULL) delete dom_versioninfo;
    //printf("~ HTML-Tree\n");
}

/**
 *	using by : HTMLBranch* mytree = HTMLTree::getHTMLTree(str); // must HTMLBranch class pointer
 *
 */

HTMLBranch* HTMLTree::getRoot(wchar_t* inStr, wchar_t* rootTagName, bool debug, wchar_t* basePath)
{
    // outside program can use as below directly.
    // use getHTMLTree can not set callback_function pointers !!!!!!
    
    HTMLBranch* fake_root = new HTMLBranch(false, L"**fake**");// create for parsing
    fake_root->debugMode = debug;
    fake_root->isFakeBranch = true;
    
    
    fake_root->parseBranches(inStr);
    
    vector<HTMLBranch *> list;
    int num = fake_root->getBranchesByTagName(rootTagName, list);
    
    if (num == 1) {
        printf("root found and got\n");
        return list[0];
    }
    
    return NULL;
    
}

/*
 * check if it is useable char
 *
 * char a1 = 'A'; // 65
 * char a2 = 'Z'; // 90
 * char a3 = 'a'; // 97
 * char a4 = 'z'; // 122
 * char a5 = '1'; // 49
 * char a6 = '9'; // 57
 * char a7 = '_'; // 95
 * char a8 = '.'; // 46
 * int i = a1
 *
 * printf("%d - %d, %d - %d, %d, %d, %d\n", i,a2,a3,a4, a5, a6, a7);
 *
 */
bool HTMLTree::isUsableChar(wchar_t inc)
{
#ifndef UNICODE
    int i = inc;
    
    if ( (i >= 65 && i <= 90) || (i >= 97 && i <= 122) || (i >= 49 && i <= 57) || (i == 95) || (i == 46))
        return true;
    return false;
#else
    
    int i = inc;
    
    if ((i >= 65 && i <= 90) || (i >= 97 && i <= 122) || (i >= 49 && i <= 57)
        || (i == 95) || (i == 46)
        || (i == 45)
        )
        return true;
    return false;
    
    if (inc <=19968 && inc >=40869){
        // chinese words
        return false;
    }
    else{
        return true;
    }
    
#endif
    
}

/************************************************************************
 * HTMLBranch part
 *
 */

HTMLBranch* HTMLBranch::parseBranches(wchar_t* inStr, wchar_t* basePath)
{
    if (!inStr) {
        return NULL;
    }
    //printf("\n\n=============\n%s\n", basePath);
    wchar_t* p = NULL;
    wchar_t* mvp = NULL;
    p = inStr;
    wchar_t* pRealTagEnd = NULL;
    wchar_t* pTagStartEnd = NULL;
    
    wchar_t* pTagStartChar = NULL;  // '<' position, in case some string before it.
    
    bool isEnd = false;
    bool tagEnd = false;
    
    HTMLBranch* pCurHasLeaf = NULL;
    
    bool tagFinish = false;
    bool short_tag = false; // <test />
    
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') p++;
    
    wchar_t* real_char_start = p;
    
    while(*p != '\0' && p != NULL)
    {
        short_tag = false;
        
        if (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') {
            p++;
            continue;
        }
        
        // tar start
        
        if(*p == '<' && isUsableChar(*(p+1)))
        {
            // if has leaf ahead, handle it
            
            //bTagStart = true;
            pTagStartChar = p;
            
            tagEnd = false;
            //bTagNameStart = true;
            mvp = p + 1;
            
            while(*mvp != ' ' && isUsableChar(*mvp) ) mvp++;
            
            if (mvp-p-1 >BRANCH_NAME_LEN)
            {
                printf("[ERROR] tag name too long\n");
                return NULL;			// make it as Leaf
            }
            //bTagNameStart = false;
            
            HTMLBranch* nb = new HTMLBranch;
            nb->debugMode = debugMode;
            nb->ptrCallBack_htmlTag = this->ptrCallBack_htmlTag;
            nb->ptrCallBack_foundjs = this->ptrCallBack_foundjs;
            
            wcsncpy(nb->brch_name, (p + 1), mvp - p - 1);
            
            const wchar_t* c1 = L"<";
            wcsncpy(nb->brch_tag_start, c1, 1);
            wcsncpy(nb->brch_tag_start + 1, nb->brch_name, wcslen(nb->brch_name));
            const wchar_t* c2 = L"</";
            const wchar_t* c3 = L">";
            wcsncpy(nb->brch_tag_end, c2, 2);
            wcsncpy(nb->brch_tag_end + 2, nb->brch_name, wcslen(nb->brch_name));
            wcsncpy(nb->brch_tag_end + 2 + wcslen(nb->brch_name), c3, 1);
            
            
            size_t name_len = wcslen(nb->brch_name)+1;
            const wchar_t* path_split = L"/";
            
            if (basePath == NULL){
                nb->brch_path = new wchar_t[name_len]; wmemset(nb->brch_path, 0, name_len);
                wcsncpy(nb->brch_path, nb->brch_name, name_len-1);
                depth =  0;
                nb->depth = 1;
            }else{
                size_t addMoreLen = wcslen(basePath)+1;
                nb->brch_path = new wchar_t[addMoreLen + name_len]; wmemset(nb->brch_path, 0, addMoreLen + name_len-1);
                wcscat(nb->brch_path, basePath);
                wcscat(nb->brch_path, path_split);
                wcscat(nb->brch_path, nb->brch_name);
                
                nb->depth = depth + 1;
            }
            
            // test start ---------------
            if (wcscmp(nb->brch_name, L"link") == 0)
                printf("[path=] %ls\n", nb->brch_path);
            
            // (wcscmp(nb->brch_name, L"head") == 0) printf("tagName = %ls\n", nb->brch_name);
            
            // test end ------------------
            
            if (debugMode){
                //printf("path=[%ls]\n", nb->brch_path);
                if (wcscmp(nb->brch_path, L"html/body/IFRAME/NOSCRIPT/IMG") ==0){
                    ;
                }
            }
            
            mvp = p + name_len;
            // first we should get the '>' end char, the real '>' char should only followed with '\t''\n' and '<' or '/>'
            
            while (*mvp == ' ' || *mvp == '\t') mvp++;
            
            if ((*mvp == '>') || (*mvp == '/' && *(mvp+1) == '>') ){
                tagEnd = true;
                if ((*mvp == '/' && *(mvp+1) == '>')) {
                    short_tag = true;
                    addBranch(nb);
                }
            }else{
                
                
                while(!tagEnd){
                    
                    while (*mvp == ' ' || *mvp == '\t') mvp++;
                    
                    if ((*mvp == '>') || (*mvp == '/' && *(mvp+1) == '>') )
                    {
                        tagEnd = true;
                        
                        if (*mvp == '/' && *(mvp+1) == '>'){
                            
                            if (wcscmp(nb->brch_name, L"a") == 0) {
                                // <a /> should not set short tag
                                //printf("");
                            }else{
                                short_tag = true;
                                addBranch(nb);
                            }
                            
                        }
                        break;
                    }
                    //now try to get attributes
                    while(*mvp == ' ' || *mvp == '\t' || !isUsableChar(*mvp))
                        mvp++;
                    
                    wchar_t* attr_name = mvp;
                    while (isUsableChar(*mvp))
                        mvp++;
                    
                    //attrnameStart = false;
                    attribute abe;
                    wcsncpy(abe.attr_name, attr_name, mvp-attr_name);
                    //printf("attrname=%ls\n", abe.attr_name);
                    
                    while (*mvp != '=' && *mvp != '\0') mvp++;
                    
                    if (*mvp == '\0'){
                        isEnd = true;
                        break;
                    }
                    
                    // we found '=' for attribute
                    mvp++;
                    while ((*mvp == ' ') || (*mvp == '\t') || (*mvp == '\n')) {
                        mvp++;
                    }
                    
                    bool attrValueQuoteTag1 = false; // for " start
                    bool attrValueQuoteTag2 = false; // for ' start
                    
                    if (*mvp == '"'){
                        attrValueQuoteTag1 = true;
                    }else if(*mvp == '\''){
                        attrValueQuoteTag2 = true;
                    }
                    
                    mvp++;
                    
                    wchar_t* valueStart = mvp;
                    
                    if (attrValueQuoteTag1) {        // if true, to find ', or " as the end
                        while (*mvp != '"'){
                            if (*mvp == '>') {
                                // if no found end quote but meet > , then mismatch , drop and break.
                                goto tagError;
                            }
                            mvp++;
                        }
                    }
                    else if (attrValueQuoteTag2){
                        while (*mvp != '\''){
                            if (*mvp == '>') {
                                // if no found end quote but meet > , then mismatch , drop and break.
                                goto tagError;
                            }
                            mvp++;
                        }
                        
                    }
                    else{                          // to find ' '  as the end
                        try {
                            while (*mvp != ' '){
                                if (*mvp == '>') {  // if found endTag ,then attrValue stopped.
                                    mvp--;
                                    break;
                                }
                                mvp++;
                            }
                            
                        }
                        catch (...) {
                            printf("catch ...\n");
                        }
                        
                    }
                    
                    long len = (mvp - valueStart);
                    if ( len >= 0 )
                    {
                        abe.attr_value = new wchar_t[len + 1];
                        wmemset(abe.attr_value, 0, len+1);
                        wcsncpy(abe.attr_value, valueStart, len);
                    }
                    // attr value start
                    // printf("attr_name=%ls, value=%ls\n", abe.attr_name, abe.attr_value);
                    
                    //
                    //if (wcscmp(nb->brch_name, L"script") == 0) {
                    //    if ( wcscmp(abe.attr_value, L"http://nfa.jd.com/loadFa.js?aid=2_164_819") == 0 )
                    //        printf("FOUND script src ------%ls\n", abe.attr_value);
                    //}
                    nb->attrs.push_back(abe);
                    nb->hasAttributes = true;
                    mvp++;
                }
                
            }
            
            pTagStartEnd = mvp;
            
            
            // now to found the end tag </XXX>
            wchar_t* pStartTag = NULL;
            wchar_t* pEndTag = NULL;
            wchar_t* pEndTagPin = NULL;
            
            // check if it is a short tag like : <name />
            if (tagEnd && !tagFinish && !short_tag)
            {
                // first handle leaf if there are string ahead the tag start!!!!!
                if (pTagStartChar != NULL) {
                    wchar_t* pp = real_char_start;
                    while (*pp == '\t' || *pp == '\n' || *pp == '\r'  || *pp == ' ') pp++;
                    
                    real_char_start = pp;
                    
                    size_t before_leaf_len = pTagStartChar - real_char_start;
                    if (before_leaf_len > 0) {
                        // see if all blank chars
                        wchar_t* freg = new wchar_t[before_leaf_len + 1];
                        wmemset(freg, 0, before_leaf_len+1);
                        wcsncpy(freg, real_char_start, before_leaf_len);
                        
                        HTMLBranch* xl = new HTMLBranch;
                        xl->isLeaf = true;
                        xl->pLeafValue = freg;
                        
                        xl->pLeafValue[before_leaf_len] = '\0';
                        xl->type = TYPE_DOM_FREG;
                        printf("ahead leaf = %ld, 【%ls】\n", before_leaf_len, xl->pLeafValue);
                        
                        addBranch(xl);
                    }
                    
                    
                }
                
                // start to find endTag
                wchar_t* base1 = mvp + 1;
                wchar_t* base2 = mvp + 1;
                int content_len = -1; // default -1 means has child branch
                
                pEndTagPin = wcsstr(base2, nb->brch_tag_end);
                
                /**
                 *  pass other child tags like : <div> <div> ...</div> ...</div>
                 *
                 */
                int x = 0;
                bool found_real_end_tag = false;
                while ( pEndTagPin != NULL)
                {
                    pStartTag = wcsstr(base1, nb->brch_tag_start);
                    pEndTagPin = wcsstr(base2, nb->brch_tag_end);
                    if (pStartTag && pEndTagPin) {
                        if (pStartTag < pEndTagPin)
                        {
                            base1 = pStartTag + 1;
                            base2 = pEndTagPin+1;
                            pEndTag = pEndTagPin;
                            x += 1;
                            continue;
                        }
                        else{
                            found_real_end_tag = true;
                            pEndTag = pEndTagPin;
                            break;
                        }
                    }
                    else{
                        found_real_end_tag = true;
                        if (pStartTag == NULL) {
                            pEndTag = pEndTagPin;
                            break;
                        }
                    }
                    
                    
                }
                // not found at least one endTag, it is a content, or <img>
                
                if (!found_real_end_tag) {
                    // error here
                    if (wcscmp(nb->brch_name, L"img") ==0) {
                        printf("GOT img tag\n");
                    }
                    
                }
                
                if (pEndTag != NULL){
                    pRealTagEnd = pEndTag;
                    real_char_start = pRealTagEnd + wcslen(nb->brch_name) +3;
                }
                else{
                    if (pTagStartEnd != NULL)
                    {
                        // it is unregular tag, also not short tag, is like <meta a="1"> ...
                        // take left as sibling or child ? here is sibling
                        /*	<meta a="1">
                         <meta a="2">
                         <link>
                         <link>....
                         </head>
                         
                         so we take left as sibling , just move ptr to next and continue loop.
                         */
                        
                        p = pTagStartEnd + 1;
                        addBranch(nb);
                        real_char_start = p;
                        
                        continue;
                        
                        
                    }
                    else
                        pCurHasLeaf = NULL;		// this is a Leaf !
                    
                    break;
                }
                
                content_len = pEndTag - pTagStartEnd-1;
                if (content_len > 0){
                    nb->strBuffer = new wchar_t[content_len + 1];
                    wmemset(nb->strBuffer, 0, content_len+1);
                    wmemcpy(nb->strBuffer, pTagStartEnd+1, content_len); // from '>' start copy
                    
                    if ( (wcscmp(nb->brch_name, L"script") == 0) || (wcscmp(nb->brch_name, L"SCRIPT") == 0) ){
                        // it is script code, no more parse into...
                        //printf("it is  ................... <script>, leaf=%ls\n", nb->strBuffer);
                        
                        if (ptrCallBack_foundjs != NULL){
                            ptrCallBack_foundjs(this, nb);
                        }
                        
                        nb->addBranch(new HTMLBranch(true, nb->strBuffer));
                        delete [] nb->strBuffer;
                    }
                    else if ((wcscmp(nb->brch_name, L"style") == 0) || (wcscmp(nb->brch_name, L"STYLE") == 0) ){
                        //printf("it is CSS <style>\n");
                        nb->addBranch(new HTMLBranch(true, nb->strBuffer));
                        delete [] nb->strBuffer;
                    }
                    else{
                        pCurHasLeaf = nb->parseBranches(nb->strBuffer, nb->brch_path);
                        delete [] nb->strBuffer;
                        nb->strBuffer = NULL;
                        // callback function
                        if (ptrCallBack_htmlTag != NULL){
                            ptrCallBack_htmlTag(this, nb);
                        }
                    }
                    
                    tagFinish = true;
                    pRealTagEnd += wcslen(nb->brch_tag_end);
                    
                }
                
                addBranch(nb);
                
            }
            
            
        }// end if tagStart
        
        // parse sibling branches
        
        if(!isEnd) {
            if (tagFinish){
                p = pRealTagEnd;
                
                
            }else if (short_tag)
            {
                p = mvp + 2;
                real_char_start = p;
                
            }else{
                p = p + 1;
            }
            
            tagFinish = false;
        }
        else break;
        
    }// end while_1
    
    if (pCurHasLeaf == NULL){
        
        if (wcscmp(brch_name, L"body") == 0) {
            printf("head having leaf...?????\n");
        }
        if (basePath == NULL){
            // this is not a tree, or no root in this HTML
            return NULL;
        }
        
        HTMLBranch* xl = new HTMLBranch;
        xl->isLeaf = true;
        xl->brch_path = new wchar_t[wcslen(basePath) + 1]; memset(xl->brch_path, 0, wcslen(basePath) + 1);
        wcsncpy(xl->brch_path, basePath, wcslen(basePath)+1);
        
        wchar_t* pValue = real_char_start;
        while( (*pValue != '\0') && (pValue != NULL) )
        {
            if ((*pValue) == '\t' || (*pValue) == '\r' || (*pValue) == '\n') {
                pValue++;
                continue;
            }else
                break;
        }
        
        wchar_t* pEnd = pValue + wcslen(pValue) - 1;
        while ( ( pEnd!=NULL ) && ((*pEnd) != '\0') )
        {
            if ((*pEnd) == ' ' || (*pEnd) == '\t' || (*pEnd) == '\r' || (*pEnd) == '\n') {
                pEnd--;
                continue;
            }else
                break;
        }
        pEnd++;
        
        long leaf_len = pEnd - pValue;
        
        if(leaf_len >0){
            xl->pLeafValue = new wchar_t[leaf_len + 1];
            wmemset(xl->pLeafValue, 0, leaf_len+1);
            wcsncpy(xl->pLeafValue, pValue, leaf_len);
            addBranch(xl);
        }
        
        //printf("Leaf Path :%s\n", xl->pLeafPath);
        //printf("Leaf Value=%s\n", xl->pLeafValue);
        
        // callback function
        if (ptrCallBack_htmlTag != NULL){
            ptrCallBack_htmlTag(this, this);
        }
        
        
        //printf("Waiting for any key input, or Enter to continue...\n"); getchar();
    }
    else{
        // there has leaf behind the branch ! append it as last leaf branch.....
        
        while ((*pRealTagEnd) == '\t' || (*pRealTagEnd) == '\r' || (*pRealTagEnd) == '\n')
            pRealTagEnd++;
        
        size_t last_leaf_len = wcslen(pRealTagEnd);
        if(last_leaf_len > 0) {
            HTMLBranch* leafBr = new HTMLBranch;
            leafBr->isLeaf = true;
            leafBr->pLeafValue = new wchar_t[last_leaf_len + 1];
            wmemset(leafBr->pLeafValue, 0, last_leaf_len+1);
            wcsncpy(leafBr->pLeafValue, pRealTagEnd, last_leaf_len);
            addBranch(leafBr);
        }
        
    }
    
    return this;
    
tagError:
    return NULL;
}

bool HTMLBranch::addBranch(HTMLBranch* brh)
{
    // reset all depth of new added branches
    
    HTMLBranch* t = brh;
    
    t->pParent = this;
    
    int step = 1;
    while (t != NULL)
    {
        t->depth = depth + step;
        t = t->pFirstChild;
        step += 1;
    }
    
    // add to branch as child branch
    if (pFirstChild == NULL)
        pFirstChild = brh;
    else		// there has a child, insert as sibling branch
    {
        HTMLBranch* cur = pFirstChild;
        while (cur->sibling_next != NULL){
            cur = cur->sibling_next;
        }
        
        cur->sibling_next = brh;
        brh->sibling_pre = cur;
        
    }
    
    nChildBranchNum += 1;
    
    return true;
}


bool HTMLBranch::insertBranchBefore(HTMLBranch* brch, HTMLBranch* before_node)
{
    HTMLBranch* pTargetNode = pFirstChild;
    
    while (pTargetNode != NULL) {
        if (pTargetNode == before_node) {
            
            if (before_node == pFirstChild) {
                brch->sibling_next = pFirstChild;
                pFirstChild->sibling_pre = brch;
                pFirstChild = brch;
                
            }else{
                brch->sibling_pre = before_node->sibling_pre;
                before_node->sibling_pre = brch;
                brch->sibling_next = before_node;
            }
            
            this->nChildBranchNum += 1;
            return true;
        }
        pTargetNode = pTargetNode->sibling_next;
        
    }
    return false;
}

void HTMLBranch::addSiblingAfter(HTMLBranch* target_node, HTMLBranch* br)
{
    if (target_node) {
        target_node->sibling_next = br;
        br->pParent = target_node->pParent;
    }
}

void HTMLBranch::InitBranch()
{
    ptrCallBack_htmlTag = NULL;
    ptrCallBack_foundjs = NULL;
    sibling_pre = NULL;
    sibling_next = NULL;
    nChildBranchNum = 0;
    pFirstChild = NULL;
    hasAttributes = false;
    depth = 0;
    isLeaf = false;
    isFakeBranch = false;
    type = TYPE_DOM_BRANCH;
    
    wmemset(brch_name, 0, BRANCH_NAME_LEN);
    wmemset(brch_tag_start, 0, BRANCH_NAME_LEN+1);
    wmemset(brch_tag_end, 0, BRANCH_NAME_LEN+3);
}
// default constructor
HTMLBranch::HTMLBranch(void)
{
    InitBranch();
}

HTMLBranch::HTMLBranch(wchar_t* path, wchar_t* leafValue)
{
    InitBranch();
    
    
    wchar_t* p = path;
    while ((*p == '/') || (*p == ' ') || (*p == '\t') ) // drop some chars
    {
        p++;
    }
    path = p;  // reset the head
    
    
    if (wcscmp(path, L"") > 0){
        
        wchar_t* end_name = wcsstr(path, L"/");
        
        if (end_name == NULL){			// it is end of node
            if (leafValue != NULL){
                //leaf = new HTMLLeaf(leafValue);
                
            }
            wcscpy(brch_name, path);
            
        }else{
            wcsncpy(brch_name, path, end_name-path);
            
            if (wcscmp(end_name, L"/") != 0){
                nChildBranchNum = 1;
                this->pFirstChild = new HTMLBranch(end_name, leafValue);
            }else{
                // still end node
                if (leafValue != NULL){
                    //leaf = new HTMLLeaf(leafValue);
                    
                }
            }
        }
        
        // set branch attributes
        long name_len = wcslen(brch_name);
        
        const wchar_t c1 = '<';
        const wchar_t* c2 = L"</";
        const wchar_t c3 = '>';
        wmemcpy(brch_tag_start,  &c1, 1);
        wmemcpy(brch_tag_start+1, brch_name, name_len);
        
        wmemcpy(brch_tag_end, c2, 2);
        wmemcpy(brch_tag_end+2, brch_name, name_len);
        wmemcpy(brch_tag_end+2+name_len,  &c3, 1);
    }
    
}
/*
 * this is for :  new HTMLBranch("a/b/c/d");
 * or :   new HTMLBranch("a/b/c/d", "hahaha");
 *
 */
HTMLBranch::HTMLBranch(PATH_NODES* head, wchar_t* leafValue, int deep)
{
    InitBranch();
    
    wmemset(brch_name, 0, 32);
    wmemset(brch_tag_start, 0, 33);
    wmemset(brch_tag_end, 0, 35);
    if (head != NULL)
    {
        wcscpy(brch_name, head->path_name);
        const wchar_t c1 = '<';
        const wchar_t* c2 = L"</";
        const wchar_t c3 = '>';
        wmemcpy(brch_tag_start,  &c1, 1);
        wmemcpy(brch_tag_start+1, head->path_name, wcslen(head->path_name));
        
        wmemcpy(brch_tag_end, c2, 2);
        wmemcpy(brch_tag_end+2, head->path_name, wcslen(head->path_name));
        wmemcpy(brch_tag_end+2+wcslen(head->path_name),  &c3, 1);
    }
    
    
    depth = deep;
    
    if (head != NULL && head->next != NULL) {
        //string sLast = getLastPath(head->next,name);
        nChildBranchNum = 1;
        if (head->next != NULL) this->pFirstChild = new HTMLBranch(head->next, leafValue, depth+1);
    }else if(head != NULL){
        //
        //		HTMLLeaf* xl = new HTMLLeaf;
        //		xl->pLeafValue = new wchar_t[wcslen(leafValue) + 1];
        //		wmemset(xl->pLeafValue, 0, wcslen(leafValue));
        //		wcscpy(xl->pLeafValue, leafValue);
        //		((HTMLBranch*)this)->addBranch(xl);
        
    }
}

// create new branch as Leaf, mostly fake_root
HTMLBranch::HTMLBranch(bool is_leaf, wchar_t* leafValue)
{
    
    InitBranch();
    return;
    
    isLeaf = is_leaf;
    if (isLeaf) {
        size_t cont_len = wcslen(leafValue);
        pLeafValue = new wchar_t[cont_len+1];
        wcsncpy(pLeafValue, leafValue, cont_len);
        pLeafValue[cont_len] = '\0';
    }
    else{
        // it must be a fake tree in this function
        int nlen = wcslen(leafValue);
        if (nlen < 32) {
            wmemcpy(brch_name, leafValue, nlen);
        }
        
    }
}

HTMLBranch::~HTMLBranch()
{
    // remove child branches first
    
    if (isFakeBranch == false) {
        HTMLBranch* a = (HTMLBranch*)pFirstChild;
        HTMLBranch* b;
        while (a != NULL)
        {
            b = a->sibling_next;
            delete a;
            a = b;
        }
        
        // then clear leaf and attrs
        int i = 0;
        vector <ATTRIBUTE>::iterator itorAttr;
        for(itorAttr = attrs.begin(); itorAttr != attrs.end(); itorAttr++)
        {
            //printf("[%ls] attribute value removed\n", attrs[i].attr_value);
            delete attrs[i].attr_value;
            attrs[i].attr_value = NULL;
            i++;
        }
        
        //attrs.clear();
        
        if (isLeaf) {
            //printf(".........freeing leaf %ls\n", pLeafValue);
            delete pLeafValue;
            
        }
        
        sibling_pre = NULL;
        sibling_next = NULL;
        
    }
    else{
        wprintf(L"removing fake_root tree.....%ls\n", brch_name);
    }
    
    if (debugMode) {
        if (isLeaf)
            wprintf(L"removing HTML-Leaf\n");
        else
            wprintf(L"removing HTML-Branch %ls\n", brch_name);
    }
    
}

/*  this function invoke  getBranches and return array of branches
 */
int HTMLBranch::get_all_branches(wchar_t* path, HTMLBranch** &in_list, wchar_t* attrName, wchar_t* attrValue)
{
    if (path == NULL) return NULL;
    PATH_NODES* head = getPathNodes(path);
    
    queryList = new HTMLBranch_list;
    HTMLBranch_list* ret = get_branch(queryList, head, attrName, attrValue);
    
    if(ret->index >0){
        in_list = new HTMLBranch*[ret->index];
        
        HTMLBranch_list* tmp = queryList;
        HTMLBranch_list* free_use;
        int x = 0;
        
        while (tmp->node != NULL)
        {
            in_list[x] = tmp->node;
            x++;
            free_use = tmp->next;
            tmp->node = NULL;
            delete tmp;
            tmp = free_use;				// next
        }
        
    }
    queryList = NULL;
    
    free_path_nodes(head);
    
    return ret->index; // return numbers of gotten branches
}

int HTMLBranch::getBranchesByTagName(wchar_t* tagName, vector <HTMLBranch*>& list, wchar_t* attrName, wchar_t* attrV)
{
    //printf("cur brch %ls\n", brch_path);
    int num = 0;
    if (wcscmp(tagName, brch_name) ==0) {
        if ((attrName != NULL) && (attrV != NULL)) {
            // now match attrName
            
            int n = attrs.size();
            for (int i = 0; i < n; i++) {
                if ( (wcscmp(attrs[i].attr_name, attrName) == 0)
                    && (wcscmp(attrs[i].attr_value, attrV) == 0)
                    ) {
                    list.push_back(this);
                    num  += 1;
                }
            }
        }
        
        else{
            list.push_back(this);
            num += 1;
        }
        
    }
    
    // check child first
    HTMLBranch* c = pFirstChild;
    if (c) {
        num += c->getBranchesByTagName(tagName, list, attrName, attrV);
    }
    
    // then ask sibling brother, the brother will ask HIS sibling in his function....
    // so this is the point !!!
    HTMLBranch* next = (HTMLBranch*)sibling_next;
    if  (next) {
        if (next->depth > this->depth) {
            num += next->getBranchesByTagName(tagName, list, attrName, attrV);
        }
        
    }
    
    return num;
}
/*
 *
 *
 *	C/C++
 */
int HTMLBranch::free_lists(HTMLBranch** &in_list, int size)
{
    
    /*int size_freed = size;
     HTMLBranch** tmp = in_list;
     
     while (size_freed > 0)
     {
     (*tmp) = NULL;
     tmp++;
     size_freed--;
     }*/
    
    delete [] in_list;
    
    return 0;
}

/**
 *  get one branch by attrname and value
 */
HTMLBranch* HTMLBranch::getBranchByAttrname(wchar_t* attrName, wchar_t* attrValue)
{
    HTMLBranch* ret = NULL;
    
    if (wcscmp(brch_name, L"div")==0) {
        ;
    }
    // check self
    if (hasAttributeName(attrName)){
        wchar_t* v = getAttribute(attrName);
        if (v != NULL){
            if (wcscmp(v, attrValue) == 0){
                return this;
            }
        }
    }
    
    // else search childs and siblings
    HTMLBranch* c = pFirstChild;
    if (c) {
        ret = c->getBranchByAttrname(attrName, attrValue);
    }
    
    if (ret != NULL) {
        return ret;
    }
    // search brothers
    
    HTMLBranch* next = sibling_next;
    if  (next) {
        ret = next->getBranchByAttrname(attrName, attrValue);
    }
    
    return ret;
}

int HTMLBranch::getBranchesByAttr(vector <HTMLBranch*>& list, wchar_t* attrName, wchar_t* attrValue)
{
    
    // check self
    if (hasAttributeName(attrName)){
        wchar_t* v = getAttribute(attrName);
        if (v != NULL){
            if (wcscmp(v, attrValue) == 0){
                list.push_back(this);
            }
        }
    }
    
    // else search childs and siblings
    HTMLBranch* c = pFirstChild;
    if (c) {
        c->getBranchesByAttr(list, attrName, attrValue);
    }
    
    // search brothers
    
    HTMLBranch* next = sibling_next;
    if  (next) {
        next->getBranchesByAttr(list, attrName, attrValue);
    }
    
    return 0;
}
/*
 *  get branches by path
 *  ex : input is path=a/b/c/d
 *  then returns the branches in a/b/c/d.
 *
 */

HTMLBranch_list* HTMLBranch::get_branch(HTMLBranch_list* nl, PATH_NODES* cur_node, wchar_t* attrName, wchar_t* attrValue)
{
    if (nl == NULL) return NULL;
    if (cur_node == NULL) return nl;
    
    HTMLBranch* n = this;
    while (n != NULL)
    {
        if (wcscmp(n->brch_name, cur_node->path_name) == 0)
        {
            if (cur_node->next == NULL){
                
                if (attrName == NULL)
                {
                    nl->index += 1;
                    nl->node = n;
                    int last_index = nl->index;
                    HTMLBranch_list* new_node = new HTMLBranch_list;
                    nl->next = new_node;
                    new_node->index = last_index;
                    nl = new_node;
                }else{
                    if (n->hasAttributeName(attrName)){
                        
                        if (attrValue == NULL){
                            nl->index += 1;
                            nl->node = n;
                            int last_index = nl->index;
                            HTMLBranch_list* new_node = new HTMLBranch_list;
                            nl->next = new_node;
                            new_node->index = last_index;
                            nl = new_node;
                        }else{
                            wchar_t* v = n->getAttribute(attrName);
                            if (v != NULL){
                                if (wcscmp(v, attrValue) == 0){
                                    nl->index += 1;
                                    nl->node = n;
                                    int last_index = nl->index;
                                    HTMLBranch_list* new_node = new HTMLBranch_list;
                                    nl->next = new_node;
                                    new_node->index = last_index;
                                    nl = new_node;
                                }
                            }
                        }
                    }
                    
                    
                }
                
                
            }
            else
                nl = n->pFirstChild->get_branch(nl, cur_node->next, attrName, attrValue);
            
        }
        n = n->sibling_next;
    }
    
    return nl;
}
/*
 *
 */
bool HTMLBranch::hasAttributeName(wchar_t* attrName)
{
    if (hasAttributes)
    {
        int i = 0;
        vector <ATTRIBUTE>::iterator itorAttr;
        for(itorAttr = attrs.begin(); itorAttr != attrs.end(); itorAttr++)
        {
            //printf("[%s] attribute value removed\n", attrs[i].attr_value);
            if (wcscmp(attrs[i].attr_name, attrName) == 0)
                return true;
            i++;
        }
        return false;
    }else
        return false;
}
/*
 *
 */
wchar_t*	HTMLBranch::getAttribute(wchar_t* attrName)
{
    if (hasAttributes)
    {
        int i = 0;
        vector <ATTRIBUTE>::iterator itorAttr;
        for(itorAttr = attrs.begin(); itorAttr != attrs.end(); itorAttr++)
        {
            //printf("[%s] attribute value removed\n", attrs[i].attr_value);
            if (wcscmp(attrs[i].attr_name, attrName) == 0)
                return attrs[i].attr_value;
            i++;
        }
    }
    return NULL;
}
/*
 *
 *
 */
bool	HTMLBranch::setAttribute(wchar_t* name, wchar_t* value)
{
    if ((name == NULL) || (value == NULL)){
        return false;
    }
    
    size_t n_len = wcslen(name);
    size_t v_len = wcslen(value);
    hasAttributes = true;
    
    if (n_len > ATTRIBUTE_NAME_LEN) return false;
    
    int i = 0;
    vector <ATTRIBUTE>::iterator itorAttr;
    for(itorAttr = attrs.begin(); itorAttr != attrs.end(); itorAttr++)
    {
        //printf("[%s] attribute value removed\n", attrs[i].attr_value);
        if (wcscmp(attrs[i].attr_name, name) == 0){
            
            delete attrs[i].attr_value;
            attrs[i].attr_value = new wchar_t[v_len + 1];
            wmemset(attrs[i].attr_value, 0, v_len+1);
            wcsncpy(attrs[i].attr_value, value, v_len);
            return true;
        }
        i++;
    }
    
    // until here, mean no attrname = name, then create one and append
    attribute abe;
    wmemset(abe.attr_name, 0, ATTRIBUTE_NAME_LEN);
    abe.attr_value = new wchar_t[v_len + 1];
    wmemset(abe.attr_value, 0, v_len+1);
    wcsncpy(abe.attr_name, name, n_len);
    wcsncpy(abe.attr_value, value, v_len);
    attrs.push_back(abe);
    return true;
}

void HTMLBranch::addClickEvent(int type, wchar_t* js)
{
    TAGEVENT te;
    te.event_type = type;
    
    size_t len = wcslen(js);
    te.event_value = new wchar_t[len+1];
    wcsncpy(te.event_value, js, len);
    te.event_value[len] = '\0';
    events.push_back(te);
}
/**
 *
 *
 */
wstring HTMLBranch::innerHTMLStr()
{
    // append child first , then sibling others
    
    wstring s;
    if (isLeaf) {
        return pLeafValue;
    }
    
    // else it is branch
    HTMLBranch* pChilds = pFirstChild;
    while (pChilds) {
        wstring s2 = pChilds->getHTMLString();
        
        s += s2;
        pChilds = pChilds->sibling_next;
        
    }
    
    return s;
}
/**
 *
 */
bool HTMLBranch::set_innerHTML(wchar_t* inStr)
{
    removeALLChilds();
    pFirstChild = NULL;
    parseBranches(inStr, brch_path);
    
    
    if (pFirstChild) {
        return true;
    }else{
        pFirstChild = new HTMLBranch(true, inStr);
        return false;
    }
}

void HTMLBranch::clone(HTMLBranch* srcBrch, bool isDeep)
{
    wcscat(brch_name, srcBrch->brch_name);
    
    pParent = srcBrch->pParent;
    
    if (srcBrch->hasAttributes) {
        hasAttributes = true;
        
    }
    
    if (isDeep) {
        
        if ((srcBrch->nChildBranchNum >0) && (srcBrch->pFirstChild)) {
            nChildBranchNum = srcBrch->nChildBranchNum;
            
            pFirstChild = new HTMLBranch;
            pFirstChild->clone(srcBrch->pFirstChild, isDeep);
            
            HTMLBranch* ne = srcBrch->pFirstChild->sibling_next;
            bool first = true;
            
            HTMLBranch* sib = NULL;
            
            while (ne) {
                HTMLBranch* nn = new HTMLBranch;
                nn->clone(ne, isDeep);
                
                if (first) {
                    pFirstChild->sibling_next = nn;
                    first = false;
                    
                }else{
                    sib->sibling_next = nn;
                    
                }
                
                sib = nn;
                
                ne = ne->sibling_next;
            }
        }
    }
    
}
/*
 *
 */
wstring HTMLBranch::getHTMLString()
{
    
    if (isLeaf) {
        return pLeafValue;
    }
    
    wstring s;
    s += brch_tag_start;
    if (hasAttributes)
    {
        s += L" ";
        int i = 0;
        
        vector <ATTRIBUTE>::iterator itorAttr;
        for(itorAttr = attrs.begin(); itorAttr != attrs.end(); itorAttr++)
        {
            if (i > 0)
            {
                s += L" ";
            }
            s +=(attrs[i].attr_name);
            s += L"=\"";
            s +=(attrs[i].attr_value);
            s += L"\"";
            i++;
        }
    }
    
    // after attributes , now add end tag
    
    // if no leaf and child branches , make it short tag with /> endding
    if (nChildBranchNum == 0)
    {
        if (wcscmp(brch_name, L"script") == 0) {
            s += L">";
        }
        else{
            s += L" />";
        }
        
    }
    else
        s += L">";
    
    // add all child branches
    
    if(nChildBranchNum >0){
        s += pFirstChild->getHTMLString();
        
        HTMLBranch* sib = pFirstChild->sibling_next;
        while (sib != NULL)
        {
            
            //sib->teststr();
            
            s += sib->getHTMLString();
            
            sib = sib->sibling_next;
        }
    }
    
    // if script tag, anyway, add the endTag to it .
    if (wcscmp(brch_name, L"script") == 0){
        s += (brch_tag_end);
    }
    
    else {// if no leaf and child branches , make it short tag with /> endding
        
        if ((nChildBranchNum>0))
            s += (brch_tag_end);
    }
    
    
    
    return s;
}

wstring HTMLBranch::teststr()
{
    printf("test function, tagName=%ls\n", brch_name);
    
    return L"testtest";
}
wstring HTMLBranch::getFormatHTMLString(int deep)
{
    if (this == NULL) return L"";
    
    if (isLeaf) {
        return pLeafValue;
    }
    
    wstring s;
    
    if ((depth ==0) &&(nChildBranchNum>0)){
        return s += (pFirstChild->getFormatHTMLString());
    }
    
    s += L"\r\n";
    int i = 1;
    while (i < depth)
    {
        s += L"  ";
        i++;
    }
    s += (brch_tag_start);
    if (hasAttributes)
    {
        s += L" ";
        
        i = 0;
        
        vector <ATTRIBUTE>::iterator itorAttr;
        for(itorAttr = attrs.begin(); itorAttr != attrs.end(); itorAttr++)
        {
            if (i > 0)
            {
                s += L" ";
            }
            s += attrs[i].attr_name;
            s += L"=\"";
            s += attrs[i].attr_value;
            s += L"\"";
            i++;
        }
    }
    
    
    // if no leaf and child branches , make it short tag with /> endding
    if (nChildBranchNum ==0){
        if (wcscmp(brch_name, L"script") == 0) {
            s += L">";
        }
        else{
            s += L" />";
        }
    }
    else
        s += L">";
    
    
    
    if(nChildBranchNum>0){
        HTMLBranch* nn = pFirstChild;
        while (nn != NULL)
        {
            s += (nn->getFormatHTMLString());
            nn = nn->sibling_next;
        }
        
        // for endTag format in next line
        s += L"\r\n";
        i = 0;
        while (i < depth)
        {
            s += L" ";
            i++;
        }
        
    }
    
    // if script tag, anyway, add the endTag to it .
    if (wcscmp(brch_name, L"script") == 0){
        s += (brch_tag_end);
    }
    
    else {// if no leaf and child branches , make it short tag with /> endding
        
        if ((nChildBranchNum>0))
            s += (brch_tag_end);
    }
    
    //if (depth == 2) wprintf(L"---%ls\n", &s);
    
    return s;
}

/**
 *  remove one child branch
 */
bool HTMLBranch::removeBranch(HTMLBranch* brh)
{
    if (brh) {
        if(brh->sibling_pre != NULL)
        {
            HTMLBranch* pre_b = brh->sibling_pre;
            pre_b->sibling_next = brh->sibling_next;
        }
        else{
            pFirstChild = brh->sibling_next;
            if(pFirstChild == NULL)
                nChildBranchNum-=1;
        }
        
        // free memory
        delete brh;
        
        if (pFirstChild)
            pFirstChild->sibling_pre = NULL;
        return true;
    }
    
    return false;
}

/**
 *      remove all child branches
 */

void HTMLBranch::removeALLChilds()
{
    while (pFirstChild) {
        removeBranch(pFirstChild);
    }
}

HTMLBranch* HTMLBranch::createBranches(wchar_t* path, wchar_t* leafValue, int deep)
{
    PATH_NODES* head = getPathNodes(path);
    
    HTMLBranch* next_node = new HTMLBranch(head, leafValue, this->depth+1);
    
    this->addBranch(next_node);
    
    free_path_nodes(head);
    return next_node;
}

PATH_NODES* HTMLBranch::getPathNodes(wchar_t* path)
{
    if (path == NULL) return NULL;
    wchar_t* p = path;
    
    while ((*p == '/') || (*p == ' ') || (*p == '\t') ) // drop some chars
    {
        p++;
    }
    
    path = p;  // reset the head
    
    wchar_t* step = p;
    
    PATH_NODES* head = NULL;
    PATH_NODES* cur_node = NULL;
    bool headNode = true;
    int i = 0;
    
    // drop blanks and / and tab if there has
    
    while (p != NULL)
    {
        if (*p != '/' && *p != '\0') {
            p++;
            i++;
            continue;
        }
        
        if(headNode)
        {
            head = new PATH_NODES;
            head->path_name = new wchar_t[i + 1];
            wmemset(head->path_name, 0, i+1);
            wcsncpy(head->path_name, step, i);
            step = p + 1;
            headNode = false;
            cur_node = head;
            i = 0;
            if(*p == '\0') break;
        }else{
            if (i == 0) break;
            PATH_NODES* nn = new PATH_NODES;
            nn->path_name = new wchar_t[i + 1];
            wmemset(nn->path_name, 0, i+1);
            wcsncpy(nn->path_name, step, i);
            step = p + 1;
            cur_node->next = nn;
            cur_node = nn;
            i = 0;
            if(*p == '\0') break;
        }
        
        p++;
    }
    
    return head;
}


bool HTMLBranch::free_path_nodes(PATH_NODES* head)
{
    PATH_NODES* c = head;
    PATH_NODES* s;
    while (c != NULL){
        s = c->next;
        delete c;
        c = s;
    }
    head = NULL;
    return true;
}


/*
 end HTMLTree classes
 */

/************************************************************************
 * helper functions
 ************************************************************************/

/* wide char to multi_char Unicode - ANSI*/
char* w2m(const wchar_t* wcs)
{
    if (wcs == NULL) {
        return NULL;
    }
    char* buf = NULL;
    size_t len = wcstombs(NULL, wcs, 0);
    if (len == 0)
        return NULL;
    buf = new char[len + 1];
    if (buf) {
        memset(buf, 0, sizeof(char)*(len + 1));
        wcstombs(buf, wcs, len + 1);
        return buf;
    }
    return NULL;
}

char* w2m2(const wchar_t* wcs, char* buf)
{
    size_t len = wcstombs(NULL, wcs, 0);
    if (len == 0)
        return NULL;
    memset(buf, 0, sizeof(char)*(len + 1));
    wcstombs(buf, wcs, len + 1);
    
    return NULL;
}
/* multi_char to wide_char ANSI - Unicode*/
wchar_t* m2w(const char* mbs)
{
    wchar_t* wbuf;
    size_t len = mbstowcs(NULL, mbs, 0);
    if (len == 0)
        return NULL;
    wbuf = new wchar_t[sizeof(wchar_t)*(len + 1)];
    memset(wbuf, 0, sizeof(wchar_t)*(len + 1));
    len = mbstowcs(wbuf, mbs, len + 1);
    return wbuf;
}
