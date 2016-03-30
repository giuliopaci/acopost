#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "util.h"
#include "iregister.h"

static inline const char* skip_white_spaces(const char* str) {
	if(!str) {
		return NULL;
	}
	while(*str && isspace((unsigned char) *str)) {
		str++;
	}
	if(!*str) {
		return NULL;
	}
	return str;
}

static inline size_t non_white_space_length(const char* str) {
	size_t i = 0;
	if(!str) {
		return 0;
	}
	while(str[i] && !isspace((unsigned char) str[i])) {
		i++;
	}
	return i;
}


int string_add(char*** strings, size_t* strings_len, size_t* strings_size, char* token) {
	if(*strings == NULL) {
		char** tmp = (char**) malloc(sizeof(char*)*16); 
		if(tmp == NULL) {				
			return -1;					
		}						
		*strings = tmp;					
		*strings_size = 16;				
		*strings_len = 0;				
	}							
	if(*strings_size < *strings_len + 1 ) {			
		size_t tmp_len = *strings_size * 2;		
		char **tmp = (char**) realloc((*strings), tmp_len*sizeof(char*));
		if(tmp == NULL) {				
			return -1;
		}
		*strings = tmp;					
		*strings_size = tmp_len;			
	}							
	(*strings)[*strings_len] = token;			
	*strings_len = *strings_len + 1;
	return 0;
}

/**
 * This function is destructive for the input string as it will replace
 * several spaces with '\0'.
 */
static inline int split_string_on_space(char*** tokens, size_t* tokens_len, size_t* tokens_size, char* line) {
	if(line) {
		while(*line) {
			while(*line && isspace((unsigned char) *line)) {
				line++;
			}
			size_t i = 0;
			while(line[i] && !isspace((unsigned char) line[i])) {
				i++;
			}
			if(i!=0) {
				if(line[i]) {
					line[i] = 0;
					i++;
				}
				if( string_add(tokens, tokens_len, tokens_size, line) == -1) {
					return -1;
				}
				line += i;
			}
		}
	}
	return 0;
}

int main(int argc, char**argv) {
	size_t n = 0;
	char *buf = NULL;
	size_t r = 0;
	FILE* f = NULL;
	FILE* o = NULL;

	iregister_pt tags = iregister_new(32);
	char** tokens = NULL;
	size_t tokens_len = 0;
	size_t tokens_size = 0;
	f = fopen(argv[1], "rb");
	o = fopen(argv[3], "wb");
	while ((r = readline(&buf,&n,f)) != -1)
	{
		tokens_len = 0;
		if( split_string_on_space(&tokens, &tokens_len, &tokens_size, buf) != -1 ) {
			ptrdiff_t itag = -1;
			int cnt = 0;
			for(ssize_t i = 1; i < (ssize_t)tokens_len-1; i+=2) {
				itag = iregister_add_name(tags, tokens[i]);
				if(sscanf(tokens[i+1], "%d", &cnt) != 1) {
					//TODO handle errors
				}
				fprintf(o, "%s %s %ld, %d\n", tokens[0], tokens[i], (long) itag, cnt);
			}
		}
	}
	fclose(o);
	fclose(f);
	iregister_delete(tags);
	if(buf != NULL) {
		free(buf);
		buf = NULL;
	}
	if(tokens != NULL) {
		free(tokens);
		tokens = NULL;
	}
	
	return 0;
}
