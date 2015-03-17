#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAXSIZE 512

void translate(const char old[], const char new[]);
void truncate(char old[], char new[]);
void handle_escape (char s[]);
int handle_ranges (const char s[], char t[]);
void delete (const char s[]);

/*
 * Purpose: Takes in the command line arguments and runs the
 * 			appropriate functions to translate or delete 
 *			characters from the standard input.
 * Return:  int 0 if successfully run, 1 if error.
 * Author:  Jason Kolenosky A00911464
 * E-mail:  JasonK58@gmail.com
 * Version: 1.0
 */
int main(int argc, char *argv[]) {

	char old[MAXSIZE], new[MAXSIZE], oldRange[MAXSIZE], newRange[MAXSIZE];
	char t[] = "-t";
	char d[] = "-d";
	
	if (argc < 3 || argc > 4) {
		printf("usage: %s [-t | -d] <arg1> <arg2>\n", argv[0]);
		printf("\targ1: old letters to replace\n");
		printf("\targ2: new letters to replace old with\n");
		printf("\t-t: truncate argument strings\n");
		printf("\t-d: delete specified character(s)\n");
		return 1;
	}
	else if (strcmp(argv[1], t) == 0) {
		strcpy(old, argv[2]);
		strcpy(new, argv[3]);
		handle_escape(old);
		handle_escape(new);
		handle_ranges(old, oldRange);
		handle_ranges(new, newRange);
		truncate(oldRange, newRange);
		translate(oldRange, newRange);
	}
	else if (strcmp(argv[1], d) == 0) {
		strcpy(new, argv[2]);
		handle_escape(new);
		handle_ranges(new, newRange);
		delete(newRange);
	}
	else if (argc == 3) {
		strcpy(old, argv[1]);
		strcpy(new, argv[2]);
		handle_escape(old);
		handle_escape(new);
		handle_ranges(old, oldRange);
		handle_ranges(new, newRange);
		translate(oldRange, newRange);
	}
	
	return 0;
}

/*
 * Purpose: Translates characters. If the first argument is
 * 			longer than the second, the second will be extended by
 * 			the final letter to match the length.
 * Author:  Jason Kolenosky A00911464
 * E-mail:  JasonK58@gmail.com
 * Version: 1.0
 */
void translate(const char old[], const char new[]) { 
	int c;
	size_t i, size;
	char temp[MAXSIZE], extra;

	if (strlen(old) > strlen(new)) {
		size = strlen(old) - strlen(new);
		extra = new[strlen(new) - 1];
		strcpy(temp, new);
		for (i = 0; i < size; i++) {
			temp[strlen(new) + i] = extra;
		}
		temp[strlen(temp)] = '\0';

		while ((c = getchar()) != EOF) { 
			for (i = strlen(old); i != 0; i--) {
				if(c == old[i - 1]) 
					c = temp[i - 1];
			}
			putchar(c);
		}
	}
	
	while ((c = getchar()) != EOF) { 
		for (i = strlen(old); i != 0; i--) {
			if(c == old[i - 1]) 
				c = new[i - 1];
		}
		putchar(c);
	}
}

/*
 * Purpose: If the truncate (-t) switch is used, this functions
 * 			will  shorten the longer argument to match the size 
 *			of the shorter one.
 * Author:  Jason Kolenosky A00911464
 * E-mail:  JasonK58@gmail.com
 * Version: 1.0
 */
void truncate(char old[], char new[]) {

	if (strlen(old) > strlen(new)) {
		old[strlen(new)] = '\0';
	}
}

/*
 * Purpose: Will convert the backslash (\) character and its 
 * 			corresponding escape character match to a single 
 *			escape character.
 * Author:  Jason Kolenosky A00911464
 * E-mail:  JasonK58@gmail.com
 * Version: 1.0
 */
void handle_escape(char s[]) { 
	size_t i, j;

	for (i = 0, j = 0; s[i] != '\0'; i++, j++) {
		if (s[i] == '\\') {
			switch(s[i+1]) {
				case '\\':
					s[j] = '\\';
					break;
				case 'a':
					s[j] = '\a';
					break;
				case 'b':
					s[j] = '\b';
					break;
				case 'f':
					s[j] = '\f';
					break;	
				case 'n':
					s[j] = '\n';
					break;
				case 'r':
					s[j] = '\r';
					break;	
				case 't':
					s[j] = '\t';
					break;	
				case 'v':
					s[j] = '\v';
					break;	
				case '\'':
					s[j] = '\'';
					break;	
				case '\"':
					s[j] = '\"';
					break;
			}
			i++;
		}
		else {
			s[j] = s[i];
		}
	}
	s[j] = '\0';
}

/*
 * Purpose: Handles ranges of characters using the dash (-) character.
 * Return:  int 0 if successfully run, 1 if error.
 * Author:  Jason Kolenosky A00911464
 * E-mail:  JasonK58@gmail.com
 * Version: 1.0
 */
int handle_ranges(const char s[], char t[]) { 
	size_t i, j = 0;
	char c;
	int first = 0;

	for (i = 0; s[i] != '\0'; i++) {
		if (first && s[i] == '-' && s[i-1] != '-' && s[i+1] != '\0') {
			if ((s[i-1] > s[i+1]) || (s[i] == '-' && s[i+1] == '-')) {
				goto ERROR;
			}
			for (c = s[i-1]+1; c < s[i+1]; c++) {
				t[j++] = c;
			}
			first = 0;
			t[j++]=s[++i];
		} else {
			t[j++] = s[i];
			first = 1;
		}
	}
	t[j] = '\0';
	return 0;
	
	ERROR:
		fprintf(stderr, "ERROR: Illegal range");
	return 1;
}

/*
 * Purpose: When the delete switch (-d) is used on the command line,
 * 			will remove specified characters from the standard input
 *			to the standard output.
 * Author:  Jason Kolenosky A00911464
 * E-mail:  JasonK58@gmail.com
 * Version: 1.0
 */
void delete(const char s[]) {
	size_t i;
	int c;
	int check = 0;
	
	while ((c = getchar()) != EOF) {
		for (i = 0; s[i] != '\0'; i++){
			if (c == s[i]) {
				check = 1;
			}
		}
		if (check == 0) {
			putchar(c);
		}
		check = 0;
	}
}