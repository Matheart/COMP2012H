D DATE "Date: \w\w\w \w\w\w  ?\d+ \d\d:\d\d:\d\d \d\d\d\d"

D COMMIT_HEAD "commit ([a-f0-9]+)[ \t]*\n(?:Merge:\s+[0-9a-f]{7}\s+[0-9a-f]{7}[ ]*\n)?${DATE}"

D COMMIT_LOG "(===[ ]*\ncommit [a-f0-9]+[ ]*\n(?:Merge:\s+[0-9a-f]{7}\s+[0-9a-f]{7}[ ]*\n)?${DATE}[ ]*\n(?:.|\n)*?(?=$|\n===))"

D ARBLINE "[^\n]*(?=\n|$)"

D ARBLINES "(?:(?:.|\n)*(?:\n|$)|^|$)"
