#include "syshead.h"

#include "buffer.h"
#include "consts.h"
#include "highlight.h"
#include "terminal.h"

int count_lines(FILE *fp) {
    int line_count = 0;
    char *line = NULL;
    size_t linecap = 0;

    while (getline(&line, &linecap, fp) != -1) {
        line_count++;
    }

    rewind(fp);
    
    return line_count;
}

void editorOpen(char *filename) {
    free(E.filename);
    E.filename = strdup(filename);

    editorSelectSyntaxHighlight();

    FILE *fp = fopen(filename, "r");
    if (!fp)
        die("fopen");

// pre-calculate line count to find line number max width
    int line_count = count_lines(fp);

    int linenum_w = floor(log10(abs(line_count))) + 2;
    if (E.linenum_w != linenum_w) {
        E.linenum_w = linenum_w;
        E.editcols = E.screencols - E.linenum_w;
    }

    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;
    while ((linelen = getline(&line, &linecap, fp)) != -1) {
        while (linelen > 0 && (line[linelen - 1] == '\n' || line[linelen - 1] == '\r'))
            linelen--;
        editorInsertRow(E.numrows, line, linelen);
    }
    free(line);
    fclose(fp);
    E.dirty = 0;
}

char *editorRowsToString(int *buflen) {
    int totlen = 0;
    int j;

    for (j = 0; j < E.numrows; j++)
        totlen += E.row[j].size + 1; // extra char for newline
    *buflen = totlen;

    char *buf = malloc(totlen);
    char *loc = buf;
    for (j = 0; j < E.numrows; j++) {
        memcpy(loc, E.row[j].chars, E.row[j].size);
        loc += E.row[j].size;
        *loc = '\n';
        loc++;
    }

    return buf;
}

void editorSave() {
    if (E.filename == NULL) {
        E.filename = editorPrompt("Save as: %s", NULL);
        if (E.filename == NULL) {
            editorSetStatusMessage("Save canceled");
            return;
        }
        editorSelectSyntaxHighlight();
    }

    int len;
    char *buf = editorRowsToString(&len);

    int fd = open(E.filename, O_RDWR | O_CREAT, 0644);
    if (fd != -1) {
        if (ftruncate(fd, len) != -1) {
            if (write(fd, buf, len) == len) {
                close(fd);
                free(buf);
                E.dirty = 0;
                editorSetStatusMessage("%d bytes written to disk", len);
                return;
            }
        }
        close(fd);
    }

    free(buf);
    editorSetStatusMessage("Can't save! I/O error: %s", strerror(errno));
}
