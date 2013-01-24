void assignptr(int **ptrtoptr){
	int i;
	*ptrtoptr = (int *)malloc(3 * sizeof(int));
	for(i=0;i<3;i++){
		*(*ptrtoptr + i) = i*3;
	}
}

int main(){
	int i;
	int *ptr;
	int **ptrtoptr = &ptr;
	assignptr(ptrtoptr);
	for(i=0;i<3;i++){
		printf("array[%d] = %d\n", i, *(ptr + i));
	}

	return(0);
}
