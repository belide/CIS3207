from scipy import stats
from matplotlib import pyplot as plt
import subprocess as sp
import numpy as np


# Return a list of time measured
def timer(app):
    results = [0] * 100

    for i in range(100):
        timer1 = sp.Popen([app], stdin=sp.PIPE, stdout=sp.PIPE, stderr=sp.PIPE)
        output = float(timer1.communicate()[0])
        results[i] = output

    return results


# Statistics stuff
def calculate_stats(data):
    N = len(data)

    # calculate mean
    print("Mean:", sum(data)/N)

    # calculate median
    data2 = sorted(data)
    if N % 2 == 0:
        print("Median:", (data2[N//2] + data2[N//2 - 1])/2)
    else:
        print(data2[N//2])

    # calculate mode
    print("Mode:", float(stats.mode(data)[0]))

if __name__ == "__main__":
    timer1 = timer("./timer1")
    timer2 = timer("./timer2")

    print("Timer 1:")
    calculate_stats(timer1)

    print("Timer 2:")
    calculate_stats(timer2)


    plt.hist(timer2, normed=True, bins=10)
    plt.ylabel("Microseconds")
    plt.xlabel("Time to execution for Timer 2")
    plt.show()
