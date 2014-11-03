#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "RngStream.h"
#include "cuckoo_search.h"


void init(int dim, int lb, int ub, Nests *Nest, RngStream g1){
	int i;
	for(i=0;i<nnest;i++){
		int j;
		for(j=0;j<dim;j++){
			Nest[i].v[j] = lb+(ub-lb)*RngStream_RandU01(g1);
		}
		Nest[i].fx = INFINITY;
	}

}

void show_nest(int dim, Nests *Nest, Nests *new_Nest, Nests *best, double fxmin){
	int i;
	for (i = 0; i < nnest; i++){
		int j;
		for (j = 0; j < dim; j++){
		//printf("%f\n", best->v[j]);
		}
	}
	printf("%6.3e\n", fxmin);
}

void simplebounds(int dim, Nests *new_Nest, int lb, int ub){
	int i;
	for (i = 0; i < dim; i++)
	{	
		double avg = 0;
		int j;
		for (j = 0; j < nnest; j++){
			avg = avg + new_Nest[j].v[i];
		}
		avg = avg/nnest;

		for (j = 0; j < nnest; j++){
			if(new_Nest[j].v[i] > ub || new_Nest[j].v[i] < lb){
				new_Nest[j].v[i] = avg;
				
			}
		}
	}

}

void get_best_nest(int dim, Nests *best, Nests *new_Nest, Nests *Nest,double (*fobj)(double x[], int)){
	int i, auxIndex;
	double auxMin=INFINITY;
	for (i = 0; i < nnest; i++)
	{
		
		double min = fobj(new_Nest[i].v, dim);
	
		if(min < Nest[i].fx){
			Nest[i].fx = min;
			memcpy(Nest[i].v, new_Nest[i].v, sizeof(Nest[i].v));
		}
		if(Nest[i].fx < auxMin){
				auxMin = Nest[i].fx;
				auxIndex = i;
		}
	}
	memcpy(best->v, Nest[auxIndex].v, sizeof(best->v));
	best->fx = auxMin;

}

void empty_nests(int dim, int lb, int ub, Nests *new_Nest, Nests *Nest, double pa, RngStream g1){

	int i;
	for (i = 0; i < nnest; i++){
		int j;
		for (j = 0; j < dim; j++){
			double num = RngStream_RandU01(g1);
			if(num>pa){
				int rindex = rand() % nnest + 1;
				double stepsize = num * Nest[rindex].v[j] - Nest[rindex].v[j];
				new_Nest[i].v[j] = Nest[i].v[j] + stepsize;
			}
		}
	}

	simplebounds(dim, new_Nest, lb, ub);
	
}

void get_cuckoos(int lb, int ub, int dim, Nests *best, Nests *new_Nest, Nests *Nest, RngStream g1){

	double beta = 3/2;
	double sigma = pow((tgamma(1.0+beta) * sin(M_PI*beta/2.0) / ((tgamma(1.0+beta)/2.0) * beta * (pow(2.0,((beta-1.0)/2.0))))),(1.0/beta));

	int i;
	for (i = 0; i < nnest; i++){
		int j;
		for (j = 0; j < dim; j++){
			double step = (RngStream_RandU01(g1)*sigma) / pow(fabs(RngStream_RandU01(g1)),(1/beta));
			double stepsize = 0.01 * step * (best->v[j]);

			new_Nest[i].v[j] = Nest[i].v[j] + stepsize * RngStream_RandU01(g1);
		}
		
	}
	simplebounds(dim, new_Nest, lb, ub);

}

void CS(int lb, int ub, int dim, Nests *Nest,double (*fobj)(double x[], int)){
	srand(time(0));

	unsigned long germe[6] = { rand(), rand(), rand(), rand(), rand(), rand() };
    RngStream_SetPackageSeed (germe);
    RngStream g1 = RngStream_CreateStream ("Laplace");
	
	double pa = 0.25;
	Nests new_Nest[nnest], best;
	init(dim,lb,ub,Nest, g1);
	memcpy(new_Nest, Nest, sizeof(new_Nest));
	get_best_nest(dim, &best, new_Nest, Nest,fobj);
	double fxmin = best.fx;
	
	int t;
	for (t = 0; t < iter; t++){

		get_cuckoos(lb, ub, dim, &best, new_Nest, Nest,g1);
		get_best_nest(dim, &best, new_Nest, Nest, fobj);
		empty_nests(dim, lb, ub, new_Nest, Nest,pa,g1);
		get_best_nest(dim, &best, new_Nest, Nest, fobj);

		if(best.fx < fxmin){
			fxmin = best.fx;
		}

	}
	
	show_nest(dim, Nest, new_Nest, &best, fxmin);
}
