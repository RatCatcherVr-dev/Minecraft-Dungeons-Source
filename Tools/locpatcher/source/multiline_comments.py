import re
comments_test = "hello // comment\n"+\
                "line 2 /* a comment */\n"+\
                "line 3 /* a comment*/ /*comment*/\n"+\
                "line 4 /* a comment\n"+\
                "continuation of a comment*/ line 5\n"+\
                "/* comment */line 6\n"+\
                "line 7 /*********\n"+\
                "********************\n"+\
                "**************/\n"+\
                "line ?? /*********\n"+\
                "********************\n"+\
                "********************\n"+\
                "********************\n"+\
                "********************\n"+\
                "**************/\n"+\
                "line ??"

comment_re = re.compile(
    r'(^)?[^\S\n]*/(?:\*(.*?)\*/[^\S\n]*|/[^\n]*)($)?',
    re.DOTALL | re.MULTILINE
)

def comment_replacer(match):
    start,mid,end = match.group(1,2,3)
    if mid is None:
        # single line comment
        return ''
    elif start is not None or end is not None:
        # multi line comment at start or end of a line
        return ''
    elif '\n' in mid:
        # multi line comment with line break
        return '\n'
    else:
        # multi line comment without line break
        return ' '

def remove_comments(text):
    return comment_re.sub(comment_replacer, text)


## test it
def test_remove_comments():
    print(comments_test)
    print("....")
    print(remove_comments(comments_test))

if __name__ == "__main__":
    test_remove_comments()