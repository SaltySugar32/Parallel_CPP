import pandas as pd #обработка данных
import matplotlib.pyplot as plt
import argparse

params = argparse.ArgumentParser()
params.add_argument('csv', type=argparse.FileType('r', encoding='utf-8'))
params = params.parse_args()


table = pd.read_csv(params.csv, index_col=0)
plot = table.plot()
plt.show()