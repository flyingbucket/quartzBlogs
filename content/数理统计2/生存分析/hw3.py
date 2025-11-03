import pandas as pd

data = {
    "Nj": [
        2418,
        1962,
        1697,
        1523,
        1329,
        1170,
        938,
        722,
        546,
        427,
        321,
        233,
        146,
        95,
        59,
        30,
    ],
    "Dj": [456, 226, 152, 171, 135, 125, 83, 74, 51, 42, 43, 34, 18, 9, 6, 0],
    "Wj": [0, 39, 22, 23, 24, 107, 133, 102, 68, 64, 45, 53, 33, 27, 23, 0],
}

table = pd.DataFrame(data)

last_row = pd.DataFrame(
    [
        {
            "Nj": 30,
            "Dj": 30,
            "Wj": 0,
        }
    ],
    index=["inf"],
)

table = pd.concat([table, last_row])
# varify:N_{j+1}=Nj-Dj-Wj
table["Nj_next_calc"] = table["Nj"] - table["Dj"] - table["Wj"]

table["Nj_next_actual"] = table["Nj"].shift(-1)

table["diff"] = table["Nj_next_actual"] - table["Nj_next_calc"]

assert (table["diff"].dropna() == 0).all(), (
    "N_{j+1}=Nj-Dj-Wj not satisfied,check original data"
)
table.drop(columns=["Nj_next_actual", "Nj_next_calc", "diff"], inplace=True)

# life table method
table["Nj_prime"] = table["Nj"] - table["Wj"] / 2
table["qj"] = table["Dj"] / table["Nj_prime"]
table["pj"] = 1 - table["qj"]
table["Pj"] = table["pj"].cumprod()

print(table)
