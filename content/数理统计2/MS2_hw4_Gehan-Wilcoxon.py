from numpy import sqrt
import pandas as pd
from lifelines.statistics import multivariate_logrank_test, pairwise_logrank_test
import numpy as np
# Q1

Tk = [1, 2, 3, 4, 5, 6, 7, 8, 10, 11, 12, 13, 15, 16, 17, 22, 23]
rk = [42, 40, 38, 37, 35, 33, 29, 28, 23, 21, 18, 16, 14, 14, 12, 9, 7]
Dk = [2, 2, 1, 2, 2, 3, 1, 4, 1, 2, 2, 1, 1, 1, 1, 2, 2]
r1k = [21, 21, 21, 21, 21, 21, 17, 16, 15, 13, 12, 12, 11, 11, 10, 7, 6]
D1k = [0, 0, 0, 0, 0, 3, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 1]
r2k = [21, 19, 17, 16, 14, 12, 12, 12, 8, 8, 6, 4, 3, 3, 2, 2, 1]
D2k = [2, 2, 1, 2, 2, 0, 0, 4, 0, 2, 2, 0, 1, 0, 1, 1, 1]

data = {"Tk": Tk, "rk": rk, "Dk": Dk, "r1k": r1k, "D1k": D1k, "r2k": r2k, "D2k": D2k}

data = pd.DataFrame(data)
assert (data["Dk"] == data["D1k"] + data["D2k"]).all() and (
    data["rk"] == data["r1k"] + data["r2k"]
).all()

W_G = (data["r1k"] * data["D2k"] - data["r2k"] * data["D1k"]).sum()
C = 21 * 21 / ((21 + 21) * (21 + 21 - 1))
sum_of_uxij = (data["rk"] * data["Dk"] * (data["rk"] - data["Dk"])).sum()
V = C * sum_of_uxij
Z = W_G / sqrt(V)

print(f"W_G:{W_G}")
print(f"V:{V}")
print(f"Z:{Z}")

# Q2

t1 = [4, 5, 9, 12, 20, 25, 30]
e1 = [1, 1, 1, 1, 0, 1, 0]  # 治疗1
t2 = [1, 4, 9, 12, 15, 23, 30]
e2 = [1, 1, 1, 1, 1, 1, 1]  # 治疗2
t3 = [3, 7, 14, 20, 27, 30, 32, 50]
e3 = [1, 1, 1, 1, 1, 1, 0, 0]  # 治疗3
t4 = [5, 15, 20, 31, 39, 47, 55, 67]
e4 = [1, 1, 1, 1, 1, 1, 0, 0]  # 治疗4
times = np.array(t1 + t2 + t3 + t4)
events = np.array(e1 + e2 + e3 + e4)
groups = (
    ["治療1"] * len(t1)
    + ["治療2"] * len(t2)
    + ["治療3"] * len(t3)
    + ["治療4"] * len(t4)
)

res = multivariate_logrank_test(times, groups=groups, event_observed=events)
print(res.test_statistic, res.p_value)  # 与上面手算结果应一致（微小数值差异）
