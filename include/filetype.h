#ifndef FILETYPE_H
#define FILETYPE_H

#include "consts.h"
#include "highlight.h"

/*** C consts ***/
char *C_HL_EXTENSIONS[] = { ".c", ".h", ".cpp", NULL };
char *C_HL_KEYWORDS[] = {
    // Keywords
    "switch", "if", "while", "for", "break", "continue", "return", "else",
    "struct", "union", "typedef", "static", "enum", "class", "case",

    // Types
    "int|", "long|", "double|", "float|", "char|", "unsinged|", "signed|",
    "void|",
    NULL
};

/*** Python consts ***/
char *PYTHON_HL_EXTENSIONS[] = {".py", ".pyw", NULL};
char *PYTHON_HL_KEYWORDS[] = {
    // Keywords
    "import", "if", "while", "for", "break", "continue", "return", "else",
    "elif", "True", "as", "assert", "del", "exec", "global", "lambda",
    "nonlocal", "pass", "print", "with", "and", "in", "is", "not", "or",
    "except", "finally", "raise", "try", "async", "await", "from", "yield",
    "False", "None", "def", "class",

    // Exceptions
    "BaseException|", "Exception|", "ArithmeticError|", "BufferError|", 
    "LookupError|", "EnvironmentError|", "StandardError|", "AssertionError|", 
    "AttributeError|", "EOFError|", "FloatingPointError|", "GeneratorExit|", 
    "ImportError|", "IndentationError|", "IndexError|", "KeyError|", 
    "KeyboardInterrupt|", "MemoryError|", "NameError|", "NotImplementedError|", 
    "OSError|", "OverflowError|", "ReferenceError|", "RuntimeError|", 
    "StopIteration|", "SyntaxError|", "SystemError|", "SystemExit" "TabError|", 
    "TypeError|", "UnboundLocalError|", "UnicodeError|", "UnicodeDecodeError|", 
    "UnicodeEncodeError|", "UnicodeTranslateError|", "ValueError|", 
    "ZeroDivisionError|", "BlockingIOError|", "BrokenPipeError|", 
    "ChildProcessError|", "ConnectionAbortedError|", "ConnectionError|", 
    "ConnectionRefusedError|", "ConnectionResetError|", "FileExistsError|", 
    "FileNotFoundError|", "InterruptedError|", "IsADirectoryError|", 
    "NotADirectoryError|", "PermissionError|", "ProcessLookupError|", 
    "RecursionError|", "StopAsyncIteration|", "TimeoutError|", "IOError|", 
    "VMSError|", "WindowsError|", "BytesWarning|", "DeprecationWarning|", 
    "FutureWarning|", "ImportWarning|", "PendingDeprecationWarning|", 
    "RuntimeWarning|", "SyntaxWarning|", "UnicodeWarning|", "UserWarning|", 
    "Warning|", "ResourceWarning|",

    // Builtins
    "False`", "True`", "None`", "NotImplemented`", "abs`", "all`", "any`", "bin`",
    "bool`", "bytearray`", "callable`", "chr`", "classmethod`", "compile`", "complex`",
    "delattr`", "dict`", "dir`", "divmod`", "enumerate`", "eval`", "filter`", "float`",
    "format`", "frozenset`", "getattr`", "globals`", "hasattr`", "hash`", "helo`",
    "hex`", "id`", "input`", "int`", "isinstance`", "issubclass`", "iter`", "len`",
    "list`", "locals`", "map`", "max`", "memoryview`", "min`", "next`", "object`",
    "ocr`", "open`", "ord`", "pow`", "print`", "property`", "range`", "repr`",
    "reversed`", "round`", "set`", "setattr`", "slice`", "sorted`", "staticmethod`",
    "str`", "sum`", "super`", "tuple`", "type`", "vars`", "zip`", "__import__`",
    "basestring`", "cmp`", "execfile`", "file`", "log`", "raw_input`", "reduce`",
    "reload`", "unichr`", "unicode`", "xrange`", "ascii`", "bytes`", "exec`",
    "object`", "open`",

    NULL};

/**
 * @brief Database of syntax-highlighting rules
 * 
 */
struct editorSyntax HLDB[] = {
    {
        "C/C++",
        C_HL_EXTENSIONS,
        C_HL_KEYWORDS,
        "//",
        "/*", "*/",
        HL_HIGHLIGHT_NUMBERS | HL_HIGHLIGHT_STRINGS | HL_HIGHLIGHT_HASHTAG | HL_HIGHLIGHT_LTGT
    },
    {
        "Python",
        PYTHON_HL_EXTENSIONS,
        PYTHON_HL_KEYWORDS,
        "#",
        "'''", "'''",
        HL_HIGHLIGHT_NUMBERS | HL_HIGHLIGHT_STRINGS
    },
};

#define HLDB_ENTRIES (sizeof(HLDB) / sizeof(HLDB[0]))

#endif