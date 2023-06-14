import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from sklearn import datasets
from sklearn.cluster import KMeans


iris = datasets.load_iris()
data1 = pd.DataFrame(iris.data, columns=iris.feature_names)


inertia = []

for k in range(1, 11):
    kmeans = KMeans(n_clusters = k, init = 'k-means++', random_state = 0).fit(data1)
    inertia.append(kmeans.inertia_)
    

plt.plot(range(1, 11), inertia)
plt.title('Bonus')
plt.xlabel('K value')
plt.ylabel('Inertia')
plt.annotate('elbow', xy=(3, 85),
            xytext=(1.5, 50),
            arrowprops=dict(facecolor='green', shrink=0.05),
            horizontalalignment='right', verticalalignment='top',
            ) 

plt.savefig('elbow.png')
