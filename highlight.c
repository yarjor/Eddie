#include "syshead.h"

#include "highlight.h"
#include "consts.h"
#include "filetype.h"
#include "structs.h"

/**
 * @brief Checks whether character c is a separator character.
 *        Separator char is a space, null-byte, or one of ,.()+/-*=~%<>[];:
 * 
 * @param c (checked character)
 * @return int (boolean value - true if c is a separator)
 */
static int is_separator(int c) {
    return isspace(c) || c == '\0' || strchr(",.()+-/*=~%<>[];:", c) != NULL;
}

void editorUpdateSyntaxBackground(erow *row) {
    // for now, only set background as normal.
    row->bg = realloc(row->bg, row->rsize);
    memset(row->bg, BG_NORMAL, row->rsize);
}

void editorUpdateSyntaxForeground(eState *state, erow *row) {
    row->hl = realloc(row->hl, row->rsize);
    memset(row->hl, HL_NORMAL, row->rsize); // reset everything to normal

    if (state->syntax == NULL)
        return;

    char **keywords = state->syntax->keywords;

    char *scs = state->syntax->singleline_comment_start;
    char *mcs = state->syntax->multiline_comment_start;
    char *mce = state->syntax->multiline_comment_end;

    int scs_len = scs ? strlen(scs) : 0;
    int mcs_len = mcs ? strlen(mcs) : 0;
    int mce_len = mce ? strlen(mce) : 0;

    int prev_sep = 1; // start of row is considered a seperator
    int in_string = 0;
    int lt_start = 0; // start location is saved to allow for non-closed lt (like in comparisons)
    int in_hashtag = 0;
    int in_comment = (row->idx > 0 && state->row[row->idx - 1].hl_open_comment);

    int i = 0;
    while (i < row->rsize) {
        char c = row->render[i];
        unsigned char prev_hl = (i > 0) ? row->hl[i - 1] : HL_NORMAL;

        if (scs_len && !in_string && !in_comment) {
            if (!strncmp(&row->render[i], scs, scs_len)) {
                memset(&row->hl[i], HL_COMMENT, row->rsize - i);
                break; // single-line comment spans across the entire row - no need for further calculations
            }
        }

        if (mcs_len && mce_len && !in_string) {
            if (in_comment) {
                row->hl[i] = HL_MLCOMMENT;
                if (!strncmp(&row->render[i], mce, mce_len)) {
                    memset(&row->hl[i], HL_MLCOMMENT, mce_len); // set the end of comment characters to comment before continuing
                    i += mce_len;
                    in_comment = 0;
                    prev_sep = 1;
                    continue;
                } else {
                    i++;
                    continue;
                }
            } else if (!strncmp(&row->render[i], mcs, mcs_len)) {
                memset(&row->hl[i], HL_MLCOMMENT, mcs_len);
                i += mcs_len;
                in_comment = 1;
                continue;
            }
        }

        if (state->syntax->flags & HL_HIGHLIGHT_LTGT) {
            if (lt_start && c == '>') { // only set ltgt after finding the closing gt
                memset(&row->hl[lt_start - 1], HL_LTGT, i - lt_start + 2);
                lt_start = 0;
                prev_sep = 1;
                i++;
                continue;
            } else if (c == '<') {
                lt_start = i + 1;
            }
        }

        if (state->syntax->flags & HL_HIGHLIGHT_STRINGS) {
            if (in_string) {
                row->hl[i] = HL_STRING;
                if (c == '\\' && i + 1 < row->rsize) {
                    row->hl[i + 1] = HL_STRING;
                    i += 2;
                    continue;
                }
                if (c == in_string)
                    in_string = 0;
                i++;
                prev_sep = 1;
                continue;
            } else {
                if (c == '"' || c == '\'') {
                    in_string = c;
                    row->hl[i] = HL_STRING;
                    i++;
                    continue;
                }
            }
        }

        if (state->syntax->flags & HL_HIGHLIGHT_HASHTAG) {
            if (in_hashtag) {
                row->hl[i] = HL_HASHTAG;
                i++;
                continue;
            } else if (!strncmp(&row->render[i], HASHTAG, 1)) {
                in_hashtag = 1;
                row->hl[i] = HL_HASHTAG;
                i++;
                continue;
            }
        }

        if (state->syntax->flags & HL_HIGHLIGHT_NUMBERS) {
            if ((isdigit(c) && (prev_sep || prev_hl == HL_NUMBER)) || (c == '.' && prev_hl == HL_NUMBER)) {
                row->hl[i] = HL_NUMBER;
                i++;
                prev_sep = 0;
                continue;
            }
        }

        if (prev_sep) { // new word was started, check for keywords
            int j;
            for (j = 0; keywords[j]; j++) {
                int klen = strlen(keywords[j]);
                int kw2 = keywords[j][klen - 1] == '|'; // type 2 mark
                int kw3 = keywords[j][klen - 1] == '`'; // type 3 mark
                if (kw2 || kw3)
                    klen--; // "remove" mark from end of keyword for comparing

                if (!strncmp(&row->render[i], keywords[j], klen) &&
                    is_separator(row->render[i + klen])) {
                    memset(&row->hl[i], kw2 ? HL_KEYWORD2 : (kw3 ? HL_KEYWORD3 : HL_KEYWORD1), klen);
                    i += klen;
                    break;
                }
            }
            if (keywords[j] != NULL) {
                prev_sep = 0;
                continue;
            }
        }

        prev_sep = is_separator(c);
        i++;
    }

    int changed = (row->hl_open_comment != in_comment); // marks change that affects next row
    row->hl_open_comment = in_comment;
    if (changed && row->idx + 1 < state->numrows)
        editorUpdateSyntax(state, &state->row[row->idx + 1]);
}

void editorUpdateSyntax(eState *state, erow *row) {
    editorUpdateSyntaxBackground(row);
    editorUpdateSyntaxForeground(state, row);
}

int editorSyntaxToColor(int hl) {
    switch (hl) {
    case HL_COMMENT:
    case HL_MLCOMMENT:
        return STYLE_BLUE_FG;
    case HL_KEYWORD1:
        return STYLE_YELLOW_FG;
    case HL_KEYWORD2:
        return STYLE_GREEN_FG;
    case HL_KEYWORD3:
        return STYLE_CYAN_FG;
    case HL_HASHTAG:
        return STYLE_MAGENTA_FG;
    case HL_LTGT:
        return STYLE_RED_FG;
    case HL_STRING:
        return STYLE_RED_FG;
    case HL_NUMBER:
        return STYLE_BRIGHT_RED_FG;
    case BG_NORMAL:
        return STYLE_DEFAULT_BG;
    case BG_MATCH:
        return STYLE_BRIGHT_BLACK_BG;
    default:
        return STYLE_WHITE_FG;
    }
}

void editorSelectSyntaxHighlight(eState *state) {
    state->syntax = NULL;
    if (state->filename == NULL)
        return;

    char *ext = strrchr(state->filename, '.');

    for (unsigned int j = 0; j < HLDB_ENTRIES; j++) {
        struct editorSyntax *s = &HLDB[j];
        unsigned int i = 0;
        while (s->filematch[i]) {
            int is_ext = (s->filematch[i][0] == '.');
            if ((is_ext && ext && !strcmp(ext, s->filematch[i])) || (!is_ext && strstr(state->filename, s->filematch[i]))) {
                state->syntax = s;

                int filerow;
                for (filerow = 0; filerow < state->numrows; filerow++) {
                    editorUpdateSyntax(state, &state->row[filerow]);
                }

                return;
            }
            i++;
        }
    }
}