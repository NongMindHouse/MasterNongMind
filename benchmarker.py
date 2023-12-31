import subprocess
from tqdm import tqdm
import pandas as pd
import os
from datetime import datetime
import time
import random

def run_c_file(file_path, parameters=[]):
    try:
        # Run the compiled executable
        command = ["./executable"] + [str(random.randint(0,1e4))] + parameters + ['0']
        run_process = subprocess.run(command, capture_output=True, text=True)
        if run_process.returncode != 0:
            raise Exception(f"Execution failed with return code {run_process.returncode}\n{run_process.stderr}")

        # Capture and return the last output
        output_lines = run_process.stdout.strip().split('\n')

        # for output in output_lines:
        #   print(output)

        last_output = output_lines[-1] if output_lines else None

        return last_output

    except Exception as e:
        print(f"Error: {e}")
        return None

def benchmarkExceution(src_path, export_dir, params, test_size, progress=True):
    not_win = 0 
    res = []
    # Compile the C file
    compile_process = subprocess.run(["gcc", src_path, "-o", "executable"])
    if compile_process.returncode != 0:
        raise Exception(f"Compilation failed with return code {compile_process.returncode}")

    startTime = time.time()

    if progress:
      range_process = tqdm(range(test_size))
    else:
       range_process = range(test_size)

    for _ in range_process:
    # for i in range(test_size):
      last_output = run_c_file(src_path, parameters=params)

      # print(f'{i} | ',end='')
      # print(last_output)

      if last_output is None:
        print("Failed to run the C file.")
        continue
      
      if last_output == 'oh no. not win':
        not_win += 1
        continue

      res.append(int(last_output.split(' ')[3]))

    elaspedTime = time.time() - startTime

    # Create a pandas Series from the fitness list
    res = pd.Series(res)

    not_win += res[res > 10].size

    # Display the results
    print(f"T.Size\t{test_size}")
    print(f"Mean\t{round(res.mean(),4)}")
    print(f"Median\t{res.median()}")
    print(f"Mode\t{res.mode().iloc[0]}")  # Mode may have multiple values, so we take the first one
    print(f"SD\t{round(res.std(),4)}")
    print(f"Max\t{res.max()}")
    print(f"Min\t{res.min()}")
    print(f"W.RATE \t{round(test_size - not_win)*100/test_size}% ({not_win} LOSES)")

    timestamp = datetime.now().strftime("%Y-%m-%d_%H-%M-%S")

    filename = f"{params[0]}{params[1]}-MGU{params[2]}-POP{params[3]}-MGEN{params[4]}-MUT{round(float(params[5])*100)}.txt"
    
    if not os.path.exists(export_dir):
       os.makedirs(export_dir)
       
    file_path = os.path.join(f"{export_dir}", filename)

    with open(file_path,'w') as f:
      f.write(timestamp)
      f.write(f"\nTime Elasped\t{round(elaspedTime,2)} sec\n")
      # f.write(f"Note\t{note}\n")
      f.write(f"T.Size\t{test_size}\n")
      f.write(f"Mean\t{round(res.mean(),4)}\n")
      f.write(f"Median\t{res.median()}\n")
      f.write(f"Mode\t{res.mode().iloc[0]}\n")
      f.write(f"SD\t{round(res.std(),4)}\n")
      f.write(f"Max\t{res.max()}\n")
      f.write(f"Min\t{res.min()}\n")
      f.write(f"W.RATE \t{round(test_size - not_win)*100/test_size}% ({not_win} LOSES)\n\n")

      f.writelines([
      f'CODE_LENGTH:{params[0]}\n',
      f'COLORS:{params[1]}\n',
      f'MAX_GUESS:{params[2]}\n',
      f'POPULATION_LENGTH:{params[3]}\n',
      f'MAX_GEN:{params[4]}\n',
      f'MUTATION_RATE:{params[5]}\n\n'])

      for r in res:
        f.write(str(r) + ',')


if __name__ == '__main__':
  c_file_path = "mm-ga-pretty.c"
  export_path = "Benchmarks"
  print('Using ', c_file_path)
  test_size = 1000
  params = [
     '4', # CODE_LENGTH
     '6', # COLORS
     '10', # MAX_GUESS
     '500', # POPULATION_LENGTH
     '400', # MAX_GEN
     '0.2', # MUTATION_RATE
  ]
  benchmarkExceution(c_file_path,export_path, params, test_size)