#include <stdio.h>
#include <string.h>

char *rtrim(char *str){
  	int n = 0;
	n = strlen(str);
    for( ;n>=0; n--){
        if(str[n] == ' ' || str[n] == 0 || str[n] == -1 || str[n] == '\t'
			|| str[n] == '\r' || str[n] == '\n')
		{
			str[n] = '\0';
        }else{
            break;
        }
    }
	return str;
}

char *ltrim(char *str){
  	int n, i;
	n = strlen(str);
    for( i=0; i < n; i++){
        if(str[i] == ' ' || str[i] == 0 || str[i] == -1 || str[i] == '\t'
			|| str[i] == '\r' || str[i] == '\n')
		{
			
        }else{
            break;
        }
    }
	if(i > 0)
		strcpy(str, str+i);
	return str;
}


char *Trim(char *ptr)
{
  int l, i;
  l = strlen(ptr);
  while(ptr[l-1] == ' ' && l > 0) l--;
  ptr[l] = 0;
  i = 0;
  while(ptr[i] == ' ' && i < l) i++;
  strcpy(ptr, ptr + i);
  return ptr;
}



void sreplace(char *src, char *dest, const char *s1, const char *s2){
	int l1, l2;
	char *tmp, *sptr;
	l1 = strlen(s1);
	l2 = strlen(s2);

	if(l1 == 0){
		strcpy(dest, src);
		return;
	}

	sptr = src;
	dest[0] = 0;
	while( (tmp = strstr(sptr, s1) ) > 0){
		strncat(dest, sptr, tmp-sptr);
		strcat(dest, s2);
		sptr = tmp + l1;
	}
	strcat(dest, sptr);
}

/*字符串分段提取 以 chip 作为分隔符， 将 src 中的第 fld 段放在 dst 中*/
int substring(char *dst, char *src, char chip, int fld){
	char *ptr, *ptr1;
	int index;
	ptr1 = ptr = src;
	while(fld > 1 && (ptr1 = strchr(ptr, chip)) > 0){
		ptr = ptr1 + 1;
		fld --;
	}

	if(fld > 1)
		return 0;
	ptr1 = strchr(ptr, chip);
	if(ptr1 <= 0){
		strcpy(dst, ptr);
	}else{
		strncpy(dst, ptr, ptr1-ptr);
		dst[ptr1-ptr] = '\0';
	}
	return 1;
}



/*
 - split - divide a string into fields, like awk split()
 = int split(char *string, char *fields[], int nfields, char *sep);
 */
int                             /* number of fields, including overflow */
split(
	char *string,
	char *fields[],     /* list is not NULL-terminated */
	int  nfields,          /* number of entries available in fields[] */
	char *sep            /* "" white, "c" single char, "ab" [ab]+ */
)
{
	register char *p = string;
	register char c;                        /* latest character */
	register char sepc = sep[0];
	register char sepc2;
	register int fn;
	register char **fp = fields;
	register char *sepp;
	register int trimtrail;

	/* white space */
	if (sepc == '\0') {
		while ((c = *p++) == ' ' || c == '\t')
				continue;
		p--;
		trimtrail = 1;
		sep = " \t";    /* note, code below knows this is 2 long */
		sepc = ' ';
	} else
		trimtrail = 0;
	sepc2 = sep[1];         /* now we can safely pick this up */

	/* catch empties */
	if (*p == '\0')
		return(0);
	/* single separator */

	if (sepc2 == '\0') {
		fn = nfields;
		for (;;) {
			*fp++ = p;
			fn--;
			if (fn == 0)
				break;
			while ((c = *p++) != sepc)
				if (c == '\0')
					return(nfields - fn);
			*(p-1) = '\0';
		}

		/* we have overflowed the fields vector -- just count them */
		fn = nfields;
		for (;;) {
			while ((c = *p++) != sepc)
				if (c == '\0')
					return(fn);
			fn++;
		}
		/* not reached */
	}

	/* two separators */
	if (sep[2] == '\0') {
		fn = nfields;
		for (;;) {
			*fp++ = p;
			fn--;
			while ((c = *p++) != sepc && c != sepc2)
				if (c == '\0') {
					if (trimtrail && **(fp-1) == '\0')
						fn++;
					return(nfields - fn);
				}
			if (fn == 0)
					break;
			*(p-1) = '\0';
			while ((c = *p++) == sepc || c == sepc2)
					continue;
			p--;
		}

		/* we have overflowed the fields vector -- just count them */
		fn = nfields;
		while (c != '\0') {
			while ((c = *p++) == sepc || c == sepc2)
				continue;
			p--;
			fn++;
			while ((c = *p++) != '\0' && c != sepc && c != sepc2)
				continue;
		}
		/* might have to trim trailing white space */
		if (trimtrail) {
			p--;
			while ((c = *--p) == sepc || c == sepc2)
				continue;
			p++;
			if (*p != '\0') {
				if (fn == nfields+1)
					*p = '\0';
				fn--;
			}
		}
		return(fn);
	}

	/* n separators */
	fn = 0;
	for (;;) {
		if (fn < nfields)
			*fp++ = p;
		fn++;
		for (;;) {
			c = *p++;
			if (c == '\0')
				return(fn);
			sepp = sep;
			while ((sepc = *sepp++) != '\0' && sepc != c)
				continue;
			if (sepc != '\0')       /* it was a separator */
				break;
		}
		if (fn < nfields)
				*(p-1) = '\0';
		for (;;) {
			c = *p++;
			sepp = sep;
			while ((sepc = *sepp++) != '\0' && sepc != c)
				continue;
			if (sepc == '\0')       /* it wasn't a separator */
				break;
		}
		p--;
	}
	/* not reached */
}

/*case-insensitive version of strstr*/
char *strcasestr(const char *str, const char *sub)
{
	int l1, l2, i, j;
	
	l1 = strlen(str);
	l2 = strlen(sub);
	for(i=0, j=0; i<l1; i++){
		if(toupper(str[i]) == toupper(sub[j]))
			j++;
		else
			j = 0;
		if(j == l2)
			return (char *)(str + i - j + 1);
	}
	return (char *)NULL;
}

