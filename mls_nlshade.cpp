
#include"de.h"
#include<bits/stdc++.h>

Fitness MLS_LSHADE::run() {
	
    cout << scientific << setprecision(8);
    initializeParameters();
    setMLS_LSHADEParameters();
    //set up the global best individual
    Individual bsf_general_solution= (variable*)malloc(sizeof(variable) * problem_size);
    Fitness bsf_general_fitness= 9999999999.0;
  
    //initialize population
    vector <Individual> pop;
    vector <Fitness> fitness(pop_size, 0);
    vector <Individual> children;
    vector <Fitness> children_fitness(pop_size, 0);
  
 
    for (int i = 0; i < pop_size; i++) {
        pop.push_back(makeNewIndividual());
        children.push_back((variable*)malloc(sizeof(variable) * problem_size));
    }
    // evaluate the initial population's fitness values
    evaluatePopulation(pop, fitness);
    //temporary best individual of population
    Individual bsf_solution = (variable*)malloc(sizeof(variable) * problem_size);
    Fitness bsf_fitness;
    int nfes = 0;
    if ((fitness[0] - optimum) < epsilon) fitness[0] = optimum;
    bsf_fitness = fitness[0];
    for (int j = 0; j < problem_size; j ++) bsf_solution[j] = pop[0][j];
    /////////////////////////////////////////////////////////////////////////
    for (int i = 0; i < pop_size; i++) {
        nfes++;
        if ((fitness[i] - optimum) < epsilon) fitness[i] = optimum;
        if (fitness[i] < bsf_fitness) {
            bsf_fitness = fitness[i];
        for (int j = 0; j < problem_size; j ++) bsf_solution[j] = pop[i][j];
        }
    if (nfes >= max_num_evaluations) break;
    }
    ////////////////////////////////////////////////////////////////////////////

    //for external archive
    int arc_ind_count = 0;
    int random_selected_arc_ind;
    vector <Individual> archive;
    for (int i = 0; i < arc_size; i++) archive.push_back((variable*)malloc(sizeof(variable) * problem_size));
  
    int num_success_params;
    vector <variable> success_sf;
    vector <variable> success_cr;
    vector <variable> dif_fitness;

    // the contents of M_f and M_cr are all initialiezed 0.5
    vector <variable> memory_sf(memory_size, 0.5);
    vector <variable> memory_cr(memory_size, 0.5);
    variable temp_sum_sf;
    variable temp_sum_cr;
    variable sum;
    variable weight;

    //memory index counter
    int memory_pos = 0;
 
    //for new parameters sampling
    variable mu_sf, mu_cr;
    int random_selected_period;
    variable *pop_sf = (variable*)malloc(sizeof(variable) * pop_size);
    variable *pop_cr = (variable*)malloc(sizeof(variable) * pop_size);

    //for current-to-pbest/1
    int p_best_ind;
    int p_num = round(pop_size *  p_best_rate);
    int *sorted_array = (int*)malloc(sizeof(int) * pop_size);
    Fitness *temp_fit = (Fitness*)malloc(sizeof(Fitness) * pop_size);

    // for linear population size reduction
    int max_pop_size = pop_size;
    int min_pop_size = 4;
    int plan_pop_size;
  
    //for renew process
    int generation=0;
    vector <double> bsf_fitness_generations;
    vector<Individual> bsf_solution_generations; 
    Individual tem= new double[problem_size];
    int k=0;
    
   //main loop
    while (nfes < max_num_evaluations) {
  	    generation++;
        for (int i = 0; i < pop_size; i++) sorted_array[i] = i;
        for (int i = 0; i < pop_size; i++) temp_fit[i] = fitness[i];
        sortIndexWithQuickSort(&temp_fit[0], 0, pop_size - 1, sorted_array);

        //update the best of population throught out generations
  	    bsf_fitness_generations.push_back(bsf_fitness);
     	Individual tt=makeNewIndividual();
      	for(int i=0;i<problem_size;i++) {
  	    	tt[i]=bsf_solution[i];
	    }
     	bsf_solution_generations.push_back(tt);

	//population evolve at least 50*(problem_size/10)*(problem_size/10) before considering to recreate
	if(generation>50*(problem_size/10)*(problem_size/10)){
		if(bsf_fitness_generations[bsf_fitness_generations.size()-50*(problem_size/10)*(problem_size/10)]-bsf_fitness_generations[bsf_fitness_generations.size()-1]<epsilon) {
	    	// the maximum number of evaluation
	    	int max_num_DSGC=min(max_num_evaluations-nfes,(unsigned int)(max_num_evaluations/problem_size));
		
	    	//local seach for the temporary best individual of population
	    	int t=DSCG_search(pop[sorted_array[0]],fitness[sorted_array[0]],problem_size,0.02 * (max_region-min_region),pow(10,-4.0*(problem_size/10.0)),0.75,max_num_DSGC);
	    	nfes+=t;
  	        if ((fitness[sorted_array[0]] - optimum) < epsilon) fitness[sorted_array[0]] = optimum;
  	      
	    	// update global best
	    	if(fitness[sorted_array[0]]<bsf_general_fitness) {
  	            bsf_general_fitness = fitness[sorted_array[0]];
  	            for (int j = 0; j < problem_size; j ++) {
		            bsf_general_solution[j] = pop[sorted_array[0]][j];
	            }
  	        }
  	    
  	        //for output file
  	        if(nfes>=floor(pow(problem_size,double(k)/5.0-3.0)*max_num_evaluations)) {
	    		records[run_id][k]=min(bsf_general_fitness,bsf_fitness)-optimum;
	    		k++;
		    }
		
  	        //break if runing out of evaluations
  	        if(nfes >= max_num_evaluations) break;
  	    
  	        // renew pop
  	        bsf_fitness=999999999999.0;
	        generation=0;
	        bsf_fitness_generations.clear();
	        bsf_solution_generations.clear();

  	        //initialize population
            for (int i = 0; i < pop_size; i++) {
                tem=makeNewIndividual();
                for(int j=0;j<problem_size;j++) pop[sorted_array[i]][j]=tem[j];
                    evaluateIndividual(pop[sorted_array[i]],fitness[sorted_array[i]]);
            }  	   
    	    for (int i = 0; i < pop_size; i++) {
                nfes++;
                if ((fitness[i] - optimum) < epsilon) fitness[i] = optimum;
                if (fitness[i] < bsf_fitness) {
                    bsf_fitness = fitness[i];
                    for (int j = 0; j < problem_size; j ++) bsf_solution[j] = pop[i][j];
                }
                if(nfes>=floor(pow(problem_size,double(k)/5.0-3.0)*max_num_evaluations)) {
			        records[run_id][k]=min(bsf_general_fitness,bsf_fitness)-optimum;
			        k++;
		        }
                if (nfes >= max_num_evaluations) {break;}
            }
            if (nfes >= max_num_evaluations) {break;}
	    }  
	}
	// 
	
	for (int i = 0; i < pop_size; i++) sorted_array[i] = i;
    for (int i = 0; i < pop_size; i++) temp_fit[i] = fitness[i];
    sortIndexWithQuickSort(&temp_fit[0], 0, pop_size - 1, sorted_array);
    for (int target = 0; target < pop_size; target++) {
        //In each generation, CR_i and F_i used by each individual x_i are generated by first selecting an index r_i randomly from [1, H] 
        random_selected_period = rand() % memory_size;
        mu_sf = memory_sf[random_selected_period];
        mu_cr = memory_cr[random_selected_period];

        //generate CR_i and repair its value
        if (mu_cr == -1) {
	        pop_cr[target] = 0;
        }
        else {
	        pop_cr[target] = gauss(mu_cr, 0.1);
         	if (pop_cr[target] > 1) pop_cr[target] = 1;
        	else if (pop_cr[target] < 0) pop_cr[target] = 0;	
        }

        //generate F_i and repair its value
        do {	
    	pop_sf[target] = cauchy_g(mu_sf, 0.1);
    	}
        while (pop_sf[target] <= 0);

        if (pop_sf[target] > 1) pop_sf[target] = 1;

        //p-best individual is randomly selected from the top pop_size *  p_i members
        p_best_ind = sorted_array[rand() % p_num];
        operateCurrentToPBest1BinWithArchive(pop, &children[target][0], target, p_best_ind, pop_sf[target], pop_cr[target], archive, arc_ind_count);
    }

    // evaluate the children's fitness values
    evaluatePopulation(children, children_fitness);

    /////////////////////////////////////////////////////////////////////////
    //update the bsf-solution and check the current number of fitness evaluations
    // if the current number of fitness evaluations over the max number of fitness evaluations, the search is terminated
    for (int i = 0; i < pop_size; i++) {
        nfes++;
        //following the rules of CEC 2014 real parameter competition, 
        //if the gap between the error values of the best solution found and the optimal solution was 10^{−8} or smaller,
        //the error was treated as 0
        if ((children_fitness[i] - optimum) < epsilon) children_fitness[i] = optimum;
        if (children_fitness[i] < bsf_fitness) {
  	        bsf_fitness = children_fitness[i];
            for (int j = 0; j < problem_size; j ++) bsf_solution[j] = children[i][j];
        }
        if(nfes>=floor(pow(problem_size,double(k)/5.0-3.0)*max_num_evaluations)) {
			records[run_id][k]=min(bsf_general_fitness,bsf_fitness)-optimum;
			//cout<<k<<": "<<records[run_id][k]<<endl;
			k++;
	    }
        if (nfes >= max_num_evaluations) break;
    }
    ////////////////////////////////////////////////////////////////////////////

    //generation alternation
    for (int i = 0; i < pop_size; i++) {
        if (children_fitness[i] == fitness[i]) {
      	    fitness[i] = children_fitness[i];
        	for (int j = 0; j < problem_size; j ++) pop[i][j] = children[i][j];
        }
        else if (children_fitness[i] < fitness[i]) {
        //parent vectors x_i which were worse than the trial vectors u_i are preserved
		    if (arc_size > 1) { 
	            if (arc_ind_count < arc_size) {
	                for (int j = 0; j < problem_size; j++) archive[arc_ind_count][j] = pop[i][j];
	                arc_ind_count++;
	            }
	    //Whenever the size of the archive exceeds, randomly selected elements are deleted to make space for the newly inserted elements
	            else {
	                random_selected_arc_ind = rand() % arc_size;
	                for (int j = 0; j < problem_size; j++) archive[random_selected_arc_ind][j] = pop[i][j];	    
	            }
	        }
	        dif_fitness.push_back(fabs(fitness[i] - children_fitness[i]));
         	fitness[i] = children_fitness[i];
         	for (int j = 0; j < problem_size; j ++) pop[i][j] = children[i][j];
  	
	        //successful parameters are preserved in S_F and S_CR
	        success_sf.push_back(pop_sf[i]);
	        success_cr.push_back(pop_cr[i]);

        }
    }
  
    num_success_params = success_sf.size();


    // if numeber of successful parameters > 0, historical memories are updated 
    if (num_success_params > 0) {      
        memory_sf[memory_pos] = 0;
        memory_cr[memory_pos] = 0;
        temp_sum_sf = 0;
        temp_sum_cr = 0;
        sum = 0;
      
        for (int i = 0; i < num_success_params; i++) sum += dif_fitness[i];

        //weighted lehmer mean
        for (int i = 0; i < num_success_params; i++) {
            weight = dif_fitness[i] / sum;

        	memory_sf[memory_pos] += weight * success_sf[i] * success_sf[i];
	        temp_sum_sf += weight * success_sf[i];

	        memory_cr[memory_pos] += weight * success_cr[i] * success_cr[i];
	        temp_sum_cr += weight * success_cr[i];
        }  

        memory_sf[memory_pos] /= temp_sum_sf;

        if (temp_sum_cr == 0 || memory_cr[memory_pos] == -1) memory_cr[memory_pos] = -1;
        else memory_cr[memory_pos] /= temp_sum_cr;

        //increment the counter
        memory_pos++;
        if (memory_pos >= memory_size) memory_pos = 0;

        //clear out the S_F, S_CR and delta fitness
        success_sf.clear();
        success_cr.clear();
        dif_fitness.clear();
    }

    // calculate the population size in the next generation
    //plan_pop_size = round((((min_pop_size - max_pop_size) / (double)max_num_evaluations) * nfes) + max_pop_size);
    plan_pop_size = round((min_pop_size - max_pop_size)*pow((double(nfes)/double(max_num_evaluations)),(1.0-double(nfes)/double(max_num_evaluations)))+max_pop_size);
    if (pop_size > plan_pop_size) {
        reduction_ind_num = pop_size - plan_pop_size;
        if (pop_size - reduction_ind_num <  min_pop_size) reduction_ind_num = pop_size - min_pop_size;

        reducePopulationWithSort(pop, fitness);     

        // resize the archive size 
        arc_size = pop_size * g_arc_rate;
        if (arc_ind_count > arc_size) arc_ind_count = arc_size;

        // resize the number of p-best individuals
        p_num = round(pop_size *  p_best_rate);
        if (p_num <= 1)  p_num = 2;
    }
}
//end loop

    if(bsf_general_fitness>bsf_fitness) {
  	    bsf_general_fitness = bsf_fitness;
  	    for (int j = 0; j < problem_size; j ++) {
		  bsf_general_solution[j] = bsf_solution[j];
	    }
	}

	//clear memory
	bsf_fitness_generations.clear();
    bsf_solution_generations.clear(); 
    pop.clear();
    fitness.clear();
    children_fitness.clear();
    children.clear();
    return bsf_general_fitness - optimum;
}


