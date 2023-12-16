import subprocess
from tqdm import tqdm
import pandas as pd

def run_c_file(file_path):
    try:
        # Compile the C file
        compile_process = subprocess.run(["gcc", file_path, "-o", "executable"])
        if compile_process.returncode != 0:
            raise Exception(f"Compilation failed with return code {compile_process.returncode}")

        # Run the compiled executable
        run_process = subprocess.run(["./executable"], capture_output=True, text=True)
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
  test_size = 1000
  res = []
  not_win = 0

  for _ in tqdm(range(test_size)):
    last_output = run_c_file(c_file_path)
    
    if last_output == 'oh no. not win':
      not_win += 1
      continue

    res.append(int(last_output.split(' ')[3]))

    # if last_output is not None:
    #     print(f"Last Output: {last_output}")
    # else:
    #     print("Failed to run the C file.")
  
  print(note)

  # Create a pandas Series from the fitness list
  res = pd.Series(res)

  # Display the results
  print(f"Mean: {res.mean()}")
  print(f"Median: {res.median()}")
  print(f"Mode: {res.mode().iloc[0]}")  # Mode may have multiple values, so we take the first one
  print(f"SD: {round(res.std(),4)}")
  print(f"Max: {res.max()}")
  print(f"Min: {res.min()}")
  print('not win:',not_win)
