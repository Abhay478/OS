import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv('q.csv')
cars = [q.mean() for q in np.split(np.array(df['c']), 5)]

plt.plot(range(5, 26, 5), cars)

plt.grid()
for i in range(5, 26, 5):
    plt.annotate(f'({i}, {cars[i//5 - 1]:.3f})', (i, cars[i//5 - 1]))
    
plt.title('Time vs Cars')
plt.xlabel('# Cars')
plt.ylabel('Tour time in milliseconds')
# plt.show()
plt.savefig('cars.png')