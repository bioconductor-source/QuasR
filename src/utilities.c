#include "utilities.h"

/*! @function
 @abstract  check if bam1_t *hit is a spliced alignment (gap in read >= MIN_INTRON_LENGTH).
 @param     hit     the alignment
 @return    true (1) or false (0)
 */
int _isSpliced(const bam1_t *hit){
    uint32_t *cigar = bam1_cigar(hit);
    int i = 0; // current position of cigar operations
    int l; // length of current cigar operation
    int op; // current cigar operation type

    // loop over cigar operations
    for (i = 0; i < hit->core.n_cigar; ++i) {

        l = cigar[i]>>4, op = cigar[i]&0xf;

        if (op == BAM_CREF_SKIP || op == BAM_CDEL) {
            // read skips reference region -> spliced alignment
            if(l >= MIN_INTRON_LENGTH)
                return 1;
        }
    }

    return 0;
}



// reverse string
// original function copy from utilities.c of Rsamtools
void _reverse(char *buf, int len){
    char tmp;
    for (int i = 0; i < floor(len / 2); ++i) {
        tmp = buf[len - i - 1];
        buf[len - i - 1] = buf[i];
        buf[i] = tmp;
    }
}

// complement sequence string
// original function copy from utilities.c of Rsamtools
void _complement(char *buf, int len){
    static const int MAX_MAP = 256;
    static char map[256];
    static int init = 0;
    if (init == 0) {
        init = 1;
        for (int i = 0; i < MAX_MAP; ++i)
            map[i] = (char) i;
        map['A'] = 'T';
        map['C'] = 'G';
        map['G'] = 'C';
        map['T'] = 'A';
        map['a'] = 't';
        map['c'] = 'g';
        map['g'] = 'c';
        map['t'] = 'a';
        map['M'] = 'K';
        map['R'] = 'Y';
        map['Y'] = 'R';
        map['K'] = 'M';
        map['m'] = 'k';
        map['r'] = 'y';
        map['y'] = 'r';
        map['k'] = 'm';
        map['V'] = 'B';
        map['H'] = 'D';
        map['D'] = 'H';
        map['B'] = 'V';
        map['v'] = 'b';
        map['h'] = 'd';
        map['d'] = 'h';
        map['b'] = 'v';
    }
    for (int i = 0; i < len; ++i)
        buf[i] = map[(int) buf[i]];
}

// open bamfile
samfile_t * _bam_tryopen(const char *filename, const char *filemode, void *aux)
{
    samfile_t *sfile = samopen(filename, filemode, aux);
    if (sfile == 0)
        Rf_error("failed to open SAM/BAM file\n  file: '%s'",
                 filename);
    if (sfile->header == 0 || sfile->header->n_targets == 0) {
        samclose(sfile);
        Rf_error("SAM/BAM header missing or empty\n  file: '%s'",
		 filename);
    }
    return sfile;
}

/*! @function
  @abstract  Get the pointer of a named list element.
  @param  list  List/data.fram object
  @param  str   Name of the list element
  @return       Pointer to the element
 */
SEXP _getListElement(SEXP list, const char *str)
{
    SEXP elmt = R_NilValue;
    SEXP names = getAttrib(list, R_NamesSymbol);

    for (R_len_t i = 0; i < Rf_length(list); i++)
	if(strcmp(CHAR(STRING_ELT(names, i)), str) == 0) {
	    elmt = VECTOR_ELT(list, i);
	    break;
	}
    return elmt;
}