void MLS_LSHADE::operateCurrentToPBest1BinWithArchive(const vector<Individual> &pop, Individual child, int &target, int &p_best_individual, variable &scaling_factor, variable &cross_rate, const vector<Individual> &archive, int &arc_ind_count) {
  int r1, r2;
  
  do {
    r1 = rand() % pop_size;
  } while (r1 == target);
  do {
    r2 = rand() % (pop_size + arc_ind_count);
  } while ((r2 == target) || (r2 == r1));

  int random_variable = rand() % problem_size;
  
  if (r2 >= pop_size) {
    r2 -= pop_size;
    for (int i = 0; i < problem_size; i++) {
      if ((randDouble() < cross_rate) || (i == random_variable)) {
	child[i] = pop[target][i] + scaling_factor * (pop[p_best_individual][i] - pop[target][i]) + scaling_factor * (pop[r1][i] - archive[r2][i]);
      }
      else {
	child[i] = pop[target][i];
      }
    }
  }
  else {
    for (int i = 0; i < problem_size; i++) {
      if ((randDouble() < cross_rate) || (i == random_variable)) {
	child[i] = pop[target][i] + scaling_factor * (pop[p_best_individual][i] - pop[target][i]) + scaling_factor * (pop[r1][i] - pop[r2][i]);
      }
      else {
	child[i] = pop[target][i];
      }
    }
  }

  //If the mutant vector violates bounds, the bound handling method is applied
  modifySolutionWithParentMedium(child,  pop[target]);
}

void MLS_LSHADE::reducePopulationWithSort(vector<Individual> &pop, vector<Fitness> &fitness) {
  int worst_ind;

  for (int i = 0; i < reduction_ind_num; i++) {
    worst_ind = 0;
    for (int j = 1; j < pop_size; j++) {
      if (fitness[j] > fitness[worst_ind]) worst_ind = j;
    }

    pop.erase(pop.begin() + worst_ind);
    fitness.erase(fitness.begin() + worst_ind);
    pop_size--;
  }
}

void  MLS_LSHADE::setMLS_LSHADEParameters() {
  arc_rate = g_arc_rate;
  arc_size = (int)round(pop_size * arc_rate);
  p_best_rate = g_p_best_rate;
  memory_size = g_memory_size;
}
