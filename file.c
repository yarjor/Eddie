#include "syshead.h"

#include "file.h"
#include "buffer.h"
#include "consts.h"
#include "structs.h"
#include "highlight.h"
#include "terminal.h"

/**
 * @brief Counts the number of lines in the file. The function assumes the pointer is to
 *        beginning of file, and will rewind to the beginning before returning.
 * 
 * @param fp (pointer to the open file. Assumed to point to the start of the file)
 * @return int (number of lines counted)
 */
static int count_lines(FILE *fp) {
    int line_count = 0;
    char *line = NULL;
    size_t linecap = 0;

    while (getline(&line, &linecap, fp) != -1) {
        line_count++;
    }

    rewind(fp);
    
    return line_count;
}

/**
 * @brief Convert an erow array into a single string ready for 
 *        writing to a file.
 * 
 * @param buflen (pointer to int the function will store the resulting length into)
 * @param numrows (number of rows being converted)
 * @param rows (pointer to the editor row array)
 * @return char* (the converted string)
 */
static char *rows_to_string(int *buflen, int numrows, erow *rows) {
    int totlen = 0;
    int j;

    for (j = 0; j < numrows; j++)
        totlen += rows[j].size + 1; // extra char for newline
    *buflen = totlen;

    char *buf = malloc(totlen);
    char *loc = buf;
    for (j = 0; j < numrows; j++) {
        memcpy(loc, rows[j].chars, rows[j].size);
        loc += rows[j].size;
        *loc = '\n';
        loc++;
    }

    return buf;
}

void editorOpen(eState *state, char *filename) {
    free(state->filename);
    state->filename = strdup(filename);

    editorSelectSyntaxHighlight(state);

    FILE *fp = fopen(filename, "r");
    if (!fp)
        die("fopen");

    // pre-calculate line count to find the minimum width for numbering column.
    int line_count = count_lines(fp);

    int linenum_w = floor(log10(abs(line_count))) + 2;
    if (state->linenum_w != linenum_w) {
        state->linenum_w = linenum_w;
        state->editcols = state->screencols - state->linenum_w;
    }

    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;
    while ((linelen = getline(&line, &linecap, fp)) != -1) {
        while (linelen > 0 && (line[linelen - 1] == '\n' || line[linelen - 1] == '\r'))
            linelen--;
        editorInsertRow(state, state->numrows, line, linelen);
    }
    free(line);
    fclose(fp);
    state->dirty = 0;
}

void editorSave(eState *state) {
    if (state->filename == NULL) { // no file was open, prompt the user to save as
        state->filename = editorPrompt(state, "Save as: %s", NULL);
        if (state->filename == NULL) {
            editorSetStatusMessage(state, "Save canceled");
            return;
        }
        editorSelectSyntaxHighlight(state);
    }

    int len;
    char *buf = rows_to_string(&len, state->numrows, state->row);

    int fd = open(state->filename, O_RDWR | O_CREAT, 0644);
    if (fd != -1) {
        if (ftruncate(fd, len) != -1) {
            if (write(fd, buf, len) == len) {
                close(fd);
                free(buf);
                state->dirty = 0;
                editorSetStatusMessage(state, "%d bytes written to disk", len);
                return;
            }
        }
        close(fd);
    }

    free(buf);
    editorSetStatusMessage(state, "Can't save! I/O error: %s", strerror(errno));
}
