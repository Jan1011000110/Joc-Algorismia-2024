import os
import sys
import random
import time
import threading

# ===================== CONFIG ========================

#bots = ["Dummy", "Dummy", "Dummy", "Harry"]
#bots = ["Ron2", "Dummy", "Dummy", "Harry"]
#bots = ["Martinet", "Dummy", "Dummy", "Harry"]
#bots = ["HarryBet", "Ron2", "Martinet", "Harry"]
bots = ["HarryBet", "Harry3", "Harry2", "Harry"]

in_file = "default.cnf" 
out_file = "default.out"
log_file = "/dev/null"

THREAD_NUM = 20

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

kill_threads = False

glob_i = 1
real_i = 0
def playGames(t_id):
    global glob_i, real_i, kill_threads
    try:
        while (glob_i <= n):
            if (kill_threads): return
            i = glob_i
            glob_i += 1
            cmd = "./Game "
            for bot in bots:
                cmd += bot+" "
            cmd += "-s "+str(random.randint(1, 999999))+" < " + in_file + " > " + f"{t_id}.".join(out_file.split(".")) + " 2> " + log_file
            #print("\n"+cmd)
            os.system(cmd)
            #os.system("cat "+f"{t_id}.".join(out_file.split(".")))
            with open(f"{t_id}.".join(out_file.split(".")), "r") as f:
                #print(f"{t_id}.".join(out_file.split(".")))
                txt = f.read()
                #print(txt)
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
                real_i += 1
            os.system("rm "+f"{t_id}.".join(out_file.split(".")))
            time.sleep(0.1)
    except:
        print(f"Thread number {t_id} died unexpectedly.")

trarr = []
for i in range(THREAD_NUM):
    trarr.append(threading.Thread(target=playGames, args=[i]))
    #trarr[len(trarr)-1].daemon = True
    trarr[len(trarr)-1].start()

while True:
    try:
        time.sleep(1) 
        if glob_i > n:
            for tr in trarr:
                tr.join()
            break
        i = real_i
        if i == 0:
            continue
        print("\n")
        print(f'{"Games 1.."+str(i)+":":<20}\t{"Average score":^31}{"Winrate":>18}')
        for bi in range(len(bots)):
            print(f'\t{bots[bi]+":":<20} {scores[bi]/i:^20.0f} {wins[bi]*100/i:>20.0f}%')
    except:
        print("Exiting...")
        kill_threads = True
        break

if (not kill_threads):
    i = real_i
    print("\n")
    print(f'{"Games 1.."+str(i)+":":<20}\t{"Average score":^31}{"Winrate":>18}')
    for bi in range(len(bots)):
        print(f'\t{bots[bi]+":":<20} {scores[bi]/i:^20.0f} {wins[bi]*100/i:>20.0f}%')