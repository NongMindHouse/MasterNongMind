import subprocess
from tqdm import tqdm
import pandas as pd
import os
from datetime import datetime

def run_c_file(file_path, parameters):
    try:
        # Compile the C file
        compile_process = subprocess.run(["gcc", file_path, "-o", "executable"])
        if compile_process.returncode != 0:
            raise Exception(f"Compilation failed with return code {compile_process.returncode}")

        # Run the compiled executable
        run_process = subprocess.run(["./executable"] + parameters, capture_output=True, text=True)
        if run_process.returncode != 0:
            raise Exception(f"Execution failed with return code {run_process.returncode}")

        # Capture and return the last output
        output_lines = run_process.stdout.strip().split('\n')
        last_output = output_lines[-1] if output_lines else None

        return last_output

    except Exception as e:
        print(f"Error: {e}")
        return None

if __name__ == '__main__':
  # Replace "your_c_file.c" with the path to your C file
  c_file_path = "mm-genetic.c"

  note = input('note: ')
  # if input('4 or 5: ') == '5':
  c_file_path = "mm-ga-pretty.c"
  
  print('Using ', c_file_path)

  test_size = 10
  res = []
  not_win = 0

  parameters = [
    # 'CODE_LENGTH'
     '5',
    #  'COLORS'
     '8',
    #  'MAX_GUESS'
     '10',
    #  'POPULATION_LENGTH'
     '300',
    #  'MAX_GEN'
     '200',
    #  'MUTATION_RATE'
     '0.1'
  ]

  for _ in tqdm(range(test_size)):
    last_output = run_c_file(c_file_path, parameters=parameters)
    
    if last_output == 'oh no. not win':
      not_win += 1
      continue

    res.append(int(last_output.split(' ')[3]))

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
  file_path = os.path.join("Archives", f"{timestamp}.txt")

  with open(file_path,'w') as f:
    f.write(f"Note\t{note}")
    f.write(f"T.Size\t{test_size}")
    f.write(f"Mean\t{round(res.mean(),4)}")
    f.write(f"Median\t{res.median()}")
    f.write(f"Mode\t{res.mode().iloc[0]}")
    f.write(f"SD\t{round(res.std(),4)}")
    f.write(f"Max\t{res.max()}")
    f.write(f"Min\t{res.min()}")
    f.write(f"W.RATE \t{round(test_size - not_win)*100/test_size}% ({not_win} LOSES)")
    for r in res:
      f.write(str(r) + ',')
