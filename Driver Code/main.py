from subprocess import Popen, PIPE
import os
import config


# removes a file if it is present
def removeFile(file):
    if file not in os.listdir(os.curdir):
        return "File "+file+" wasn't present: ignoring"

    out, err = Popen('rm '+file, stdout=PIPE, stderr=PIPE, shell=True).communicate()
    out = out.strip()
    err = err.strip()
    if err != '' or out != '':
        raise Exception(err+'\n'+out)
    
    return "File "+file+" was removed"


# returns only if file is compiled successfully
# provided the exec is absent when it is called
def compileFile(file):
    if file not in os.listdir(os.curdir):
        err = "File "+file+" is absent, can't proceed"
        raise Exception(err)

    file_name = file.split('.')[0]
    if file_name=='script_parallel_v2':
      out, err = Popen('/apps/gcc_10_2_0/bin/g++ -o '+file_name+' '+file+' -fopenmp -Wall  -fopt-info-vec-optimized  -ftree-vectorizer-verbose=2  -Wvector-operation-performance -std=c++11', stdout=PIPE, stderr=PIPE, shell=True).communicate()
    else:
      out, err = Popen('/apps/gcc_10_2_0/bin/g++ -o '+file_name+' '+file+' -fopenmp -Wall  -fopt-info-vec-optimized  -ftree-vectorizer-verbose=2  -Wvector-operation-performance -std=c++11', stdout=PIPE, stderr=PIPE, shell=True).communicate()
    out = out.strip()
    err = err.strip()
    if file_name not in os.listdir(os.curdir):
        print out
        print err
        err = "File "+file+" wasn't compiled"
        raise Exception(err)
            
    
    return "File "+file+" was compiled"



if __name__ == "__main__":
    # remove the previous execs and recompile
    try:
        print removeFile('script_serial_v2')
        print removeFile('script_parallel_v2')

        print compileFile('script_serial_v2.cpp')
        print compileFile('script_parallel_v2.cpp')
    except Exception as e:
        print e, ":exiting"
        exit()

    print "*"*80

    reuse = None
    probSizeIndx, procNumIndx, rid = None, None, None

    runs = None
    while type(runs) != int or runs <= 0:
        try:
            runs = int(raw_input('Number of runs: '))
        except:
            pass

    print "Warning: if configs have changed, don't reuse!"
    while reuse != 'n' and reuse != 'y':
        reuse = raw_input('Reuse previous data? (y/n): ')

        if reuse == 'n':
            # don't reuse data, fresh start
            print "Removing data.txt"
            print removeFile('data.txt')
            probSizeIndx, procNumIndx, rid = 0, 0, 0
        elif reuse == 'y':
            if 'data.txt' not in os.listdir(os.curdir):
                print 'data.txt is absent: ignoring'
                probSizeIndx, procNumIndx, rid = 0, 0, 0
            else:
                with open('data.txt', 'r') as f:
                    for line in f.readlines():
                        try:
                            n, p, r, a, e = line.split(',')
                            n, p, r = int(n), int(p), int(r)
                            probSizeIndx = config.prob.index(n)+1
                            procNumIndx = config.procs.index(p)
                            rid = r
                            if rid == runs:
                                print "data.txt has all the required runs, delete additional data"
                                exit()
                        except:
                            print "Configs are inconsistent with data.txt: exiting"
                            exit()

    for runId in range(rid, runs):
        print "*"*80
        print "Run #:", runId+1
        for p in range(procNumIndx, len(config.procs)):
            print "*"*40
            print "Processors:", config.procs[p]
            for n in range(probSizeIndx, len(config.prob)):
                print "Problem Size:", config.prob[n]

                if config.procs[p] == 0:
                    cmd = './script_serial_v2 '+str(config.prob[n])+' ' + str(config.procs[p])
                else:
                    cmd = './script_parallel_v2 '+str(config.prob[n])+' ' + str(config.procs[p])
                    
                process = Popen(cmd, stdout=PIPE, stderr=PIPE, shell=True)
                out, err = process.communicate()
                if process.returncode != 0:
                    print "Exit code:", process.returncode, "exiting"
                    print err.strip()
                    print out.strip()
                    exit()
                else:
                    out = out.strip()
                    algName,algApp,N,P,e2eX,e2eY,algX,algY = out.split(',')

                    # below code is from 'text_to_csv.py from original letshpc'
                    algTime = float(algX) + float(algY) / 1e9
                    e2eTime = float(e2eX) + float(e2eY) / 1e9
                    ###########################################################

                    algTime = str(algTime)
                    e2eTime = str(e2eTime)

                    dataPoint = ','.join([N,P,str(runId),algTime,e2eTime])

                    with open('data.txt', 'a') as f:
                        f.write(dataPoint+'\n')
            
            # next run should begin at 0s
            probSizeIndx = 0
        procNumIndx = 0
