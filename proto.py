import json
import subprocess
import os

#with open("build/compile_commands.json") as f:
with open("compile_commands.json") as f:
    db = json.load(f)

for entry in db:
    src = entry["file"]

    if os.path.basename(src) == "main.c":
        continue

    if os.path.basename(src) == "lex.yy.c":
        continue

    if os.path.basename(src) == "parser.tab.c":
        continue

    if "build/" in os.path.dirname(src):
        continue

    if not src.endswith(".c"):
        continue

    cmd = entry["command"].split()
    compiler_flags = []

    i = 0
    while i < len(cmd):
        arg = cmd[i]

        if arg.startswith(("-I")):#, "-D", "-U")):
            compiler_flags.append(arg)

        elif arg in ("-isystem", "-include", "-imacros"):
            compiler_flags.extend(["-I", cmd[i + 1]])
            i += 1

        i += 1

#    print(compiler_flags)

    proto = os.path.splitext(src)[0] + ".proto.h"

    subprocess.run(
        ["cproto", *compiler_flags, src],
        stdout=open(proto, "w"), stderr=open("/dev/null", "w")
    )
#    print(["cproto", *compiler_flags, src])
