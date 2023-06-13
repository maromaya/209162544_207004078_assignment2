
#include <Python.h>
#define PY_SSIZE_T_CLEAN
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


static double ECDis(double* arr1, double* arr2, int d);
static int checkEPS(double **curCent,double **prevCent,int K,int d, double eps);
static double** kmeans(double** points_list,double** curClusters,long K,long row_counter,long d,long iteration_number, double eps);


static void convertToCArray(PyObject * list, double** cData,long numOfItems,long dimension){
//cArray initialize
 long i,j;
 PyObject* listItem;
 PyObject* item;

 //putting vals in c array
 for (i = 0; i < numOfItems; i++) {
        listItem = PyList_GetItem(list, i);
        if (!PyList_Check(listItem)){  /* We only print lists */
           continue;
        }
        for (j= 0; j < dimension; j++) {
            item = PyList_GetItem(listItem, j);
            cData[i][j] = PyFloat_AsDouble(item);
        }
    }
}


static int checkEPS(double **curCent,double **prevCent,int K,int d, double eps){
    int i;
    for(i=0;i<K;i++){
        double res;
        res=ECDis(curCent[i],prevCent[i],d);
        if (res>eps){
            return 1;
        }

    }
    return 0;
}


static double ECDis(double* arr1, double* arr2, int d) {
    double sum = 0.0;
    double sub;
    int i;
    for (i = 0; i < d; i++) {
        sub = arr1[i] - arr2[i];
        sum += sub * sub;
    }
    return sqrt(sum);
}


static double** kmeans(double** points_list,double** curClusters,long K,long row_counter,long d,long iteration_number, double eps){
    int * point_cluster_array;
    int min_clust;
    double min_value=1e20;
    double temp_value;
    double **prevClusters;
    int iteration;
    int k;
    int count;
    int i;
    int j;

    min_clust=0;


    /*bulid k curcent and prevcent array*/
    prevClusters = malloc(K*sizeof(double*));
    if(prevClusters==NULL){
        printf("An Error Has Occurred");
        exit(1);
    }

    for(i=0 ; i<K ; i++ ){
        prevClusters[i] = malloc(d*sizeof(double));
        if(prevClusters[i]==NULL){
            printf("An Error Has Occurred");
            exit(1);
        }
      
    }

    /*build array that connects point to cluster by index*/
    point_cluster_array=malloc(row_counter*sizeof(int));
    if(point_cluster_array==NULL){
        printf("An Error Has Occured");
        exit(1);
    };


    /*find to each point her centroid*/
    for(i =0; i< row_counter;i++){
        min_value= 1e20;
        for(j=0;j<K;j++){
            temp_value = ECDis(points_list[i], curClusters[j],d);

             if (temp_value<min_value){
                min_value=temp_value;
                min_clust=j;
            }

        }
        point_cluster_array[i]=min_clust;
    }

    /*update cent*/
    /*copy cur to prev before the change*/
        for(i=0 ; i<K ; i++){
            for(j=0;j<d;j++){
                prevClusters[i][j]=curClusters[i][j];
                curClusters[i][j]=0.0;
            }
        }

        /*update centroid*/
        for(i=0 ; i<K ; i++){
            count =0;
            for(j =0; j< row_counter ; j++){
                if(i==point_cluster_array[j]){
                    count+=1;
                    for (k=0;k<d;k++){
                        curClusters[i][k]+=points_list[j][k];
                    }                    
                }
            }
            if (count==0){
                printf("An Error Has Occurred");
                exit(1);
            }
            for (k=0;k<d;k++){
                curClusters[i][k]=curClusters[i][k]/count;
            }
        }
        iteration=1;

    while ((iteration<iteration_number) && (checkEPS(curClusters,prevClusters,K,d,eps)==1)){
        iteration+=1;
        /*find cluster to all points*/
        for(i =0; i< row_counter;i++){
            min_value=1e20;
            for(j=0;j<K;j++){
                temp_value = ECDis(points_list[i], curClusters[j],d);
                if (temp_value<min_value){
                    min_value=temp_value;
                    min_clust=j;
                }
            }
            point_cluster_array[i]=min_clust;
        }
        /*copy cur to prev before the change*/
        for(i=0 ; i<K ; i++){
            for(j=0;j<d;j++){
                prevClusters[i][j]=curClusters[i][j];
                curClusters[i][j]=0.0;
            }
        }

        /*update centroid*/
        for(i=0 ; i<K ; i++){
            count =0;
            for(j =0; j< row_counter ; j++){
                if(i==point_cluster_array[j]){
                    count+=1;
                    for (k=0;k<d;k++){
                        curClusters[i][k]+=points_list[j][k];
                    }                    
                }
            }
            if (count==0){
                printf("An Error Has Occurred");
                exit(1);
            }
            for (k=0;k<d;k++){
                curClusters[i][k]=curClusters[i][k]/count;
            }
        }
    }
    
    for(i = 0; i<K ; i++){
        free(prevClusters[i]);
    }
    free(prevClusters);
    free(point_cluster_array);

    return curClusters;  
    
}

static PyObject* fit(PyObject *self, PyObject *args)
{
    long K;
    long N;
    long d;
    int i;
    int j;
    int m;
    double eps;
    long maxIter;
    double** observationsC;
    double** centroidsC;
    PyObject* observations;
    PyObject* centroids;
    PyObject *result;
    PyObject *item;
    double** test;


    if(!PyArg_ParseTuple(args, "lllldOO:kmeansspp", &K, &N, &d, &maxIter, &eps, &observations,&centroids)) {
        printf("An Error Has Occurred");
        return NULL;
    }
    if (!PyList_Check(observations)){
        printf("An Error Has Occurred");
        return NULL;}
    if (!PyList_Check(centroids)){
         printf("An Error Has Occurred");
         return NULL;}
    
    //initialize memory for c arrays
     observationsC = calloc(N,sizeof(double*));
     for( m=0 ; m<N ; m++ ) {
        observationsC[m] = calloc(d, sizeof(double));
        assert(observationsC[m] != NULL);
    }
     centroidsC = calloc(K,sizeof(double*));
     for( m=0 ; m<K ; m++ ) {
        centroidsC[m] = calloc(d, sizeof(double));
        assert(centroidsC[m] != NULL);
    }

    convertToCArray(observations,observationsC,N,d);
    convertToCArray(centroids,centroidsC,K,d);
    test=kmeans(observationsC,centroidsC,K,N,d,maxIter, eps);
    result = PyTuple_New(K);
    for (i = 0; i < K; i++) {
        item = PyTuple_New(d);
    for (j = 0; j < d; j++)
        PyTuple_SET_ITEM(item, j, PyFloat_FromDouble(test[i][j]));
    PyTuple_SET_ITEM(result, i, item);
    }   
    
    for(i = 0; i<K ; i++){
        free(centroidsC[i]);
    }
    free(centroidsC);
    for(i = 0; i<N ; i++){
        free(observationsC[i]);
    }
    free(observationsC);
    return result;
}

#define FUNC(_flag, _name, _docstring) { #_name, (PyCFunction)_name, _flag, PyDoc_STR(_docstring) }

static PyMethodDef _methods[] = {
    {"fit", (PyCFunction)fit, METH_VARARGS},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef _moduledef = {
    PyModuleDef_HEAD_INIT,
    "mykmeanssp",
    NULL,
    -1,
    _methods
};

PyMODINIT_FUNC
PyInit_mykmeanssp(void)
{
    return PyModule_Create(&_moduledef);
}