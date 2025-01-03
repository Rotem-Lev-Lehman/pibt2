import argparse
import re

STARTS_LABEL = "starts="
GOALS_LABEL = "goals="
SOLUTION_LABEL = "solution="

def parse_input() -> tuple[str, str]:
    parser = argparse.ArgumentParser()
    parser.add_argument("results_file", help="the path to the results file to process.", type=str)
    parser.add_argument("output_file", help="the path to the processed file.", type=str)
    args = parser.parse_args()
    return (args.results_file, args.output_file)

if __name__ == "__main__":
    results_file, output_file = parse_input()
    parsing_solution = False
    solution = []
    last_line_index = -1
    with open(results_file, "r") as res:
        for line in res.readlines():
            if not parsing_solution:
                if line.startswith(STARTS_LABEL):
                    start_positions = line.split(STARTS_LABEL)[1]
                elif line.startswith(GOALS_LABEL):
                    goal_positions = line.split(GOALS_LABEL)[1]
                elif line.startswith(SOLUTION_LABEL):
                    parsing_solution = True
            else:
                solution.append(line)
                last_line_index = int(line.split(":")[0])
    goals_line = f"{last_line_index + 1}:{goal_positions}"
    solution.append(goals_line)
    with open(output_file, "w") as out:
        out.writelines(solution)
