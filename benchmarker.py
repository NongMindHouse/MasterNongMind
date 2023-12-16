import subprocess
from tqdm import tqdm
import pandas as pd
import os
from datetime import datetime

def run_c_file(file_path, parameters=[]):
    try:
        # Compile the C file
        # compile_process = subprocess.run(["gcc", file_path, "-o", "executable"])
        # if compile_process.returncode != 0:
        #     raise Exception(f"Compilation failed with return code {compile_process.returncode}")

        # Run the compiled executable
        command = ["./executable"] + parameters + ['0']
        run_process = subprocess.run(command, capture_output=True, text=True)
        if run_process.returncode != 0:
            raise Exception(f"Execution failed with return code {run_process.returncode}\n{run_process.stderr}")

        # Capture and return the last output
        output_lines = run_process.stdout.strip().split('\n')
        last_output = output_lines[-1] if output_lines else None

        return last_output

    except Exception as e:
        print(f"Error: {e}")
        return None

if __name__ == '__main__':
  # Replace "your_c_file.c" with the path to your C file

  # note = input('note: ')
  # if input('4 or 5: ') == '5':
  c_file_path = "mm-ga-pretty.c"
  
  print('Using ', c_file_path)

  test_size = 1000
  res = []
  not_win = 0

  # BEST COMBINATION (FOR NOW)
  #    '4', # CODE_LENGTH
  #    '6', # COLORS
  #    '10', # MAX_GUESS
  #    '150', # POPULATION_LENGTH
  #    '100', # MAX_GEN
  #    '0.10', # MUTATION_RATE

  #    '5', # CODE_LENGTH
  #    '8', # COLORS
  #    '10', # MAX_GUESS
  #    '300', # POPULATION_LENGTH
  #    '200', # MAX_GEN
  #    '0.15', # MUTATION_RATE

  param = [
     '4', # CODE_LENGTH
     '6', # COLORS
     '10', # MAX_GUESS
     '150', # POPULATION_LENGTH
     '100', # MAX_GEN
     '0.10', # MUTATION_RATE
  ]

  #   Compile the C file
  compile_process = subprocess.run(["gcc", c_file_path, "-o", "executable"])
  if compile_process.returncode != 0:
      raise Exception(f"Compilation failed with return code {compile_process.returncode}")

  for _ in tqdm(range(test_size)):
  # for i in range(test_size):
    last_output = run_c_file(c_file_path, parameters=param)

    # print(f'{i} | ',end='')
    # print(last_output)

    if last_output is None:
      print("Failed to run the C file.")
      continue
    
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

  filename = f"{param[0]}{param[1]}-MGU{param[2]}-POP{param[3]}-MGEN{param[4]}-MUT{round(float(param[5])*100)}.txt"
  file_path = os.path.join("Benchmarks", filename)

  with open(file_path,'w') as f:
    f.write(timestamp)
    # f.write(f"Note\t{note}\n")
    f.write(f"\nT.Size\t{test_size}\n")
    f.write(f"Mean\t{round(res.mean(),4)}\n")
    f.write(f"Median\t{res.median()}\n")
    f.write(f"Mode\t{res.mode().iloc[0]}\n")
    f.write(f"SD\t{round(res.std(),4)}\n")
    f.write(f"Max\t{res.max()}\n")
    f.write(f"Min\t{res.min()}\n")
    f.write(f"W.RATE \t{round(test_size - not_win)*100/test_size}% ({not_win} LOSES)\n\n")

    f.writelines([
    f'CODE_LENGTH:{param[0]}\n',
    f'COLORS:{param[1]}\n',
    f'MAX_GUESS:{param[2]}\n',
    f'POPULATION_LENGTH:{param[3]}\n',
    f'MAX_GEN:{param[4]}\n',
    f'MUTATION_RATE:{param[5]}\n\n'])

    for r in res:
      f.write(str(r) + ',')
