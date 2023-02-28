import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv('bcas.csv')
bcas = [q.mean() for q in np.split(np.array(df['avg']), 5)]

df = pd.read_csv('cas.csv')
cas = [q.mean() for q in np.split(np.array(df['avg']), 5)]

df = pd.read_csv('tas.csv')
tas = [q.mean() for q in np.split(np.array(df['avg']), 5)]

plt.plot(range(10, 51, 10), bcas, label = 'bcas')
plt.plot(range(10, 51, 10), cas, label = 'cas')
plt.plot(range(10, 51, 10), tas, label = 'tas')

plt.grid()
plt.legend()
for i in range(10, 51, 10):
    plt.annotate(f'({i}, {bcas[i//10 - 1]:.3f})', (i, bcas[i//10 - 1]))
    plt.annotate(f'({i}, {cas[i//10 - 1]:.3f})', (i, cas[i//10 - 1]))
    plt.annotate(f'({i}, {tas[i//10 - 1]:.3f})', (i, tas[i//10 - 1]))
    
plt.title('Average')
plt.xlabel('#Threads')
plt.ylabel('Waiting time in seconds')
plt.savefig('avg.png')