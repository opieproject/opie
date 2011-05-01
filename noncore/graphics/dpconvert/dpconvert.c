/***************************************************************************
 *                                                                         *
 *   dpconvert - converts DrawPad drawings to PNG files                    *
 *                                                                         *
 *   Copyright (C) 2009 Paul Eggleton                                      *
 *   Portions Copyright (C) 2002 S. Prud'homme <prudhomme@laposte.net>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <stdio.h>
#include <glib.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

int main(int argc, char **argv) {
    if( argc != 3 ) {
        fprintf(stderr, "usage: dpconvert /path/to/drawpad.xml /path/to/outputdir\n");
        return 1;
    }

    const char *outputPath = argv[2];

    if(!g_file_test(outputPath, G_FILE_TEST_IS_DIR)) {
        fprintf(stderr, "output directory %s does not exist or not a directory\n", outputPath);
        return 4;
    }

    xmlDoc *doc = xmlParseFile(argv[1]);
    if (!doc) {
        fprintf(stderr, "Unable to parse XML file %s", argv[1]);
        return 3;
    }

    /* Get the root node of the input document */
    xmlNode *root = xmlDocGetRootElement(doc);
    if (!root) {
        fprintf(stderr, "Unable to get xml root element");
        xmlFreeDoc(doc);
        return 3;
    }

    xmlNode *node = root->children;
    if(node) {
        node = root->children;
        while(node && strcmp(node->name, "images") != 0)
            node = node->next;

        if(node)
            node = node->children;
        
        while(node) {
            int untitledidx = 0;
            if(node->type == XML_ELEMENT_NODE && strcmp(node->name, "image") == 0) {
                gchar *title = NULL;
                gchar *data = NULL;
                int length = 0;
                xmlNode *childnode = node->children;
                while(childnode) {
                    if(childnode->type == XML_ELEMENT_NODE) {
                        if(strcmp(childnode->name, "title") == 0) {
                            xmlChar *value = xmlNodeGetContent(childnode);
                            if(value) {
                                if(strlen(value) > 0)
                                    title = g_strdup(value);
                                xmlFree(value);
                            }
                        }
                        else if(strcmp(childnode->name, "data") == 0) {
                            char *value = xmlGetProp(childnode, "length");
                            if(value) {
                                length = atoi(value);
                                xmlFree(value);
                            }

                            if(length > 0) {
                                xmlChar *inpdata = xmlNodeGetContent(childnode);
                                if(inpdata) {
                                    int i;
                                    int inplen;
                                    data = g_malloc(length+1);
                                    inplen = strlen(inpdata);
                                    for (i=0; i < inplen/2; i++) {
                                        char h = inpdata[2 * i];
                                        char l = inpdata[2 * i  + 1];
                                        unsigned char r = 0;

                                        if (h <= '9') {
                                            r += h - '0';
                                        } else {
                                            r += h - 'a' + 10;
                                        }

                                        r = r << 4;

                                        if (l <= '9') {
                                            r += l - '0';
                                        } else {
                                            r += l - 'a' + 10;
                                        }

                                        data[i] = r;
                                    }
                                    xmlFree(inpdata);
                                }
                            }
                        }
                    }
                    childnode = childnode->next;
                }

                if(!title) {
                    untitledidx++;
                    title = g_strdup_printf("untitled%d", untitledidx);
                }

                gchar *filename = g_strdup_printf("%s.png", title);
                gchar *path = g_build_filename(outputPath, filename, NULL);

                printf("writing %s\n", filename);
                
                FILE *f = fopen(path, "w+");
                if(f) {
                    fwrite(data, length, 1, f);
                    if(ferror(f)) {
                        perror("write failed");
                        fclose(f);
                        xmlFreeDoc(doc);
                        return 5;
                    }
                    fclose(f);
                }
                else {
                    perror("open failed");
                    xmlFreeDoc(doc);
                    return 5;
                }

                g_free(filename);
                g_free(path);

                g_free(title);
                if(data)
                    g_free(data);
                
            }
            node = node->next;
        }
    }
    
    xmlFreeDoc(doc);

    return 0;
}
