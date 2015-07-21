#include "minicfg.h"

minicfg::minicfg(const char* filepath)
{
    lines_list = NULL;
    lines_head = NULL;
    
    if (filepath) {
        // save path
        size_t len = strlen(filepath);
        filefullname = new char[len+1];
        strncpy(filefullname, filepath, len);
        filefullname[len] = '\0';
        
        
        f = fopen(filefullname, "r");
        if (f == NULL) {
            // if not exist , create one
            f = fopen(filefullname, "w+");
            sp_debug("**** create default %s file ok.", filefullname);
            
        }
    }
    
    
    
}

void minicfg::start_parse()
{
    f = fopen(filefullname, "r+");
    int ret = parseFile();
    if (ret == FILE_TOO_LONG) {
        sp_debug("file too long");
    }else if (ret == INVALID_FORMAT){
        sp_debug("format error");
    }else if (ret == LINE_TOO_LONG){
        sp_debug("line too long error");
    }else if (ret == UNKNOWN){
        sp_debug("unknown error");
    }
    
    fclose(f);
}
// init as config file using
void minicfg::init_cfg()
{
    fprintf(f, "#  This is created by default ! cfg file size should not longer than 80k bytes.\n");
    fprintf(f, "#  each line should not longer than 512 bytes !!.\n");
    fprintf(f, "#  the cfg format is  \"key = value \" .\n");
    
    fprintf(f, "server_port = 50731\n");
    fprintf(f, "listen_num = 200\n");
    fprintf(f, "max_connections = 10000\n");
    fprintf(f, "#serv windows means how many connections a serviceMan handle, init to 10 now\n");
    fprintf(f, "serviceNum = 10\n\n");
    
    fprintf(f, "# Mysql settings\n");
    fprintf(f, "dbhost = localhost\n");
    fprintf(f, "dbport = 3306\n");
    fprintf(f, "dbname = grampus\n");
    fprintf(f, "dbusername = grampus\n");
    fprintf(f, "dbuserpass = bmwjeep\n");
    
    fflush(f);
    fclose(f);
    start_parse();
}

// init as dns cache use
void minicfg::init_dns_cache()
{
    fprintf(f, "#  This is created by default ! file size should not longer than 80k bytes.\r\n");
    fprintf(f, "#  each line should not longer than 512 bytes !!.\r\n");
    fprintf(f, "#  the line format is  \"key = value \" .\r\n");
    
    fflush(f);
    fclose(f);
    start_parse();
}

minicfg::~minicfg()
{
    sp_debug("exiting minicfg.....proc=%d\n", (int)getpid());
    
    LinesList* start = lines_head;
    
    while (start != NULL) {
        
        LinesList* nn = start->next;
        
        delete start;
        
        start = nn;
    }
    
    if (f) fclose(f);
}

int minicfg::parseFile()
{
    //memset(contentBuf, 0, CONTENT_BUF_SIZE+1);
    fread(contentBuf, CONTENT_BUF_SIZE, 1, f);
    
    //sp_debug("%s", contentBuf);
    
    char* p = contentBuf;
    while(*p != '\0')
	{
        char* end = strstr(p, LINE_BREAK_CHAR);
        if (end) {
            
            int len = end - p;
            if ( len >= LINE_BUF_SIZE) {
                sp_debug("too long of line chars\n");
                return LINE_TOO_LONG;
            }
            
            // got a line,
            if (len > 0) {
                
                // pass blanks and null line
                char* p2 = p;
                while (*p2 == ' ' || *p2 == '\t') {
                    p2++;
                    len--;
                }
                
                // if still >0, then real
                
                if (len > 0) {
                    
                    if (lines_list == NULL) {
                        lines_list = new LinesList;
                        lines_head = lines_list;
                    }else{
                        LinesList* tmp = lines_list;
                        lines_list = new LinesList;
                        tmp->next = lines_list;
                    }
                    
                    lines_list->len = len;
                    strncpy(lines_list->lineBuf, p2, len);
                    //sp_debug("copied %d\n", len);
                    parseLine(lines_list);
                    p = end;
                    
                }
                
            }
            
        }
		p++;
    }
    return 0;
}

