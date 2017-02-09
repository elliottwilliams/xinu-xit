#include <xinu.h>
#include <ctype.h>
#define LINE 16

// Append to `buf` with sprintf semantics, and advance `buf` by the number of
// characters added.
#define saprintf(buf, fmt, ...) (buf) += sprintf((buf), (fmt), ##__VA_ARGS__)

// Xinu doesn't globally define a `min` macro
#define min(a, b) ((a) > (b) ? (b) : (a))

#define COL_INVERT "\x1b[7m"
#define COL_RESET  "\x1b[0m"
#define COL_INVERT_LEN strlen(COL_INVERT)
#define COL_RESET_LEN strlen(COL_RESET)

// Add `n` spaces to `buf` and return the number of spaces added.
static inline int pad(char * buf, int n) {
  for (int i = 0; i < n; i++) {
    saprintf(buf, " ");
    buf += 1;
  }
  return n;
}

int hexcmp_buflen(int size, char * padstr) {
  int padlen = strlen(padstr);
  return sizeof(char) * (20 * size +
                         ((4+padlen) * (size/LINE)));
}

void hexcmp(char * out, const char * va, const char * vb, int size,
            char * padstr)
{
  if (padstr == NULL)
    padstr = "";
  int padlen = strlen(padstr);

  // each byte: 3 chars for hex+space, 1 char for ascii rep, 16 for maximal
  //            color changes
  // each line: 4 bytes for separators and padding, and sizeof(padstr)
  char * buf = out;

  int i, r = 0;
  while (r < size) {
    // Print the line padding if not first line.
    if (r != 0) {
      sprintf(buf, "%s", r ? padstr : "");
      buf += padlen;
    }

    // Calculate how many characters to print in this line.
    int line = min(LINE, size - r);

    // Print bytes in hex...
    for (i = 0; i < line; i++) {
      // ...inverting if the byte is different between `va` and `vb`...
      if (va[r] != vb[r]) {
        sprintf(buf, COL_INVERT "%02x " COL_RESET, 0xff & va[r++]);
        buf += 3 + COL_INVERT_LEN + COL_RESET_LEN;
      } else {
        sprintf(buf, "%02x ", 0xff & va[r++]);
        buf += 3;
      }

      // ...and adding an extra space between words.
      if ((i != 0) && (i % 8 == 0)) {
        sprintf(buf, " ");
        buf += 1;
      }
    }

    // If `line` is not a complete line, add spaces to align the ascii column.
    buf += pad(buf, 3*(LINE-line) + (LINE-line)/8);

    // Rewind to print ascii representation.
    r -= line;

    // Print bytes in ascii...
    sprintf(buf, "\t|");
    buf += 2;

    // ...using their printable characters, or '.' if unprintable...
    for (i = 0; i < line; i++) {
      // ...inverting if the byte is different between `va` and `vb`...
      if (va[r] != vb[r]) {
        sprintf(buf, COL_INVERT "%c" COL_RESET, 
                isprint(va[r++]) ? va[r-1] : '.');
        buf += 1 + COL_INVERT_LEN + COL_RESET_LEN;
      } else {
        sprintf(buf, "%c",
                isprint(va[r++]) ? va[r-1] : '.');
        buf += 1;
      }
    }

    // ...with extra padding if the line is not unit length.
    buf += pad(buf, LINE-line);
    sprintf(buf, "|\n");
    buf += 2;
  }
}
