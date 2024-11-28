import os
import sys
import random
import time

# ===================== CONFIG ========================

bots = ["Dummy", "Dummy", "Dummy", "Harry"]

in_file = "default.cnf" 
out_file = "default.out"
log_file = "log.out"

# ===================== CONFIG ========================


lst_st = " "
with open("last_compile_state.log", "r") as f:
    lst_st = f.read()
if lst_st != "turbo":
    os.system("make clean")
    with open("last_compile_state.log", "w") as f:
        f.write("turbo")
os.system("bash -c \"export GOFAST='-DGOFAST';export DEBUGFLG='';make\"")

n = int(sys.argv[1])

scores = [0 for x in bots]
wins = [0 for x in bots]

for i in range(1, n+1):
    time.sleep(0.1)
    cmd = "./Game "
    for bot in bots:
        cmd += bot+" "
    cmd += "-s "+str(random.randint(1, 999999))+" < " + in_file + " > " + out_file + " 2> " + log_file
    print("\n"+cmd)
    os.system(cmd)
    with open("default.out", "r") as f:
        txt = f.read()
        txt = txt.split("\n")
        for j in range(1, len(txt)):
            tx = txt[-j].split("\t")
            if (tx[0] == "score"):
                bb = -1
                bs = 0
                for bi in range(len(bots)):
                    sc = int(tx[bi+1])
                    scores[bi] += sc
                    if sc > bs:
                        bs = sc 
                        bb = bi
                wins[bb] += 1
                break
        print(f'{"Games 1.."+str(i)+":":<20}\t{"Average score":^31}{"Winrate":>18}')
        for bi in range(len(bots)):
            print(f'\t{bots[bi]+":":<20} {scores[bi]/i:^20.0f} {wins[bi]*100/i:>20.0f}%')
        