
#ifndef __ZHUtil__HTMLTree__
#define __ZHUtil__HTMLTree__

#pragma once
#pragma warning(disable:4996)
#pragma warning(disable:4067)
#ifdef WIN32
#include "stdafx.h"
#endif
#include <vector>
#include <string>
#include <map>
using namespace std;

/*
 define callback function type here. used if parsing procedure.
 init by null , check it if null when use
 */

// call back to tell connect ok or not
/*
 return 1 ,continue parse, 0 will break parsing.
 if break parse, HTMLBranch will be deleted for the dealloc function.
 so , note to copy data out then return 0.
 */
class HTMLBranch;
typedef int(*ptrCallBackFoundTag)(void* sender, HTMLBranch* branch);
typedef int(*ptrCallBackFoundJS)(void* sender, HTMLBranch* brh);
/*
 * some structures need.
 */
#define			ATTRIBUTE_NAME_LEN		17
#define			BRANCH_NAME_LEN			17
// the attributes structure, save value and name, and will be push_back into vector
typedef struct attribute
{
    wchar_t attr_name[ATTRIBUTE_NAME_LEN];
    wchar_t* attr_value;
    attribute():attr_value(NULL)
    {
        //memset(attr_name, 0, ATTRIBUTE_NAME_LEN);
        wmemset(attr_name, 0x0, ATTRIBUTE_NAME_LEN);
    }
}ATTRIBUTE;

enum BRANCH_TYPE{
    TYPE_DOM_BRANCH,
    TYPE_DOM_FREG,
} ;
// the click, mousemove, mouseover actions added on current tag
// event type would be 'click', 'mousemove', etc.
typedef struct tagevent
{
    int event_type;
    wchar_t* event_value;
    tagevent():event_value(NULL)
    {
        
    }
}TAGEVENT;

// a link nodes stucture to get target Branch
// this stucture is for parse the path later use.
typedef struct path_nodes
{
    wchar_t* path_name;	// such as aa
    path_nodes* next;	// pointer to next path_node
    path_nodes():path_name(NULL),next(NULL)
    {
    }
}PATH_NODES;

class HTMLBranch;
class HTMLLeaf;

typedef HTMLBranch*  t_pHTMLBranch;
typedef struct _HTMLBranch_list
{
    HTMLBranch* node;
    int index;
    _HTMLBranch_list* next;
    _HTMLBranch_list():node(NULL), index(0), next(NULL)
    {
        
    }
}HTMLBranch_list;

class HTMLTree
{
public:
    HTMLTree(void);
    virtual ~HTMLTree(void);
    virtual bool addBranch(HTMLBranch* brh) = 0;
    virtual bool removeBranch(HTMLBranch* brh) = 0;
    static bool isUsableChar(wchar_t inc);
    static HTMLBranch* getRoot(wchar_t* inStr, wchar_t* rootTagName=L"html", bool debug=false, wchar_t* basePath = NULL);
    
    
    virtual bool        insertBranchBefore(HTMLBranch* brch, HTMLBranch* before_node) = 0;
    virtual HTMLBranch* parseBranches(wchar_t* inStr, wchar_t* basePath = NULL) = 0;
    virtual wstring     getHTMLString() = 0;
    virtual wstring     getFormatHTMLString(int deep = 0) = 0;
    virtual wstring     innerHTMLStr() = 0;     // return inner html string of current node
    virtual bool        set_innerHTML(wchar_t* inStr) = 0; // parse string into child branch of current node.
    virtual HTMLBranch* getBranchByAttrname(wchar_t* attrName, wchar_t* attrValue = NULL) = 0;
    virtual int         getBranchesByAttr(vector <HTMLBranch*>& list, wchar_t* attrName, wchar_t* attrValue = NULL) = 0;
    virtual void        removeALLChilds() = 0;
    virtual int         getBranchesByTagName(wchar_t* tagName, vector <HTMLBranch*>& list, wchar_t* attrName = NULL, wchar_t* attrV = NULL) = 0;
    
    
public:
    int                 found_branches;
    HTMLBranch*         pFirstChild;
    int                 nChildBranchNum;
    int                 depth; // or \t nums , the root has depth=0
    HTMLBranch_list*    queryList;
    bool                debugMode;
    wchar_t*            dom_versioninfo;
    
};

