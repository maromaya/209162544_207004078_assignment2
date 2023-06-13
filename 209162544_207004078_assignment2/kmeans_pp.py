import mykmeanssp
import numpy as np
import pandas as pd
import sys
np.random.seed(0)

def checktypeClus(str):
    try:
        bol = (int(str) == float(str))
        return not bol
    except:
        print("Invalid number of clusters!")
        sys.exit(1)


def checktypeIter(str):
    try:
        bol = (int(str) == float(str))
        return not bol
    except:
        print("Invalid maximum iteration!")
        sys.exit(1)

#input
arglist = sys.argv
if len(sys.argv) == 6:
    try:
        K = int(arglist[1])
    except ValueError:
        print("Invalid number of clusters!")
        quit()
    try:
        iteration_number = int(arglist[2])
    except ValueError:
        print("Invalid maximum iteration!")
        quit()

    if (checktypeClus(arglist[1]) or int(arglist[1]) < 2):
        print("Invalid number of clusters!")
        sys.exit(1)
    if (checktypeIter(arglist[2]) or int(arglist[2]) < 1 or int(arglist[2]) > 1000):
        print("Invalid maximum iteration!")
        sys.exit(1)
    try:
        eps=float(arglist[3])
    except ValueError:
        print("An Error Has Occured")
        quit() 

    In_filename1=arglist[4]
    In_filename2=arglist[5]

elif len(sys.argv)==5:
    try:
        K = int(arglist[1])
    except ValueError:
        print("Invalid number of clusters!")
        quit()

    if (checktypeClus(arglist[1]) or int(arglist[1]) < 2):
        print("Invalid number of clusters!")
        sys.exit(1)
    
    try:
        eps=float(arglist[2])
    except ValueError:
        print("An Error Has Occured")
        quit()
    
    In_filename1=arglist[3]
    In_filename2=arglist[4]

if 'iteration_number' not in locals():
    iteration_number = 300



#data

try:
    input1=pd.read_csv(In_filename1,delimiter=',',header=None)
    input2=pd.read_csv(In_filename2,delimiter=',',header=None)
except Exception:
    print("An Error Has Occured") ##JUST IN CASE
    quit()
    
try:
    inner=pd.DataFrame.merge(input1,input2,how='inner',left_on=0,right_on=0)
except Exception:
    print("An Error Has Occured")
    quit()
inner.sort_values(by=0, inplace=True)
inner.set_index(inner[0],inplace=True)
inner=inner.iloc[:,1:]
N=inner.shape[0]
d=inner.shape[1]


def find_argmin_cluster_to_point(centroid_list,point,i): 
    min=sys.maxsize
    for j in range(i):
        temp = np.linalg.norm(point - centroid_list[j])
        if temp<min:
            min=temp
    return min

i=1
centroid_list=[None]*K
D=[0]*N
P=[0]*N
centorid1=inner.loc[np.random.choice(N)]
centroid_list[0]=centorid1
while i<K:
    for l in range(N):
        D[l]=find_argmin_cluster_to_point(centroid_list,inner.loc[l],i)
    if np.sum(D)==0:
        print("An Error Has Occurred")
        quit()
    for l in range(N):
        P[l]=D[l]/np.sum(D)
    i+=1
    centroid_list[i-1]=inner.loc[np.random.choice(N,p=P)]
result_index=[]
res_centroids=[]
for centroid in centroid_list:
    res_centroids.append(centroid.values.tolist())
    result_index.append(str(int(centroid.name)))
inner=inner.values.tolist()
returned_tuple_from_c=mykmeanssp.fit(K,N,d,iteration_number,eps, inner,res_centroids)
print(",".join(result_index))
for tuple in returned_tuple_from_c:
    mystr=",".join(["{:.4f}".format(number) for number in tuple])
    print(mystr)



