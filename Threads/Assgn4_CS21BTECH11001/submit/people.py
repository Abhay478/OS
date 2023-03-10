import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv('q.csv')
people = [q.mean() for q in np.split(np.array(df['p']), 9)]

plt.plot(range(10, 51, 5), people)

# print(people)

plt.grid()
for i in range(5, 46, 5):
    plt.annotate(f'({i + 5}, {people[i//5 - 1]:.3f})', (i + 5, people[i//5 - 1]))
    
plt.title('Time vs People')
plt.xlabel('# People')
plt.ylabel('Tour time in milliseconds')
# plt.show()
plt.savefig('people.png')