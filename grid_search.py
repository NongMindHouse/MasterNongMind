from benchmarker import benchmarkExceution
from tqdm import tqdm
import numpy as np

if __name__ == '__main__':
    c_file_path = "mm-ga-pretty.c"
    color_length = '5'
    colors_num = '8'
    max_guess = '10'
    export_folder = f"results-{color_length}-{colors_num}"
    test_size = 1000
    print("Strat Grid Search")
    print('Using ', c_file_path)

    # Loop for Grid Search 
    for population_length  in tqdm(range(50,550,50)):
        for max_gen in range(50,550,50):
            for mutation_rate in np.arange(0.05, 0.55, 0.05):
                params = [
                    color_length,        # CODE_LENGTH Fixed
                    colors_num,        # COLORS Fixed
                    max_guess,       # MAX_GUESS Fixed
                    str(population_length),  # POPULATION_LENGTH  
                    str(max_gen),                 # MAX_GEN  
                    str(mutation_rate),         # MUTATION_RATE
                ]
                benchmarkExceution(c_file_path, export_folder, params, test_size)
                