
with open('values.cpp', 'r') as source:
    source_txt = source.read()

with open('values.h', 'r') as header:
    out = ""
    for line in header:
        if (end := line.find("parse(Cursor<Css::Sst>& c);")) != -1:
            start = line.find("<")
            typename = line[start+1:end-2]
            print(line[:-2])

            query = f"ValueParser<{typename}>::parse(Cursor<Css::Sst>& c)"
            code_start = source_txt.find(query) + len(query) + 2

            index = code_start
            stack = ['{']
            while stack:
                if source_txt[index] == '{':
                    stack.append(source_txt[index])
                elif source_txt[index] == '}':
                    stack.pop()
                index += 1
            
            print(source_txt[code_start-1:index+1])
        else:
            print(line, end="")
                