int minicfg::parseLine(LinesList* line)
{
    //sp_debug("->%s\n", line->lineBuf);
    
    char* p = line->lineBuf;
    
    if (*p == '#') {
        line->type = COMMENT;
        return 0;
    }
    else{
        // to see if there are '=' in it
        char* e = strstr(line->lineBuf, "=");
        if (e) {
            line->type = VALID_STR;
            
            *e = '\0'; // end of str
            e++;
            line->key = trim(line->lineBuf);
            line->value = trim(e);
            
        }
        else{
            return -1;
        }
    }
    return 0;
}

char* minicfg::trim(char* inStr)
{
    int len = strlen(inStr);
    char* p = inStr;
    char* p2 = inStr + len-1;
    
    while (*p == ' ') {
        p++;
        len--;
    }
    
    while (*p2 == ' ') {
        p2--;
        len--;
    }
    
    *(p2+1) = '\0';
    return p;
    
}

char* minicfg::getValue(const char* key)
{
    if (key == NULL) 
        return NULL;
    
    LinesList* start = lines_head;
    while (start != NULL) {
        if (start->type == VALID_STR){
            
            if (strcmp(key, start->key) == 0) {
                sp_debug("GOT %s=%s\n", key, start->value);
                return start->value;
            }
            
            //sp_debug("%s++++++%s..", start->key, start->value);
        }
        start = start->next;
    }
    return NULL;
}

bool minicfg::saveValue(const char* key, const char* value)
{
    
    LinesList* start = lines_head;
    LinesList* lastNode = NULL;
    
    while (start != NULL) {
        if (start->type == VALID_STR){
            
            if (strcmp(key, start->key) == 0) {
                //sp_debug("GOT %s=%s\n", key, start->value);
                delete start->value;
                size_t len = strlen(value);
                start->value = new char[len+1];
                strncpy(start->value, value, len);
                start->value[len] = '\0';
                
                return true;
            }
            
            //sp_debug("%s++++++%s..", start->key, start->value);
        }
        lastNode = start;
        start = start->next;
    }
    
    // to here , no match key found , append new one .
    
    size_t line_len = strlen(key) + strlen(value) + 4;
    
    char* aline = new char[line_len+1];
    
    strcat(aline, key);
    strcat(aline, " = ");
    strcat(aline, value);
    strcat(aline, NEXT_LINE_END);
    
    LinesList* lineNode = new LinesList;
    lineNode->len = (int)line_len;
    strncpy(lineNode->lineBuf, aline, line_len);
    delete aline;
    parseLine(lineNode);

    if (lastNode) {
        lastNode->next = lineNode;
    }
    else{
        lines_head = lineNode;
    }
    return true;
}

void minicfg::saveFile()
{
    f = fopen(filefullname, "w+");
    
    LinesList* ss = lines_head;
    char buf[LINE_BUF_SIZE];
    while (ss != NULL) {
        bzero(buf, LINE_BUF_SIZE);
        size_t line_len;
        
        switch (ss->type) {
                
            case COMMENT:
            {
                line_len = ss->len;
                
                strcat(buf, ss->lineBuf);
                strcat(buf, NEXT_LINE_END);
            }
                break;
            case VALID_STR:
            {
                line_len = strlen(ss->key) + strlen(ss->value) + 4;
                
                strcat(buf, ss->key);
                strcat(buf, " = ");
                strcat(buf, ss->value);
                strcat(buf, NEXT_LINE_END);
            }
                break;
            default:
                break;
        }
        
        sp_debug("Line=%s", buf);
        fwrite(buf, line_len, 1, f); fflush(f);
        ss = ss->next;
    }

    fclose(f);
}
