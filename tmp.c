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

#ifdef T3_PROB_IS_FLOAT
typedef float prob_t;
#define MAXPROB MAXFLOAT
#else
typedef double prob_t;
#define MAXPROB MAXDOUBLE
#endif

typedef struct word_s
{
  char *string;      /* grapheme */
  size_t count;      /* total number of occurences */
  int *tagcount;     /* maps tag index -> no. of occurences */
  prob_t *lp;        /* probability/log probability (t3)*/
  ptrdiff_t defaulttag;    /* most frequent tag index (tbt,et)*/
  char *aclass;      /* ambiguity class (et) */
} word_t;
typedef word_t *word_pt;

/* ------------------------------------------------------------ */
static word_pt new_word(char *s, size_t cnt, size_t not)
{
  word_pt w=(word_pt)mem_malloc(sizeof(word_t));
  size_t i;

  w->string=s;
  w->count=cnt;
  w->tagcount=(int *)mem_malloc(not*sizeof(int));
  memset(w->tagcount, 0, not*sizeof(int));
  w->lp=(prob_t *)mem_malloc(not*sizeof(prob_t));
  for (i=0; i<not; i++) { w->lp[i]=-MAXPROB; }
/*   memset(w->lp, 0, not*sizeof(double)); */
  return w;
}

/* ------------------------------------------------------------ */
static void delete_word(word_pt w)
{
  mem_free(w->tagcount);
  mem_free(w->lp);
  mem_free(w);
}

int main(int argc, char**argv) {
	size_t n = 0;
	char *buf = NULL;
	size_t r = 0;
	size_t lno = 0;
	FILE* f = NULL;
	FILE* o = NULL;

	sregister_pt strings = sregister_new(6000);
	iregister_pt tags = iregister_new(32);
	size_t not = iregister_get_length(tags);
	hash_pt dictionary = hash_new(5000, .5, hash_string_hash, hash_string_equal);
	char** tokens = NULL;
	size_t tokens_len = 0;
	size_t tokens_size = 0;
	f = fopen(argv[1], "rb");
	o = fopen(argv[3], "wb");
	while ((r = readline(&buf,&n,f)) != -1)
	{
		lno++;
		tokens_len = 0;
		if( split_string_on_space(&tokens, &tokens_len, &tokens_size, buf) != -1 ) {
			if(tokens_len == 0) {
				report(1, "can't find word in lexicon file at line:%lu)\n", (unsigned long) lno);
			} else {
				ptrdiff_t itag = -1;
				int cnt = 0;
				for(ssize_t i = 1; i < (ssize_t)tokens_len-1; i+=2) {
					itag = iregister_add_name(tags, tokens[i]);
					if(sscanf(tokens[i+1], "%d", &cnt) != 1) {
						//TODO handle errors
					}
					fprintf(o, "%s %s %ld, %d\n", tokens[0], tokens[i], (long) itag, cnt);
				}
				char* rs = (char*)sregister_get(strings,tokens[0]);
				word_pt wd, old;
			}
		}
		//TODO handle errors
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
