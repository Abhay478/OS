import numpy as np
import matplotlib.pyplot as plt

x = np.loadtxt('points.txt').T
p = [1, 2, 3, 4, 5]

# plt.plot(p, x[0] / 10 ** 6, marker='.', ls=':', c = 'r')
# plt.ylabel('CPU time in s')
# plt.xlabel('#points in millions')
# plt.title("CPU time, using clock()")

# for i, j in zip(p, x[0]):
#     plt.text(i, j / 10 ** 6, f'({i}, {str(j / 10 ** 6)[:4]})')

plt.plot(p, x[1] / 10 ** 6, marker='.', ls=':', c = 'r')
plt.ylabel('Real time in s')
plt.xlabel('#points in millions')
plt.title("Real time, using clock_gettime()")

for i, j in zip(p, x[1]):
    plt.text(i, j / 10 ** 6, f'({i}, {str(j / 10 ** 6)[:4]})')


# plt.show()
plt.savefig('p_real.png')