class HTMLBranch: public HTMLTree
{
public:
    HTMLBranch(void);
    HTMLBranch(wchar_t* path, wchar_t* v = NULL); // create branch by path
    HTMLBranch(PATH_NODES* name, wchar_t* leafValue = NULL, int deep = 0);
    HTMLBranch(bool is_leaf, wchar_t* leafValue = NULL);
    
    void		InitBranch();
    virtual		~HTMLBranch(void);
    
    bool	addBranch(HTMLBranch* brh);
    void    addSiblingAfter(HTMLBranch* target_node, HTMLBranch* br);
    bool    insertBranchBefore(HTMLBranch* brch, HTMLBranch* before_node);
    bool	removeBranch(HTMLBranch* brh);
    void    removeALLChilds();
    HTMLBranch* parseBranches(wchar_t* inStr, wchar_t* basePath = NULL);
    virtual wstring innerHTMLStr();
    bool    set_innerHTML(wchar_t* inStr);
    // return number of got branches, branches saved in inList .
    int                 get_all_branches(wchar_t* path,
                                         HTMLBranch** &inList,
                                         wchar_t* attrName = NULL,
                                         wchar_t* attrValue = NULL);
    int                 free_lists(HTMLBranch** &inList, int size=0);// free the mem of lists which have HTMLBranch* nodes
    
    HTMLBranch_list*    get_branch(HTMLBranch_list* nl = NULL, PATH_NODES* cur_node = NULL, wchar_t* attrName = NULL, wchar_t* attrValue = NULL);
    
    bool                hasAttributeName(wchar_t* attrName); // if it has an attrName
    wchar_t*            getAttribute(wchar_t* attrName);
    bool                setAttribute(wchar_t* name, wchar_t* value);
    wstring             getHTMLString();
    wstring             getFormatHTMLString(int deep = 1);
    bool                free_path_nodes(PATH_NODES* head);
    HTMLBranch*         createBranches(wchar_t* path = NULL, wchar_t* leafValue = NULL, int deep = 0);
    PATH_NODES*         getPathNodes(wchar_t* path);
    //string getLastPath(PATH_NODES* all_nodes, char* whole_path);
    
    /** get branches by given tagName
     *  if recurve , then loop into it for all , and put in array back
     */
    int getBranchesByTagName(wchar_t* tagName, vector <HTMLBranch*>& list, wchar_t* attrName=NULL, wchar_t* attrV=NULL);
    
    virtual HTMLBranch* getBranchByAttrname(wchar_t* attrName, wchar_t* attrValue = NULL);
    int getBranchesByAttr(vector <HTMLBranch*>& list, wchar_t* attrName, wchar_t* attrValue = NULL);
    
    wstring             teststr();
    void                clone(HTMLBranch* srcBrch, bool isDeep = true);
public:
    ptrCallBackFoundTag ptrCallBack_htmlTag;
    ptrCallBackFoundJS ptrCallBack_foundjs;
    
    BRANCH_TYPE    type;    // it is normal branch of dom , or a fragment element ...etc
    
    wchar_t brch_name[BRANCH_NAME_LEN];		// must less than 31 chars .\0 is one
    wchar_t* brch_path; // 'a/b/c/d....'
    vector <ATTRIBUTE> attrs;
    wchar_t brch_tag_start[BRANCH_NAME_LEN + 1]; // '<' added
    wchar_t brch_tag_end[BRANCH_NAME_LEN + 3]; // "</" and ">"
    wchar_t* strBuffer;
    
    int depth; // or \t nums , the root has depth=0
    
    bool hasAttributes;
    int nChildBranchNum;
    
    bool isLeaf;
    bool isFakeBranch;
    
    HTMLBranch* pParent;
    HTMLBranch* pFirstChild;
    HTMLBranch* sibling_pre;
    HTMLBranch* sibling_next;
    //HTMLLeaf* leaf;
    
    wchar_t*    pLeafValue;
    
    // for addEventListener, it is events list
    vector <TAGEVENT> events;
    void addClickEvent(int type, wchar_t* js);
};


/**********************************************
 * helper funcs
 * these funcs put here because it is frequently
 * used with HTMLBranch class.
 *********************************************/

char* w2m(const wchar_t* wcs);
wchar_t* m2w(const char* mbs);
char* w2m2(const wchar_t* wcs, char* buf);

#endif


