import numpy as np
import matplotlib.pyplot as plt

x = np.loadtxt('times.txt').T
p = [2, 4, 8, 16, 32]

# plt.plot(p, x[0] / 10 ** 6, marker='.', ls=':', c = 'r')
# plt.ylabel('CPU time in s')
# plt.xlabel('#threads')
# plt.title("CPU time, using clock()")

# for i, j in zip(p, x[0]):
#     plt.text(i, j / 10 ** 6, f'({i}, {str(j / 10 ** 6)[:4]})')

plt.plot(p, x[1] / 10 ** 6, marker='.', ls=':', c = 'r')
plt.ylabel('Real time in s')
plt.xlabel('#threads')
plt.title("Real time, using clock_gettime()")

for i, j in zip(p, x[1]):
    plt.text(i, j / 10 ** 6, f'({i}, {str(j / 10 ** 6)[:4]})')


# plt.show()
plt.savefig('t_real.png')