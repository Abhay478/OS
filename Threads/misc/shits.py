import numpy as np
import matplotlib.pyplot as plt
import scipy.stats as sc

rv = sc.uniform(-1, 2)
x = np.linspace(-2, 2)
a = rv.pdf(x)
y = np.linspace(-2, 2)
b = rv.pdf(y)
plt.plot(x, rv.pdf(x))
plt.xlim(-2, 2)
plt.show()