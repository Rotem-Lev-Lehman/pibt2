#pragma once
#include "solver.hpp"


class TasksDispatcher
{
protected:
  MAPF_Instance* const P;  // problem instance

private:
  Nodes available_starts;
  Nodes available_goals;
  std::mt19937* const MT;   // seed for randomness

public:
  TasksDispatcher(MAPF_Instance* _P);
  ~TasksDispatcher() {}

  /**
   * Dispatches k options of (start, goal) pairs from the currently available nodes.
   * Returns 2 configs, the first for the start nodes and the second for the goals. 
   */
  void dispatch(size_t k, Config& start_config, Config& goal_config);
};

class PP_MAPFSolver : public MAPF_Solver
{
public:
  static const std::string SOLVER_NAME;

protected:
  size_t k;  // k - minimal revealed privacy per agent (the amount of optional
          // nodes we might be at each timestamp).
  int field_of_view_radius; // field_of_view_radius - 
          // each agent must keep away from other agents by at least this radius.
  bool use_dispatcher = false; // True - use the TasksDispatcher class to create the new problem to solve.
          // False - use the input problem and divide it to the different agents by the given k.
          //         Will use the agents as if each agent's mock agents are one after the other.

private:
  struct SubAgent {
    size_t id;
    Node* start;
    Node* goal;
  };

  using SubAgents = std::vector<SubAgent*>;

  // PP_MAPFSolver agent
  struct Agent {
    size_t id;
    size_t real_sub_agent_id;
    SubAgents* sub_agents;
  };
  using Agents = std::vector<Agent*>;

private:
  Agent* generate_agent(TasksDispatcher& tasks_dispatcher, size_t id);
  void generate_configs(Agents& agents, Config& start, Config& goal);
  void set_solution(Agents& agents, Plan& solution);
  Node* generate_random_node();

protected:
  void run();

public:
  PP_MAPFSolver(MAPF_Instance* _P);
  ~PP_MAPFSolver() {}

  void setParams(int argc, char* argv[]);
  static void printHelp();

  size_t getK() const { return k; };
};
