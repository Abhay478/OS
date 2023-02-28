import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv('cas.csv')
wc = np.array(df['wc'])
x = [q.mean() for q in np.split(wc, 5)]

av = np.array(df['avg'])
y = [q.mean() for q in np.split(av, 5)]

plt.plot(range(10, 51, 10), x, label = 'Worst-case')
plt.plot(range(10, 51, 10), y, label = 'Average')
for i in range(10, 51, 10):
    plt.annotate(f'({i}, {x[i//10 - 1]:.3f})', (i, x[i//10 - 1]))
    plt.annotate(f'({i}, {y[i//10 - 1]:.3f})', (i, y[i//10 - 1]))
    
plt.title('CAS')
plt.legend()
plt.grid()
plt.xlabel('#Threads')
plt.ylabel('Waiting time in seconds')
plt.savefig('cas.png')